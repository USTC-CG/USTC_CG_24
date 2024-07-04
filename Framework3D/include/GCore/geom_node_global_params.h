#pragma once

#include "USTC_CG.h"
#include "pxr/usd/sdf/path.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
struct PickEvent;
struct GeomNodeGlobalParams {
    pxr::SdfPath prim_path;
    PickEvent* pick;
};
USTC_CG_NAMESPACE_CLOSE_SCOPE