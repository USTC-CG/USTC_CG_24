#include "GCore/Components/MeshOperand.h"
#include "GCore/Components/PointsComponent.h"
#include "GCore/GOP.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"
#include "pxr/base/gf/vec3f.h"
#include "pxr/base/vt/array.h"

namespace USTC_CG::node_set_vert_color {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Geometry>("Geometry");
    b.add_input<decl::Float3Buffer>("Color");
    b.add_output<decl::Geometry>("Geometry");
}

static void node_exec(ExeParams params)
{
    // Left empty.
    auto color = params.get_input<pxr::VtArray<pxr::GfVec3f>>("Color");
    auto geometry = params.get_input<GOperandBase>("Geometry");

    auto mesh = geometry.get_component<MeshComponent>();
    auto points = geometry.get_component<PointsComponent>();
    if (mesh) {

		mesh->displayColor = color;
    }
    else if (points)
    {
		points->displayColor = color;
    }
    else
    {
		throw std::runtime_error("The input is not a mesh or points");
	}

	params.set_output("Geometry", std::move(geometry));
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Set Vertex Color");
    strcpy_s(ntype.id_name, "geom_set_vert_color");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_set_vert_color
