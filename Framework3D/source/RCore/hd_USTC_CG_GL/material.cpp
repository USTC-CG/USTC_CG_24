#include "material.h"

#include "Utils/Logging/Logging.h"
#include "pxr/imaging/hd/sceneDelegate.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;

void Hd_USTC_CG_Material::Sync(
    HdSceneDelegate* sceneDelegate,
    HdRenderParam* renderParam,
    HdDirtyBits* dirtyBits)
{
    VtValue vtMat = sceneDelegate->GetMaterialResource(GetId());
    if (vtMat.IsHolding<HdMaterialNetworkMap>()) {
        HdMaterialNetworkMap const& hdNetworkMap = vtMat.UncheckedGet<HdMaterialNetworkMap>();
        if (!hdNetworkMap.terminals.empty() && !hdNetworkMap.map.empty()) {
            logging("Loaded a material", Info);
        }
    }
    else {
        logging("Not loaded a material", Info);
    }
    *dirtyBits = Clean;
}

HdDirtyBits Hd_USTC_CG_Material::GetInitialDirtyBitsMask() const
{
    return AllDirty;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
