#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/primvarsAPI.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>

#include "GCore/Components/MaterialComponent.h"
#include "GCore/Components/MeshOperand.h"
#include "GCore/Components/SkelComponent.h"
#include "GCore/Components/XformComponent.h"
#include "Nodes/GlobalUsdStage.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"
#include "pxr/base/gf/rotation.h"

#include "character_animation/animator.h"

namespace USTC_CG::node_character_animation {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::AnimatorSocket>("Animator"); // Animator class
    b.add_input<decl::Geometry>("Geometry"); // contain mesh and skeleton

    b.add_output<decl::AnimatorSocket>("Animator");  // Animator class
    b.add_output<decl::Geometry>("Output Geometry"); // contain mesh and skeleton
}

static void node_exec(ExeParams params)
{
    auto geom = params.get_input<GOperandBase>("Geometry"); 

    auto mesh = geom.get_component<MeshComponent>();
    auto skel = geom.get_component<SkelComponent>();

    if (!mesh) {
        throw std::runtime_error("Read mesh error.");
    }
    else if (!skel)   
        throw std::runtime_error("Read skeleton error.");

	auto animator = std::make_shared<Animator>(mesh, skel);
	animator->step(skel);

    params.set_output("Animator", animator);
    params.set_output("Output Geometry", geom);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Animate Mesh");
    strcpy_s(ntype.id_name, "geom_animate_mesh");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_animate_mesh
