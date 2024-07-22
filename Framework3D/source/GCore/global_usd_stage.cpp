#include <pxr/base/vt/value.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/sdf/types.h>
#include <pxr/usd/usd/attribute.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/cylinder.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/sphere.h>

#include "Nodes/GlobalUsdStage.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

pxr::UsdStageRefPtr GlobalUsdStage::global_usd_stage =
    pxr::UsdStage::CreateInMemory();

void GlobalUsdStage::CreateObject(pxr::SdfPath path, ObjectType type)
{
    int counter = 0;
    std::string objectName;
    pxr::SdfPath objectPath;

    // Find a unique name for the object
    do {
        objectName = "object_" + std::to_string(counter++);
        objectPath = path.AppendChild(pxr::TfToken(objectName));
    } while (global_usd_stage->GetPrimAtPath(objectPath));

    switch (type) {
        case ObjectType::Mesh: {
            pxr::UsdGeomMesh::Define(global_usd_stage, objectPath);
            // Add your mesh customization code here
            break;
        }
        case ObjectType::Cylinder: {
            pxr::UsdGeomCylinder::Define(global_usd_stage, objectPath);
            // Add your cylinder customization code here
            break;
        }
        case ObjectType::Sphere: {
            pxr::UsdGeomSphere::Define(global_usd_stage, objectPath);
            // Add your sphere customization code here
            break;
        }
        default: break;
    }
}

pxr::SdfPath GlobalUsdStage::EditObject(pxr::SdfPath path)
{
    auto prim = global_usd_stage->GetPrimAtPath(path);
    assert(prim);
    if (prim) {
        // Define a string attribute name
        pxr::TfToken attrName("geometry_node");

        // Create or get the attribute
        auto attr = prim.GetAttribute(attrName);
        if (!attr) {
            attr =
                prim.CreateAttribute(attrName, pxr::SdfValueTypeNames->String);

            // Set the value of the attribute
            std::string value = "";
            attr.Set(pxr::VtValue(value));
        }

        return attr.GetPath();
    }

    return pxr::SdfPath::EmptyPath();
}

void GlobalUsdStage::DeleteObject(pxr::SdfPath path)
{
    if (path != pxr::SdfPath::AbsoluteRootPath()) {
        if (global_usd_stage->GetPrimAtPath(path)) {
            global_usd_stage->RemovePrim(path);
        }
    }
}

USTC_CG_NAMESPACE_CLOSE_SCOPE