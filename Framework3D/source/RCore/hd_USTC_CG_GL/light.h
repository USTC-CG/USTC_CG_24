#pragma once
#include "USTC_CG.h"
#include "pxr/base/gf/vec3f.h"
#include "pxr/imaging/garch/glApi.h"
#include "pxr/imaging/hd/light.h"
#include "pxr/imaging/hd/material.h"
#include "pxr/imaging/hio/image.h"
#include "pxr/pxr.h"
#include "pxr/usd/sdf/assetPath.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
class Shader;
#if USTC_CG_BUILD_MODULE
#endif

using namespace pxr;
class USTC_CG_API Hd_USTC_CG_Light : public HdLight {
   public:
    explicit Hd_USTC_CG_Light(const SdfPath& id, const TfToken& lightType)
        : HdLight(id),
          _lightType(lightType)
    {
    }

    void Sync(HdSceneDelegate* sceneDelegate, HdRenderParam* renderParam, HdDirtyBits* dirtyBits)
        override;
    HdDirtyBits GetInitialDirtyBitsMask() const override;

    VtValue Get(TfToken const& token) const;

    [[nodiscard]] TfToken GetLightType() const
    {
        return _lightType;
    }

   private:
    // Stores the internal light type of this light.
    TfToken _lightType;
    // Cached states.
    TfHashMap<TfToken, VtValue, TfToken::HashFunctor> _params;
};

class USTC_CG_API Hd_USTC_CG_Dome_Light : public Hd_USTC_CG_Light {
   public:
    struct USTC_CG_API InputDescriptor {
        HioImageSharedPtr image = nullptr;

        TfToken wrapS;
        TfToken wrapT;

        TfToken uv_primvar_name;

        VtValue value;

        GLuint glTexture = 0;
        TfToken input_name;
    } ;

    Hd_USTC_CG_Dome_Light(const SdfPath& id, const TfToken& lightType)
        : Hd_USTC_CG_Light(id, lightType)
    {
    }

    GLuint createTextureFromHioImage(const InputDescriptor& descriptor);
    void RefreshGLBuffer();
    void BindTextures(Shader& shader, unsigned& id);

    void _PrepareDomeLight(SdfPath const& id, HdSceneDelegate* scene_delegate);
    void Sync(HdSceneDelegate* sceneDelegate, HdRenderParam* renderParam, HdDirtyBits* dirtyBits)
        override;

    void Finalize(HdRenderParam* renderParam) override;

   private:
    pxr::SdfAssetPath textureFileName;
    GfVec3f radiance;

    InputDescriptor env_texture;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
