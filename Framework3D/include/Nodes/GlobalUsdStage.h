#pragma once
#include <pxr/usd/usd/stage.h>

#include "USTC_CG.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

// This is not best practice, but I am really in a hurry to get them all
// running. Later this will be improved with usd path resolver functionality.
// This stage serves for sharing data from the nodes to the renderer
struct GlobalUsdStage {
    static pxr::UsdStageRefPtr global_usd_stage;

    static constexpr int timeCodesPerSecond = 30;

};

USTC_CG_NAMESPACE_CLOSE_SCOPE