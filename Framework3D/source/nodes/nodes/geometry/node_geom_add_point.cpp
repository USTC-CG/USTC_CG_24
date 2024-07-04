#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "geom_node_base.h"
#include "nvrhi/utils.h"

namespace USTC_CG::node_geom_add_point {

struct AddedPoints {
    pxr::VtArray<pxr::GfVec3f> points;
};

static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_storage<AddedPoints>();

    b.add_output<decl::Geometry>("Points");
}

static void node_exec(ExeParams params)
{
    auto& storage = params.get_storage<AddedPoints&>();

}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "geom_add_point");
    strcpy(ntype.id_name, "node_geom_add_point");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_geom_add_point
