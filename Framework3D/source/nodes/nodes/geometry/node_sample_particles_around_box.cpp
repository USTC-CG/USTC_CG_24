#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/primvarsAPI.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>

#include "GCore/Components/MaterialComponent.h"
#include "GCore/Components/MeshOperand.h"
#include "GCore/Components/PointsComponent.h"
#include "GCore/Components/XformComponent.h"
#include "Nodes/GlobalUsdStage.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"
#include "pxr/base/gf/rotation.h"
#include "sph_fluid/utils.h"
#include "sph_fluid/particle_system.h"

namespace USTC_CG::node_sample_particle_around_box {

using namespace USTC_CG::node_sph_fluid;

static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Float3>("box min");
    b.add_input<decl::Float3>("box max");
    b.add_input<decl::Float>("scale factor").default_val(0.2f).min(0.0f).max(1.0f);
    b.add_input<decl::Float3>("num particle per axis");

    b.add_output<decl::Geometry>("Particles");
}

static void node_exec(ExeParams params)
{

    auto box_min = params.get_input<pxr::GfVec3f>("box min");
    auto box_max = params.get_input<pxr::GfVec3f>("box max");
    auto scale_factor = params.get_input<float>("scale factor");

    auto n_per_axis = params.get_input<pxr::GfVec3f>("num particle per axis");

    if (box_max[0] < box_min[0] || box_max[1] < box_min[1] || box_max[2] < box_min[2]) 
    {
		throw std::runtime_error("Invalid particle samping box.");
	}
    if (n_per_axis[0] <= 0 || n_per_axis[1] <= 0 ||
        n_per_axis[2] <= 0) {
        throw std::runtime_error("Invalid number of particles per axis.");
    }

    auto particle_pos = ParticleSystem::sample_particle_pos_around_a_box(
		Eigen::Vector3d(box_min[0], box_min[1], box_min[2]),
		Eigen::Vector3d(box_max[0], box_max[1], box_max[2]),
		Eigen::Vector3i(n_per_axis[0], n_per_axis[1], n_per_axis[2]),
        false,
        scale_factor
        );

    auto geometry = GOperandBase();
    auto points_component = std::make_shared<PointsComponent>(&geometry);
    geometry.attach_component(points_component);

    points_component->vertices = eigen_to_usd_vertices(particle_pos); 

	float point_width = 0.05;
	points_component->width = pxr::VtArray<float>(particle_pos.rows(), point_width);


    params.set_output("Particles", std::move(geometry));
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Sample Particles around a box");
    strcpy_s(ntype.id_name, "geom_sample_particle_around_box");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_sample_particles_around_a_box
