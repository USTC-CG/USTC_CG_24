#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "geom_node_base.h"
#include "GCore/geom_node_global_params.h"
#include "nvrhi/utils.h"
#include "pxr/base/tf/ostreamMethods.h"
#include "pxr/base/vt/typeHeaders.h"
namespace USTC_CG::node_geom_add_point {

struct AddedPoints {
    pxr::VtArray<pxr::GfVec3f> points;

    nlohmann::json serialize()
    {
        auto val = pxr::TfStringify(points);
        nlohmann::json ret;
        ret["points"] = val;
        return ret;
    }

    void deserialize(const nlohmann::json& json)
    {
    }
};

static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_storage<AddedPoints>();

    b.add_output<decl::Geometry>("Points");
}

static void node_exec(ExeParams params)
{
    auto& storage = params.get_storage<AddedPoints&>();

    params.set_storage(storage);
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
