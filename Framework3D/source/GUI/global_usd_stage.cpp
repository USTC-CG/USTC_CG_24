#include "Nodes/GlobalUsdStage.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
pxr::UsdStageRefPtr GlobalUsdStage::global_usd_stage =
    pxr::UsdStage::CreateInMemory();

USTC_CG_NAMESPACE_CLOSE_SCOPE