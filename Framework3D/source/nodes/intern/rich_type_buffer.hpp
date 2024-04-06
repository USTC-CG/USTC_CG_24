#pragma once
#include "USTC_CG.h"
#include "pxr/base/tf/hash.h"
#include "pxr/base/tf/hashmap.h"
USTC_CG_NAMESPACE_OPEN_SCOPE

class Hd_USTC_CG_Light;
class Hd_USTC_CG_Camera;
class Hd_USTC_CG_Mesh;
class Hd_USTC_CG_Material;

using LightArray = pxr::VtArray<Hd_USTC_CG_Light *>;
using CameraArray = pxr::VtArray<Hd_USTC_CG_Camera *>;
using MeshArray = pxr::VtArray<Hd_USTC_CG_Mesh *>;
using MaterialMap = pxr::TfHashMap<pxr::SdfPath, Hd_USTC_CG_Material *, pxr::TfHash>;

USTC_CG_NAMESPACE_CLOSE_SCOPE