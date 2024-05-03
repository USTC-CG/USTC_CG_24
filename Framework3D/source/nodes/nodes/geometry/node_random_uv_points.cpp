#include <random>

#include "GCore/Components/PointsComponent.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"

namespace USTC_CG::node_random_uv_points {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Int>("Count").min(1).max(100).default_val(10);
    b.add_input<decl::Int>("Seed").min(-100).max(100).default_val(0);
    b.add_input<decl::Float>("Width").min(0.001).max(1).default_val(0.1f);
    b.add_output<decl::Geometry>("Points");
}

static void node_exec(ExeParams params)
{
    using namespace pxr;
    auto count = params.get_input<int>("Count");
    auto seed = params.get_input<int>("Seed");
    auto width = params.get_input<float>("Width");

    std::default_random_engine engine(seed);

    auto geometry = GOperandBase();
    auto points_component = std::make_shared<PointsComponent>(&geometry);
    geometry.attach_component(points_component);

    pxr::VtArray<pxr::GfVec3f>& points = points_component->vertices;
    pxr::VtArray<float>& widths = points_component->width;

    std::uniform_real_distribution<float> distX(0.f, 1.0f - std::numeric_limits<float>::epsilon());
    std::uniform_real_distribution<float> distY(0.f, 1.0f - std::numeric_limits<float>::epsilon());

    for (int i = 0; i < count; ++i) {
        float x = distX(engine);
        float y = distY(engine);
        points.push_back(GfVec3f(x, y, 0.0f));
        widths.push_back(width);
    }

    params.set_output("Points", geometry);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Random UV Points");
    strcpy_s(ntype.id_name, "geom_random_uv_points");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_random_uv_points
