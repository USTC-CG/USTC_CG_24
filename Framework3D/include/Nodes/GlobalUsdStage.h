#pragma once
#include <pxr/usd/usd/stage.h>

#include "USTC_CG.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

enum class GlobalUsdActions { Create, Delete };

// Enum to represent the type of objects
enum class ObjectType { None, Mesh, Cylinder, Sphere };

// This is not best practice, but I am really in a hurry to get them all
// running. Later this will be improved with usd path resolver functionality.
// This stage serves for sharing data from the nodes to the renderer
struct USTC_CG_API GlobalUsdStage {
    static pxr::UsdStageRefPtr global_usd_stage;

    static constexpr int timeCodesPerSecond = 30;

    // Function to handle the creation of different objects
    static void CreateObject(pxr::SdfPath path, ObjectType type);
    static pxr::SdfPath EditObject(pxr::SdfPath path);

    static void DeleteObject(pxr::SdfPath path);
};

USTC_CG_NAMESPACE_CLOSE_SCOPE