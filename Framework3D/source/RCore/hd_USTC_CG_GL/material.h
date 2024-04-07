#pragma once
#include "USTC_CG.h"
#include "Utils/Logging/Logging.h"
#include "Utils/Macro/map.h"
#include "pxr/imaging/garch/glApi.h"
#include "pxr/imaging/hd/material.h"
#include "pxr/imaging/hio/image.h"

namespace pxr {
class Hio_OpenEXRImage;

}

USTC_CG_NAMESPACE_OPEN_SCOPE
class Shader;
using namespace pxr;

class Hio_StbImage;
class Hd_USTC_CG_Material : public HdMaterial {
   public:
    struct InputDescriptor {
        HioImageSharedPtr image = nullptr;

        TfToken wrapS;
        TfToken wrapT;

        TfToken uv_primvar_name;

        VtValue value;

        GLuint glTexture = 0;
    };

    explicit Hd_USTC_CG_Material(SdfPath const& id);

    void Sync(HdSceneDelegate* sceneDelegate, HdRenderParam* renderParam, HdDirtyBits* dirtyBits)
        override;

    void RefreshGLBuffer();
    void BindTextures(Shader& shader);
    HdDirtyBits GetInitialDirtyBitsMask() const override;

    TfToken requireTexcoordName();

    void Finalize(HdRenderParam* renderParam) override;

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
    HdMaterialNetwork2 surfaceNetwork;

    // Function to create an OpenGL texture from a HioImage object
    static GLuint createTextureFromHioImage(const InputDescriptor& descriptor);

    void TryLoadTexture(
        const char* str,
        InputDescriptor& descriptor,
        HdMaterialNode2& usd_preview_surface);
    void TryLoadParameter(
        const char* str,
        InputDescriptor& descriptor,
        HdMaterialNode2& usd_preview_surface);

    void TryCreateGLTexture(InputDescriptor& descriptor);

    HdMaterialNode2 get_input_connection(
        HdMaterialNetwork2 surfaceNetwork,
        std::map<TfToken, std::vector<HdMaterialConnection2>>::value_type& input_connection);
    void DestroyTexture(InputDescriptor& input_descriptor);
};

USTC_CG_NAMESPACE_CLOSE_SCOPE