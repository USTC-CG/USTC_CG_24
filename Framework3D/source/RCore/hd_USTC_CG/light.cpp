#include "light.h"

#include "Utils/Logging/Logging.h"
#include "pxr/base/gf/plane.h"
#include "pxr/base/gf/ray.h"
#include "pxr/base/gf/vec2f.h"
#include "pxr/imaging/glf/simpleLight.h"
#include "pxr/imaging/hd/changeTracker.h"
#include "pxr/imaging/hd/rprimCollection.h"
#include "pxr/imaging/hd/sceneDelegate.h"
#include "pxr/imaging/hio/image.h"
#include "texture.h"
#include "utils/math.hpp"
#include "utils/sampling.hpp"

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

    if (!TF_VERIFY(sceneDelegate != nullptr)) {
        return;
    }

    const SdfPath& id = GetId();

    // Change tracking
    HdDirtyBits bits = *dirtyBits;

    // Transform
    if (bits & DirtyTransform) {
        _params[HdTokens->transform] = VtValue(sceneDelegate->GetTransform(id));
    }

    // Lighting Params
    if (bits & DirtyParams) {
        HdChangeTracker& changeTracker = sceneDelegate->GetRenderIndex().GetChangeTracker();

        // Remove old dependencies
        VtValue val = Get(HdTokens->filters);
        if (val.IsHolding<SdfPathVector>()) {
            auto lightFilterPaths = val.UncheckedGet<SdfPathVector>();
            for (const SdfPath& filterPath : lightFilterPaths) {
                changeTracker.RemoveSprimSprimDependency(filterPath, id);
            }
        }

        if (_lightType == HdPrimTypeTokens->simpleLight) {
            _params[HdLightTokens->params] = sceneDelegate->Get(id, HdLightTokens->params);
        }
        // else if (_lightType == HdPrimTypeTokens->domeLight)
        //{
        //     _params[HdLightTokens->params] =
        //         _PrepareDomeLight(id, sceneDelegate);
        // }
        //// If it is an area light we will extract the parameters and convert
        //// them to a GlfSimpleLight that approximates the light source.
        // else
        //{
        //     _params[HdLightTokens->params] =
        //         _ApproximateAreaLight(id, sceneDelegate);
        // }

        // Add new dependencies
        val = Get(HdTokens->filters);
        if (val.IsHolding<SdfPathVector>()) {
            auto lightFilterPaths = val.UncheckedGet<SdfPathVector>();
            for (const SdfPath& filterPath : lightFilterPaths) {
                changeTracker.AddSprimSprimDependency(filterPath, id);
            }
        }
    }

    if (bits & (DirtyTransform | DirtyParams)) {
        // Update cached light objects.  Note that simpleLight ignores
        // scene-delegate transform, in favor of the transform passed in by
        // params...
        // if (_lightType == HdPrimTypeTokens->domeLight) {
        //    // Apply domeOffset if present
        //    VtValue domeOffset = sceneDelegate->GetLightParamValue(id, HdLightTokens->domeOffset);
        //    if (domeOffset.IsHolding<GfMatrix4d>()) {
        //        transform = domeOffset.UncheckedGet<GfMatrix4d>() * transform;
        //    }
        //    auto light = Get(HdLightTokens->params).GetWithDefault<GlfSimpleLight>();
        //    light.SetTransform(transform);
        //    _params[HdLightTokens->params] = VtValue(light);
        //}
        // else if (_lightType != HdPrimTypeTokens->simpleLight)
        //{
        //    // e.g. area light
        //    auto light = Get(HdLightTokens->params).GetWithDefault<GlfSimpleLight>();
        //    GfVec3d p = transform.ExtractTranslation();
        //    GfVec4f pos(p[0], p[1], p[2], 1.0f);
        //    // Convention is to emit light along -Z
        //    GfVec4d zDir = transform.GetRow(2);
        //    if (_lightType == HdPrimTypeTokens->rectLight ||
        //        _lightType == HdPrimTypeTokens->diskLight) {
        //        light.SetSpotDirection(GfVec3f(-zDir[0], -zDir[1], -zDir[2]));
        //    }
        //    else if (_lightType == HdPrimTypeTokens->distantLight) {
        //        // For a distant light, translate to +Z homogeneous limit
        //        // See simpleLighting.glslfx : integrateLightsDefault.
        //        pos = GfVec4f(zDir[0], zDir[1], zDir[2], 0.0f);
        //    }
        //    else if (_lightType == HdPrimTypeTokens->sphereLight) {
        //    }

        //    light.SetDiffuse(GfVec4f(color[0], color[1], color[2], 0));
        //    light.SetPosition(pos);
        //    _params[HdLightTokens->params] = VtValue(light);
        //}
    }

    *dirtyBits = Clean;
}

HdDirtyBits Hd_USTC_CG_Light::GetInitialDirtyBitsMask() const
{
    if (_lightType == HdPrimTypeTokens->simpleLight ||
        _lightType == HdPrimTypeTokens->distantLight) {
        return AllDirty;
    }
    else {
        return (DirtyParams | DirtyTransform);
    }
}

bool Hd_USTC_CG_Light::IsDomeLight()
{
    return _lightType == HdPrimTypeTokens->domeLight;
}

VtValue Hd_USTC_CG_Light::Get(const TfToken& token) const
{
    VtValue val;
    TfMapLookup(_params, token, &val);
    return val;
}

Color Hd_USTC_CG_Sphere_Light::Sample(
    const GfVec3f& pos,
    GfVec3f& dir,
    GfVec3f& sampled_light_pos,

    float& sample_light_pdf,
    const std::function<float()>& uniform_float)
{
    auto distanceVec = position - pos;

    auto basis = constructONB(-distanceVec.GetNormalized());

    auto distance = distanceVec.GetLength();

    // A sphere light is treated as all points on the surface spreads energy uniformly:
    float sample_pos_pdf;
    // First we sample a point on the hemi sphere:
    auto sampledDir =
        CosineWeightedDirection(GfVec2f(uniform_float(), uniform_float()), sample_pos_pdf);
    auto worldSampledDir = basis * sampledDir;

    auto sampledPosOnSurface = worldSampledDir * radius + position;
    sampled_light_pos = sampledPosOnSurface;

    // Then we can decide the direction.
    dir = (sampledPosOnSurface - pos).GetNormalized();

    // and the pdf (with the measure of solid angle):
    float cosVal = GfDot(-dir, worldSampledDir.GetNormalized());

    sample_light_pdf = sample_pos_pdf / radius / radius * cosVal * distance * distance;

    // Finally we calculate the radiance
    if (cosVal < 0) {
        return Color{ 0 };
    }
    return irradiance * cosVal / M_PI;
}

Color Hd_USTC_CG_Sphere_Light::Intersect(const GfRay& ray, float& depth)
{
    double distance;
    if (ray.Intersect(GfRange3d{ position - GfVec3d{ radius }, position + GfVec3d{ radius } })) {
        if (ray.Intersect(position, radius, &distance)) {
            depth = distance;

            return irradiance / M_PI;
        }
    }
    depth = std::numeric_limits<float>::infinity();
    return { 0, 0, 0 };
}

void Hd_USTC_CG_Sphere_Light::Sync(
    HdSceneDelegate* sceneDelegate,
    HdRenderParam* renderParam,
    HdDirtyBits* dirtyBits)
{
    Hd_USTC_CG_Light::Sync(sceneDelegate, renderParam, dirtyBits);
    auto id = GetId();

    radius = sceneDelegate->GetLightParamValue(id, HdLightTokens->radius).Get<float>();

    auto diffuse = sceneDelegate->GetLightParamValue(id, HdLightTokens->diffuse).Get<float>();
    power = sceneDelegate->GetLightParamValue(id, HdLightTokens->color).Get<GfVec3f>() * diffuse;

    auto transform = Get(HdTokens->transform).GetWithDefault<GfMatrix4d>();

    GfVec3d p = transform.ExtractTranslation();
    position = GfVec3f(p[0], p[1], p[2]);

    area = 4 * M_PI * radius * radius;

    irradiance = power / area;
}

Color Hd_USTC_CG_Dome_Light::Sample(
    const GfVec3f& pos,
    GfVec3f& dir,
    GfVec3f& sampled_light_pos,
    float& sample_light_pdf,
    const std::function<float()>& uniform_float)
{
    dir = UniformSampleSphere(GfVec2f{ uniform_float(), uniform_float() }, sample_light_pdf);
    sampled_light_pos = dir * std::numeric_limits<float>::max() / 100.f;

    return Le(dir);
}

Color Hd_USTC_CG_Dome_Light::Intersect(const GfRay& ray, float& depth)
{
    depth = std::numeric_limits<float>::max() / 100.f;  // max is smaller than infinity, lol

    return Le(GfVec3f(ray.GetDirection()));
}

void Hd_USTC_CG_Dome_Light::_PrepareDomeLight(SdfPath const& id, HdSceneDelegate* sceneDelegate)
{
    const VtValue v = sceneDelegate->GetLightParamValue(id, HdLightTokens->textureFile);
    if (!v.IsEmpty()) {
        if (v.IsHolding<SdfAssetPath>()) {
            textureFileName = v.UncheckedGet<SdfAssetPath>();
            texture = std::make_unique<Texture2D>(textureFileName);
            if (!texture->isValid()) {
                texture = nullptr;
            }

            logging("Attempting to load file " + textureFileName.GetAssetPath(), Warning);
        }
        else {
            texture = nullptr;
        }
    }
    auto diffuse = sceneDelegate->GetLightParamValue(id, HdLightTokens->diffuse).Get<float>();
    radiance = sceneDelegate->GetLightParamValue(id, HdLightTokens->color).Get<GfVec3f>() * diffuse;
}

void Hd_USTC_CG_Dome_Light::Sync(
    HdSceneDelegate* sceneDelegate,
    HdRenderParam* renderParam,
    HdDirtyBits* dirtyBits)
{
    Hd_USTC_CG_Light::Sync(sceneDelegate, renderParam, dirtyBits);

    auto id = GetId();
    _PrepareDomeLight(id, sceneDelegate);
}

Color Hd_USTC_CG_Dome_Light::Le(const GfVec3f& dir)
{
    if (texture) {
        auto uv = GfVec2f((M_PI + std::atan2(dir[1], dir[0])) / 2.0 / M_PI, 0.5 - dir[2] * 0.5);

        auto value = texture->Evaluate(uv);

        if (texture->component_conut() >= 3) {
            return GfCompMult(Color{ value[0], value[1], value[2] },radiance);
        }
    }
    else {
        return radiance;
    }
}

void Hd_USTC_CG_Dome_Light::Finalize(HdRenderParam* renderParam)
{
    texture = nullptr;
    Hd_USTC_CG_Light::Finalize(renderParam);
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
