#include "GCore/Components/PointsComponent.h"
#include "GCore/geom_node_global_params.h"
#include "GUI/ui_event.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "geom_node_base.h"
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

    b.add_input<decl::Float>("Width").min(0.001).max(1).default_val(0.1f);

    b.add_output<decl::Geometry>("Points");
}

static void node_exec(ExeParams params)
{
    auto& storage = params.get_storage<AddedPoints&>();

    auto pick = params.get_global_params<GeomNodeGlobalParams>().pick;
    if (pick) {
        storage.points.push_back(pxr::GfVec3f(pick->point));
    }

    params.set_storage(storage);

    auto width = params.get_input<float>("Width");

    auto geometry = Geometry();
    auto points_component = std::make_shared<PointsComponent>(&geometry);
    geometry.attach_component(points_component);

    pxr::VtArray widths(storage.points.size(), width);

    points_component->set_vertices(storage.points);
    points_component->set_width(widths);

    params.set_output("Points", geometry);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Add Point");
    strcpy(ntype.id_name, "node_geom_add_point");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_geom_add_point
