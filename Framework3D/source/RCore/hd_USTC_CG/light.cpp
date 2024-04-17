#include "light.h"

#include "Utils/Logging/Logging.h"
#include "pxr/base/gf/plane.h"
#include "pxr/base/gf/ray.h"
#include "pxr/base/gf/rotation.h"
#include "pxr/base/gf/vec2f.h"
#include "pxr/imaging/glf/simpleLight.h"
#include "pxr/imaging/hd/changeTracker.h"
#include "pxr/imaging/hd/rprimCollection.h"
#include "pxr/imaging/hd/sceneDelegate.h"
#include "pxr/imaging/hio/image.h"
#include "renderParam.h"
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
    static_cast<Hd_USTC_CG_RenderParam*>(renderParam)->AcquireSceneForEdit();

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

        // Add new dependencies
        val = Get(HdTokens->filters);
        if (val.IsHolding<SdfPathVector>()) {
            auto lightFilterPaths = val.UncheckedGet<SdfPathVector>();
            for (const SdfPath& filterPath : lightFilterPaths) {
                changeTracker.AddSprimSprimDependency(filterPath, id);
            }
        }
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

void Hd_USTC_CG_Light::Finalize(HdRenderParam* renderParam)
{
    static_cast<Hd_USTC_CG_RenderParam*>(renderParam)->AcquireSceneForEdit();

    HdLight::Finalize(renderParam);
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
    if (texture != nullptr) {
        auto uv = GfVec2f((M_PI + std::atan2(dir[1], dir[0])) / 2.0 / M_PI, 0.5 - dir[2] * 0.5);

        auto value = texture->Evaluate(uv);

        if (texture->component_conut() >= 3) {
            return GfCompMult(Color{ value[0], value[1], value[2] }, radiance);
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

// HW7_TODO: write the following, you should refer to the sphere light.

void Hd_USTC_CG_Distant_Light::Sync(
    HdSceneDelegate* sceneDelegate,
    HdRenderParam* renderParam,
    HdDirtyBits* dirtyBits)
{
    Hd_USTC_CG_Light::Sync(sceneDelegate, renderParam, dirtyBits);
    auto id = GetId();
    angle = sceneDelegate->GetLightParamValue(id, HdLightTokens->angle).Get<float>();
    angle = std::clamp(angle, 0.03f, 89.9f) * M_PI / 180.0f;

    auto diffuse = sceneDelegate->GetLightParamValue(id, HdLightTokens->diffuse).Get<float>();
    radiance = sceneDelegate->GetLightParamValue(id, HdLightTokens->color).Get<GfVec3f>() *
               diffuse / (1 - cos(angle)) / 2.0 / M_PI;

    auto transform = Get(HdTokens->transform).GetWithDefault<GfMatrix4d>();

    direction = transform.TransformDir(GfVec3f(0, 0, -1)).GetNormalized();
}

Color Hd_USTC_CG_Distant_Light::Sample(
    const GfVec3f& pos,
    GfVec3f& dir,
    GfVec3f& sampled_light_pos,
    float& sample_light_pdf,
    const std::function<float()>& uniform_float)
{
    float theta = uniform_float() * angle;
    float phi = uniform_float() * 2 * M_PI;

    auto sampled_dir = GfVec3f(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

    auto basis = constructONB(-direction);

    dir = basis * sampled_dir;
    sampled_light_pos = pos + dir * std::numeric_limits<float>::max() / 100.f;

    sample_light_pdf = 1.0f / sin(theta) / (2.0f * M_PI * angle);

    return radiance;
}

Color Hd_USTC_CG_Distant_Light::Intersect(const GfRay& ray, float& depth)
{
    depth = std::numeric_limits<float>::max() / 100.f;

    if (GfDot(ray.GetDirection().GetNormalized(), -direction) > cos(angle)) {
        return radiance;
    }
    return Color(0);
}

Color Hd_USTC_CG_Rect_Light::Sample(
    const GfVec3f& pos,
    GfVec3f& dir,
    GfVec3f& sampled_light_pos,
    float& sample_light_pdf,
    const std::function<float()>& uniform_float)
{
    return {};
}

Color Hd_USTC_CG_Rect_Light::Intersect(const GfRay& ray, float& depth)
{
    return {};
}

void Hd_USTC_CG_Rect_Light::Sync(
    HdSceneDelegate* sceneDelegate,
    HdRenderParam* renderParam,
    HdDirtyBits* dirtyBits)
{
    Hd_USTC_CG_Light::Sync(sceneDelegate, renderParam, dirtyBits);

    auto transform = Get(HdTokens->transform).GetWithDefault<GfMatrix4d>();

    auto id = GetId();
    width = sceneDelegate->GetLightParamValue(id, HdLightTokens->width).Get<float>();
    height = sceneDelegate->GetLightParamValue(id, HdLightTokens->height).Get<float>();

    corner0 = transform.TransformAffine(GfVec3f(-0.5 * width, -0.5 * height, 0));
    corner1 = transform.TransformAffine(GfVec3f(-0.5 * width, 0.5 * height, 0));
    corner2 = transform.TransformAffine(GfVec3f(0.5 * width, -0.5 * height, 0));
    corner3 = transform.TransformAffine(GfVec3f(0.5 * width, 0.5 * height, 0));

    auto diffuse = sceneDelegate->GetLightParamValue(id, HdLightTokens->diffuse).Get<float>();
    power = sceneDelegate->GetLightParamValue(id, HdLightTokens->color).Get<GfVec3f>() * diffuse;

    // HW7_TODO: calculate irradiance
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
