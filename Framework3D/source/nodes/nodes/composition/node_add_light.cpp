#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdLux/sphereLight.h>

#include "Nodes/GlobalUsdStage.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "comp_node_base.h"

namespace USTC_CG::node_comp_add_point_light {

using namespace pxr;
static void node_declare(NodeDeclarationBuilder& b)
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

static void node_exec(ExeParams params)
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

    auto xformPath = SdfPath("/Reference").AppendPath(SdfPath("XForm"));
    auto lightPath = xformPath.AppendPath(sdf_path);
    // Create a transform for the light and set its position
    pxr::UsdGeomXform lightXform = pxr::UsdGeomXform::Define(global_stage, xformPath);

    auto transformOp = lightXform.GetTransformOp();
    if (!transformOp) {
        transformOp = lightXform.AddTransformOp();
    }
    transformOp.Set(pxr::GfMatrix4d().SetTranslate(pxr::GfVec3f(x, y, z)));

    pxr::UsdLuxSphereLight sphere_light = pxr::UsdLuxSphereLight::Define(global_stage, lightPath);
    sphere_light.CreateDiffuseAttr().Set(VtValue(GfVec3f(r, g, b)));
    auto radius = params.get_input<float>("radius");
    sphere_light.CreateRadiusAttr().Set(VtValue(radius));
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Add Point Light");
    strcpy_s(ntype.id_name, "comp_add_point_light");

    comp_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    ntype.ALWAYS_REQUIRED = true;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_comp_add_point_light
