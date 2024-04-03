#pragma once
#include "USTC_CG.h"
#include "pxr/imaging/hd/light.h"
#include "pxr/pxr.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
class USTC_CG_API Hd_USTC_CG_Light : public HdLight {
   public:
    explicit Hd_USTC_CG_Light(const SdfPath& id, const TfToken& lightType) : HdLight(id)
    {
    }

    void Sync(HdSceneDelegate* sceneDelegate, HdRenderParam* renderParam, HdDirtyBits* dirtyBits)
        override;
    HdDirtyBits GetInitialDirtyBitsMask() const override;

    VtValue Get(TfToken const& token) const;

   private:
    // Stores the internal light type of this light.
    TfToken _lightType;
    // Cached states.
    TfHashMap<TfToken, VtValue, TfToken::HashFunctor> _params;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
