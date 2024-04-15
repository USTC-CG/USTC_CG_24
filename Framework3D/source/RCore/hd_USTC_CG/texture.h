#pragma once
#include "USTC_CG.h"
#include "pxr/base/gf/vec2f.h"
#include "pxr/base/gf/vec4f.h"
#include "pxr/usd/sdf/assetPath.h"
#include "surfaceInteraction.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
class Texture2D {
   public:
    Texture2D();
    Texture2D(SdfAssetPath path);

    // Texture Interface
    GfVec4f Evaluate(const GfVec2f& uv) const;
    ~Texture2D();

    unsigned component_conut() const
    {
        return _component_count;
    }

    bool isValid() const
    {
        return texture != nullptr;
    }

   private:
    unsigned _component_count;

    SdfAssetPath textureFileName;
    HioImageSharedPtr texture;
    HioImage::StorageSpec storageSpec;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
