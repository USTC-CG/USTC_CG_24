#pragma once

#include "USTC_CG.h"
#include "color.h"
#include "pxr/imaging/hd/material.h"
#include "pxr/imaging/hio/image.h"

namespace pxr {
class Hio_OpenEXRImage;

}

USTC_CG_NAMESPACE_OPEN_SCOPE
class Texture2D;
class Shader;
using namespace pxr;

class Hio_StbImage;
class Hd_USTC_CG_Material : public HdMaterial {
   public:
    struct InputDescriptor {
        std::unique_ptr<Texture2D> image = nullptr;

        TfToken wrapS;
        TfToken wrapT;

        TfToken uv_primvar_name;

        VtValue value;

        TfToken input_name;
    };

    explicit Hd_USTC_CG_Material(SdfPath const& id);

    void Sync(HdSceneDelegate* sceneDelegate, HdRenderParam* renderParam, HdDirtyBits* dirtyBits)
        override;

    HdDirtyBits GetInitialDirtyBitsMask() const override;

    TfToken requireTexcoordName();

    void Finalize(HdRenderParam* renderParam) override;
    Color Sample(const GfVec3f& wo, GfVec3f& wi, float& pdf, GfVec2f texcoord, const std::function<float()>& uniform_float);
    GfVec3f Eval(GfVec3f wi, GfVec3f wo, GfVec2f texcoord);
    float Pdf(GfVec3f wi, GfVec3f wo, GfVec2f texcoord);

    InputDescriptor diffuseColor;
    InputDescriptor specularColor;
    InputDescriptor emissiveColor;
    InputDescriptor displacement;
    InputDescriptor opacity;
    InputDescriptor opacityThreshold;
    InputDescriptor roughness;
    InputDescriptor metallic;
    InputDescriptor clearcoat;
    InputDescriptor clearcoatRoughness;
    InputDescriptor occlusion;
    InputDescriptor normal;
    InputDescriptor ior;

   private:
    struct MaterialRecord {
        GfVec3f diffuseColor;
        GfVec3f specularColor;
        GfVec3f emissiveColor;
        GfVec3f displacement;
        float opacity;
        float opacityThreshold;
        float roughness;
        float metallic;
        float clearcoat;
        float clearcoatRoughness;
        float occlusion;
        GfVec3f normal;
        float ior;
    };

    MaterialRecord SampleMaterialRecord(GfVec2f texcoord);
    HdMaterialNetwork2 surfaceNetwork;

    void TryLoadTexture(
        const char* str,
        InputDescriptor& descriptor,
        HdMaterialNode2& usd_preview_surface);
    void TryLoadParameter(
        const char* str,
        InputDescriptor& descriptor,
        HdMaterialNode2& usd_preview_surface);

    HdMaterialNode2 get_input_connection(
        HdMaterialNetwork2 surfaceNetwork,
        std::map<TfToken, std::vector<HdMaterialConnection2>>::value_type& input_connection);
};

USTC_CG_NAMESPACE_CLOSE_SCOPE