
#include <pxr/base/gf/rotation.h>
#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdLux/domeLight.h>
#include <pxr/usd/usdLux/rectLight.h>
#include <pxr/usd/usdLux/sphereLight.h>

#include "Nodes/GlobalUsdStage.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "comp_node_base.h"
#include "pxr/base/gf/matrix4f.h"

namespace USTC_CG::node_comp_add_point_light {

using namespace pxr;
static void node_declare_add_sphere_light(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Float>("X").default_val(0).min(-10).max(10);
    b.add_input<decl::Float>("Y").default_val(0).min(-10).max(10);
    b.add_input<decl::Float>("Z").default_val(0).min(-10).max(10);
    b.add_input<decl::Float>("R").default_val(1).min(0).max(10);
    b.add_input<decl::Float>("G").default_val(1).min(0).max(10);
    b.add_input<decl::Float>("B").default_val(1).min(0).max(10);
    b.add_input<decl::Float>("radius").default_val(0.1).min(0).max(1);

    b.add_input<decl::String>("Path").default_val("Light");
}

static void node_exec_add_sphere_light(ExeParams params)
{
    auto x = params.get_input<float>("X");
    auto y = params.get_input<float>("Y");
    auto z = params.get_input<float>("Z");

    auto r = params.get_input<float>("R");
    auto g = params.get_input<float>("G");
    auto b = params.get_input<float>("B");

    auto path = params.get_input<std::string>("Path");
    auto sdf_path = SdfPath(path.c_str());

    auto global_stage = GlobalUsdStage::global_usd_stage;

    auto xformPath = SdfPath("/Reference").AppendPath(SdfPath("XForm" + sdf_path.GetString()));
    auto lightPath = xformPath.AppendPath(sdf_path);
    // Create a transform for the light and set its position
    pxr::UsdGeomXform lightXform = pxr::UsdGeomXform::Define(global_stage, xformPath);

    auto transformOp = lightXform.GetTransformOp();
    if (!transformOp) {
        transformOp = lightXform.AddTransformOp();
    }
    transformOp.Set(pxr::GfMatrix4d().SetTranslate(pxr::GfVec3f(x, y, z)));

    pxr::UsdLuxSphereLight sphere_light = pxr::UsdLuxSphereLight::Define(global_stage, lightPath);
    sphere_light.CreateColorAttr().Set(VtValue(GfVec3f(r, g, b)));
    sphere_light.CreateDiffuseAttr().Set(1.0f);
    auto radius = params.get_input<float>("radius");
    sphere_light.CreateRadiusAttr().Set(VtValue(radius));
}

using namespace pxr;
static void node_declare_add_dome_light(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Float>("R").default_val(1).min(0).max(10);
    b.add_input<decl::Float>("G").default_val(1).min(0).max(10);
    b.add_input<decl::Float>("B").default_val(1).min(0).max(10);

    b.add_input<decl::Float>("Rotate X").default_val(0).min(-180).max(180);
    b.add_input<decl::Float>("Rotate Y").default_val(0).min(-180).max(180);
    b.add_input<decl::Float>("Rotate Z").default_val(0).min(-180).max(180);

    b.add_input<decl::String>("File").default_val("Default");

    b.add_input<decl::String>("Path").default_val("DomeLight");
}

static void node_exec_add_dome_light(ExeParams params)
{
    auto rotate_x = params.get_input<float>("Rotate X");
    auto rotate_y = params.get_input<float>("Rotate Y");
    auto rotate_z = params.get_input<float>("Rotate Z");

    auto r = params.get_input<float>("R");
    auto g = params.get_input<float>("G");
    auto b = params.get_input<float>("B");

    auto texture_file_name = params.get_input<std::string>("File");
    auto asset_path = pxr::SdfAssetPath(texture_file_name.c_str());

    auto path = params.get_input<std::string>("Path");
    auto sdf_path = SdfPath(path.c_str());

    auto global_stage = GlobalUsdStage::global_usd_stage;

    auto xformPath = SdfPath("/Reference").AppendPath(SdfPath("XForm" + sdf_path.GetString()));
    auto lightPath = xformPath.AppendPath(sdf_path);
    // Create a transform for the light and set its position
    pxr::UsdGeomXform lightXform = pxr::UsdGeomXform::Define(global_stage, xformPath);

    pxr::GfMatrix4d r_x;
    r_x.SetRotate(pxr::GfRotation{ { 1, 0, 0 }, rotate_x });
    pxr::GfMatrix4d r_y;
    r_y.SetRotate(pxr::GfRotation{ { 0, 1, 0 }, rotate_y });
    pxr::GfMatrix4d r_z;
    r_z.SetRotate(pxr::GfRotation{ { 0, 0, 1 }, rotate_z });

    auto transform = r_x * r_y * r_z;

    auto transformOp = lightXform.GetTransformOp();
    if (!transformOp) {
        transformOp = lightXform.AddTransformOp();
    }
    transformOp.Set(transform);

    pxr::UsdLuxDomeLight dome_light = pxr::UsdLuxDomeLight::Define(global_stage, lightPath);
    dome_light.CreateColorAttr().Set(VtValue(GfVec3f(r, g, b)));
    dome_light.CreateDiffuseAttr().Set(1.0f);
    dome_light.CreateTextureFileAttr().Set(VtValue(asset_path));
}

using namespace pxr;
static void node_declare_add_rect_light(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Float>("R").default_val(1).min(0).max(10);
    b.add_input<decl::Float>("G").default_val(1).min(0).max(10);
    b.add_input<decl::Float>("B").default_val(1).min(0).max(10);

    b.add_input<decl::Float>("X").default_val(0).min(-10).max(10);
    b.add_input<decl::Float>("Y").default_val(0).min(-10).max(10);
    b.add_input<decl::Float>("Z").default_val(0).min(-10).max(10);

    b.add_input<decl::Float>("Rotate X").default_val(0).min(-180).max(180);
    b.add_input<decl::Float>("Rotate Y").default_val(0).min(-180).max(180);
    b.add_input<decl::Float>("Rotate Z").default_val(0).min(-180).max(180);

    b.add_input<decl::Float>("Width").default_val(0.5).min(0.1).max(5);
    b.add_input<decl::Float>("Height").default_val(0.5).min(0.1).max(5);

    b.add_input<decl::String>("Path").default_val("DomeLight");
}

static void node_exec_add_rect_light(ExeParams params)
{
    auto x = params.get_input<float>("X");
    auto y = params.get_input<float>("Y");
    auto z = params.get_input<float>("Z");

    auto rotate_x = params.get_input<float>("Rotate X");
    auto rotate_y = params.get_input<float>("Rotate Y");
    auto rotate_z = params.get_input<float>("Rotate Z");

    auto r = params.get_input<float>("R");
    auto g = params.get_input<float>("G");
    auto b = params.get_input<float>("B");

    auto width = params.get_input<float>("Width");
    auto height = params.get_input<float>("Height");

    auto path = params.get_input<std::string>("Path");
    auto sdf_path = SdfPath(path.c_str());

    auto global_stage = GlobalUsdStage::global_usd_stage;

    auto xformPath = SdfPath("/Reference").AppendPath(SdfPath("XForm" + sdf_path.GetString()));
    auto lightPath = xformPath.AppendPath(sdf_path);
    // Create a transform for the light and set its position
    pxr::UsdGeomXform lightXform = pxr::UsdGeomXform::Define(global_stage, xformPath);

    pxr::GfMatrix4d t;
    t.SetTranslate((pxr::GfVec3d{ x, y, z }));
    pxr::GfMatrix4d r_x;
    r_x.SetRotate(pxr::GfRotation{ { 1, 0, 0 }, rotate_x });
    pxr::GfMatrix4d r_y;
    r_y.SetRotate(pxr::GfRotation{ { 0, 1, 0 }, rotate_y });
    pxr::GfMatrix4d r_z;
    r_z.SetRotate(pxr::GfRotation{ { 0, 0, 1 }, rotate_z });

    auto transform = r_x * r_y * r_z * t;

    auto transformOp = lightXform.GetTransformOp();
    if (!transformOp) {
        transformOp = lightXform.AddTransformOp();
    }
    transformOp.Set(transform);

    pxr::UsdLuxRectLight rect_light = pxr::UsdLuxRectLight::Define(global_stage, lightPath);
    rect_light.CreateColorAttr().Set(VtValue(GfVec3f(r, g, b)));
    rect_light.CreateDiffuseAttr().Set(1.0f);
    rect_light.CreateWidthAttr().Set( VtValue(width));
    rect_light.CreateHeightAttr().Set(VtValue(height));
}

static void node_register()
{
    static NodeTypeInfo ntype_add_sphere_light;

    strcpy(ntype_add_sphere_light.ui_name, "Add Point Light");
    strcpy_s(ntype_add_sphere_light.id_name, "comp_add_point_light");

    comp_node_type_base(&ntype_add_sphere_light);
    ntype_add_sphere_light.node_execute = node_exec_add_sphere_light;
    ntype_add_sphere_light.declare = node_declare_add_sphere_light;
    ntype_add_sphere_light.ALWAYS_REQUIRED = true;
    nodeRegisterType(&ntype_add_sphere_light);

    static NodeTypeInfo ntype_add_dome_light;

    strcpy(ntype_add_dome_light.ui_name, "Add Dome Light");
    strcpy_s(ntype_add_dome_light.id_name, "comp_add_dome_light");

    comp_node_type_base(&ntype_add_dome_light);
    ntype_add_dome_light.node_execute = node_exec_add_dome_light;
    ntype_add_dome_light.declare = node_declare_add_dome_light;
    ntype_add_dome_light.ALWAYS_REQUIRED = true;
    nodeRegisterType(&ntype_add_dome_light);

    static NodeTypeInfo ntype_add_rect_light;

    strcpy(ntype_add_rect_light.ui_name, "Add Rect Light");
    strcpy_s(ntype_add_rect_light.id_name, "comp_add_rect_light");

    comp_node_type_base(&ntype_add_rect_light);
    ntype_add_rect_light.node_execute = node_exec_add_rect_light;
    ntype_add_rect_light.declare = node_declare_add_rect_light;
    ntype_add_rect_light.ALWAYS_REQUIRED = true;
    nodeRegisterType(&ntype_add_rect_light);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_comp_add_point_light
