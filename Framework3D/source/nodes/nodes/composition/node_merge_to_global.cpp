#include "Nodes/GlobalUsdStage.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "comp_node_base.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/references.h"
#include "pxr/usd/usdGeom/tokens.h"

namespace USTC_CG::node_merge_to_global {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Layer>("Layer");
    b.add_input<decl::String>("Path").default_val("Scene");
}

static void node_exec(ExeParams params)
{
    using namespace pxr;
    auto layer = params.get_input<pxr::UsdStageRefPtr>("Layer");
    layer->SetMetadata(pxr::UsdGeomTokens->upAxis, pxr::VtValue(pxr::UsdGeomTokens->z));
    ;

    auto path = params.get_input<std::string>("Path");
    auto sdf_path = SdfPath(path.c_str());

    auto global_stage = GlobalUsdStage::global_usd_stage;

    UsdPrim assemblyRoot = global_stage->DefinePrim(SdfPath("/Reference").AppendPath(sdf_path));

    assemblyRoot.GetReferences().SetReferences({ layer->GetRootLayer()->GetIdentifier() });
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Merge To Global");
    strcpy_s(ntype.id_name, "comp_merge_to_global");

    comp_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    ntype.ALWAYS_REQUIRED = true;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_merge_to_global
