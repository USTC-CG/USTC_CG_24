#include "light.h"

#include "Utils/Logging/Logging.h"
#include "pxr/imaging/hd/changeTracker.h"
#include "pxr/imaging/hd/rprimCollection.h"
#include "pxr/imaging/hd/sceneDelegate.h"
#include "pxr/imaging/glf/simpleLight.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
void Hd_USTC_CG_Light::Sync(
    HdSceneDelegate* sceneDelegate,
    HdRenderParam* renderParam,
    HdDirtyBits* dirtyBits)
{
    TRACE_FUNCTION();
    HF_MALLOC_TAG_FUNCTION();

    TF_UNUSED(renderParam);

    if (!TF_VERIFY(sceneDelegate != nullptr))
    {
        return;
    }

    const SdfPath& id = GetId();

    // HdStLight communicates to the scene graph and caches all interesting
    // values within this class. Later on Get() is called from
    // TaskState (RenderPass) to perform aggregation/pre-computation,
    // in order to make the shader execution efficient.

    // Change tracking
    HdDirtyBits bits = *dirtyBits;

    // Transform
    if (bits & DirtyTransform)
    {
        _params[HdTokens->transform] = VtValue(sceneDelegate->GetTransform(id));
    }

    std::ostringstream val;
    val << VtValue(sceneDelegate->GetTransform(id))
        .Cast<GfMatrix4d>();

    USTC_CG::logging(
        val.str(),
        USTC_CG::Info);

    // Lighting Params
    if (bits & DirtyParams)
    {
        HdChangeTracker& changeTracker =
            sceneDelegate->GetRenderIndex().GetChangeTracker();

        // Remove old dependencies
        VtValue val = Get(HdTokens->filters);
        if (val.IsHolding<SdfPathVector>())
        {
            auto lightFilterPaths = val.UncheckedGet<SdfPathVector>();
            for (const SdfPath& filterPath : lightFilterPaths)
            {
                changeTracker.RemoveSprimSprimDependency(filterPath, id);
            }
        }

        if (_lightType == HdPrimTypeTokens->simpleLight)
        {
            _params[HdLightTokens->params] =
                sceneDelegate->Get(id, HdLightTokens->params);
        }
        //else if (_lightType == HdPrimTypeTokens->domeLight)
        //{
        //    _params[HdLightTokens->params] =
        //        _PrepareDomeLight(id, sceneDelegate);
        //}
        //// If it is an area light we will extract the parameters and convert
        //// them to a GlfSimpleLight that approximates the light source.
        //else
        //{
        //    _params[HdLightTokens->params] =
        //        _ApproximateAreaLight(id, sceneDelegate);
        //}

        // Add new dependencies
        val = Get(HdTokens->filters);
        if (val.IsHolding<SdfPathVector>())
        {
            auto lightFilterPaths = val.UncheckedGet<SdfPathVector>();
            for (const SdfPath& filterPath : lightFilterPaths)
            {
                changeTracker.AddSprimSprimDependency(filterPath, id);
            }
        }
    }

    if (bits & (DirtyTransform | DirtyParams))
    {
        auto transform =
            Get(HdTokens->transform).GetWithDefault<GfMatrix4d>();
        // Update cached light objects.  Note that simpleLight ignores
        // scene-delegate transform, in favor of the transform passed in by
        // params...
        if (_lightType == HdPrimTypeTokens->domeLight)
        {
            // Apply domeOffset if present
            VtValue domeOffset = sceneDelegate->GetLightParamValue(
                id,
                HdLightTokens->domeOffset);
            if (domeOffset.IsHolding<GfMatrix4d>())
            {
                transform = domeOffset.UncheckedGet<GfMatrix4d>() * transform;
            }
            auto light =
                Get(HdLightTokens->params).GetWithDefault<GlfSimpleLight>();
            light.SetTransform(transform);
            _params[HdLightTokens->params] = VtValue(light);
        }
        else if (_lightType != HdPrimTypeTokens->simpleLight)
        {
            // e.g. area light
            auto light =
                Get(HdLightTokens->params).GetWithDefault<GlfSimpleLight>();
            GfVec3d p = transform.ExtractTranslation();
            GfVec4f pos(p[0], p[1], p[2], 1.0f);
            // Convention is to emit light along -Z
            GfVec4d zDir = transform.GetRow(2);
            if (_lightType == HdPrimTypeTokens->rectLight ||
                _lightType == HdPrimTypeTokens->diskLight)
            {
                light.SetSpotDirection(GfVec3f(-zDir[0], -zDir[1], -zDir[2]));
            }
            else if (_lightType == HdPrimTypeTokens->distantLight)
            {
                // For a distant light, translate to +Z homogeneous limit
                // See simpleLighting.glslfx : integrateLightsDefault.
                pos = GfVec4f(zDir[0], zDir[1], zDir[2], 0.0f);
            }
            light.SetPosition(pos);
            _params[HdLightTokens->params] = VtValue(light);
        }
    }

    // Shadow Params
    if (bits & DirtyShadowParams)
    {
        _params[HdLightTokens->shadowParams] =
            sceneDelegate->GetLightParamValue(id, HdLightTokens->shadowParams);
    }

    // Shadow Collection
    if (bits & DirtyCollection)
    {
        VtValue vtShadowCollection = sceneDelegate->GetLightParamValue(
            id,
            HdLightTokens->shadowCollection);

        // Optional
        if (vtShadowCollection.IsHolding<HdRprimCollection>())
        {
            auto newCollection =
                vtShadowCollection.UncheckedGet<HdRprimCollection>();

            if (_params[HdLightTokens->shadowCollection] != newCollection)
            {
                _params[HdLightTokens->shadowCollection] = VtValue(
                    newCollection);

                HdChangeTracker& changeTracker =
                    sceneDelegate->GetRenderIndex().GetChangeTracker();

                changeTracker.MarkCollectionDirty(newCollection.GetName());
            }
        }
        else
        {
            _params[HdLightTokens->shadowCollection] =
                VtValue(HdRprimCollection());
        }
    }

    *dirtyBits = Clean;
}

HdDirtyBits Hd_USTC_CG_Light::GetInitialDirtyBitsMask() const
{
    // In the case of simple and distant lights we want to sync all dirty bits,
    // but for area lights coming from the scenegraph we just want to extract
    // the Transform and Params for now.
    if (_lightType == HdPrimTypeTokens->simpleLight ||
        _lightType == HdPrimTypeTokens->distantLight)
    {
        return AllDirty;
    }
    else
    {
        return (DirtyParams | DirtyTransform);
    }
}

VtValue Hd_USTC_CG_Light::Get(const TfToken& token) const
{
    VtValue val;
    TfMapLookup(_params, token, &val);
    return val;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
