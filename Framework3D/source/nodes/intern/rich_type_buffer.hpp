#pragma once
#include "USTC_CG.h"
USTC_CG_NAMESPACE_OPEN_SCOPE

class Hd_USTC_CG_Light;
class Hd_USTC_CG_Camera;

using LightArray = pxr::VtArray<Hd_USTC_CG_Light *>;
using CameraArray = pxr::VtArray<Hd_USTC_CG_Camera *>;

struct TextureHandle;

USTC_CG_NAMESPACE_CLOSE_SCOPE