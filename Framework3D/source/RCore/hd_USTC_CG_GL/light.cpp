#include "light.h"

#include "RCore/internal/gl/GLResources.hpp"
#include "Utils/Logging/Logging.h"
#include "pxr/imaging/glf/simpleLight.h"
#include "pxr/imaging/hd/changeTracker.h"
#include "pxr/imaging/hd/rprimCollection.h"
#include "pxr/imaging/hd/sceneDelegate.h"
#include "pxr/imaging/hio/image.h"
#include "pxr/usd/sdr/shaderNode.h"
#include "pxr/usd/usd/tokens.h"
#include "pxr/usdImaging/usdImaging/tokens.h"

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

    // HdStLight communicates to the scene graph and caches all interesting
    // values within this class. Later on Get() is called from
    // TaskState (RenderPass) to perform aggregation/pre-computation,
    // in order to make the shader execution efficient.

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
        auto transform = Get(HdTokens->transform).GetWithDefault<GfMatrix4d>();
        // Update cached light objects.  Note that simpleLight ignores
        // scene-delegate transform, in favor of the transform passed in by
        // params...
        if (_lightType == HdPrimTypeTokens->domeLight) {
            // Apply domeOffset if present
            VtValue domeOffset = sceneDelegate->GetLightParamValue(id, HdLightTokens->domeOffset);
            if (domeOffset.IsHolding<GfMatrix4d>()) {
                transform = domeOffset.UncheckedGet<GfMatrix4d>() * transform;
            }
            auto light = Get(HdLightTokens->params).GetWithDefault<GlfSimpleLight>();
            light.SetTransform(transform);
            _params[HdLightTokens->params] = VtValue(light);
        }
        else if (_lightType != HdPrimTypeTokens->simpleLight) {
            // e.g. area light
            auto light = Get(HdLightTokens->params).GetWithDefault<GlfSimpleLight>();
            GfVec3d p = transform.ExtractTranslation();
            GfVec4f pos(p[0], p[1], p[2], 1.0f);
            // Convention is to emit light along -Z
            GfVec4d zDir = transform.GetRow(2);
            if (_lightType == HdPrimTypeTokens->rectLight ||
                _lightType == HdPrimTypeTokens->diskLight) {
                light.SetSpotDirection(GfVec3f(-zDir[0], -zDir[1], -zDir[2]));
            }
            else if (_lightType == HdPrimTypeTokens->distantLight) {
                // For a distant light, translate to +Z homogeneous limit
                // See simpleLighting.glslfx : integrateLightsDefault.
                pos = GfVec4f(zDir[0], zDir[1], zDir[2], 0.0f);
            }
            else if (_lightType == HdPrimTypeTokens->sphereLight) {
                _params[HdLightTokens->radius] =
                    sceneDelegate->GetLightParamValue(id, HdLightTokens->radius);
            }
            auto diffuse =
                sceneDelegate->GetLightParamValue(id, HdLightTokens->diffuse).Get<float>();
            auto color =
                sceneDelegate->GetLightParamValue(id, HdLightTokens->color).Get<GfVec3f>() *
                diffuse;
            light.SetDiffuse(GfVec4f(color[0], color[1], color[2], 0));
            light.SetPosition(pos);
            _params[HdLightTokens->params] = VtValue(light);
        }
    }

    // Shadow Params
    if (bits & DirtyShadowParams) {
        _params[HdLightTokens->shadowParams] =
            sceneDelegate->GetLightParamValue(id, HdLightTokens->shadowParams);
    }

    // Shadow Collection
    if (bits & DirtyCollection) {
        VtValue vtShadowCollection =
            sceneDelegate->GetLightParamValue(id, HdLightTokens->shadowCollection);

        // Optional
        if (vtShadowCollection.IsHolding<HdRprimCollection>()) {
            auto newCollection = vtShadowCollection.UncheckedGet<HdRprimCollection>();

            if (_params[HdLightTokens->shadowCollection] != newCollection) {
                _params[HdLightTokens->shadowCollection] = VtValue(newCollection);

                HdChangeTracker& changeTracker = sceneDelegate->GetRenderIndex().GetChangeTracker();

                changeTracker.MarkCollectionDirty(newCollection.GetName());
            }
        }
        else {
            _params[HdLightTokens->shadowCollection] = VtValue(HdRprimCollection());
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
        _lightType == HdPrimTypeTokens->distantLight) {
        return AllDirty;
    }
    else {
        return (DirtyParams | DirtyTransform);
    }
}

VtValue Hd_USTC_CG_Light::Get(const TfToken& token) const
{
    VtValue val;
    TfMapLookup(_params, token, &val);
    return val;
}

GLuint Hd_USTC_CG_Dome_Light::createTextureFromHioImage(const InputDescriptor& env_texture)
{
    // Step 4: Create an OpenGL texture object
    GLuint texture;
    glGenTextures(1, &texture);

    // Step 5: Bind the texture object and specify its parameters
    glBindTexture(GL_TEXTURE_2D, texture);

    auto image = env_texture.image;
    if (image) {
        // Step 1: Get image information
        int width = image->GetWidth();
        int height = image->GetHeight();
        HioFormat format = image->GetFormat();

        HioImage::StorageSpec storageSpec;
        storageSpec.width = width;
        storageSpec.height = height;
        storageSpec.format = format;
        storageSpec.data = malloc(width * height * image->GetBytesPerPixel());
        if (!storageSpec.data) {
            return 0;
        }

        // Step 3: Read the image data
        if (!image->Read(storageSpec)) {
            free(storageSpec.data);
            return 0;
        }

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GetGLInternalFormat(format),
            width,
            height,
            0,
            GetGLFormat(format),
            GetGLType(format),
            storageSpec.data);
        free(storageSpec.data);
    }
    else {
        auto val = radiance;
        float color[4] = { val[0], val[1], val[2], 1.0f };

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GetGLInternalFormat(HioFormatFloat32Vec4),
            1,
            1,
            0,
            GetGLFormat(HioFormatFloat32Vec4),
            GetGLType(HioFormatFloat32Vec4),
            color);
    }
    glGenerateMipmap(GL_TEXTURE_2D);

    float aniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

void Hd_USTC_CG_Dome_Light::RefreshGLBuffer()
{
    if (env_texture.glTexture == 0) {
        env_texture.glTexture = createTextureFromHioImage(env_texture);
    }
}

void Hd_USTC_CG_Dome_Light::_PrepareDomeLight(SdfPath const& id, HdSceneDelegate* sceneDelegate)
{
    const VtValue v = sceneDelegate->GetLightParamValue(id, HdLightTokens->textureFile);
    textureFileName = v.Get<pxr::SdfAssetPath>();

    env_texture.image = HioImage::OpenForReading(textureFileName.GetAssetPath(), 0, 0);

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

void Hd_USTC_CG_Dome_Light::Finalize(HdRenderParam* renderParam)
{
    Hd_USTC_CG_Light::Finalize(renderParam);
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
