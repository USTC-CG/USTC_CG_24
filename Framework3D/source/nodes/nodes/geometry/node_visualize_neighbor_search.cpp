#include "GCore/Components/MeshOperand.h"
#include "GCore/Components/PointsComponent.h"
#include "GCore/GOP.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"
#include "pxr/base/gf/vec3f.h"
#include "pxr/base/vt/array.h"
#include "sph_fluid/particle_system.h"
#include "sph_fluid/utils.h"

namespace USTC_CG::node_visualize_neighbor_search {

using namespace USTC_CG::node_sph_fluid;

static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Geometry>("fluid particles");
    b.add_input<decl::Geometry>("boundary particles");
    b.add_input<decl::Int>("idx").default_val(0).min(0).max(10000);

    b.add_output<decl::Geometry>("all particles");
    b.add_output<decl::Float3Buffer>("colors");
}

static void node_exec(ExeParams params)
{
    auto idx = params.get_input<int>("idx");

    auto fluid_particle_geo = params.get_input<GOperandBase>("fluid particles");
    auto boundary_particle_geo = params.get_input<GOperandBase>("boundary particles");

    auto fluid_particle_pos = usd_vertices_to_eigen(fluid_particle_geo.get_component<PointsComponent>()->vertices);
    auto boundary_particle_pos = usd_vertices_to_eigen(boundary_particle_geo.get_component<PointsComponent>()->vertices);

    auto ps = ParticleSystem(fluid_particle_pos, boundary_particle_pos);

    int n_particles = fluid_particle_pos.rows() + boundary_particle_pos.rows(); 

    //ps.assign_particles_to_cells(); // already done when init particle system
    //ps.search_neighbors();

    MatrixXd new_color = Eigen::MatrixXd::Zero(n_particles, 3);

    if (idx >= n_particles)
        throw std::runtime_error("Invalid idx");

    auto p = ps.particles()[idx]; 

    std::cout << "[visualize neighbor search] p->idx = " << p->idx() << " type: " << p->type()
              << " X= (" << p->x().transpose(); 
    std::cout << "), neighbor size:" << p->neighbors().size() << std::endl;
	  for(auto & q: p->neighbors())
	  {
		new_color.row(q->idx()) << 1, 0, 0;
        std::cout << "q: idx= " << q->idx() << " x= " << q->x().transpose() << std::endl;
	  }
	  new_color.row(idx) << 0, 1, 0;

    //-------------------------

    auto color = eigen_to_usd_vertices(new_color);

	auto geometry = GOperandBase();
	auto points_component = std::make_shared<PointsComponent>(&geometry);
	geometry.attach_component(points_component);

    MatrixXd vertices = MatrixXd::Zero(n_particles, 3); 
    vertices.block(0, 0, fluid_particle_pos.rows(), 3) = fluid_particle_pos;
    vertices.block(fluid_particle_pos.rows(), 0, boundary_particle_pos.rows(), 3) =
        boundary_particle_pos;

	points_component->vertices = eigen_to_usd_vertices(vertices);
	float point_width = 0.05;
	points_component->width = pxr::VtArray<float>(vertices.rows(), point_width);

    params.set_output("all particles", std::move(geometry));
	params.set_output("colors", std::move(color));


}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "visualize neighbor search");
    strcpy_s(ntype.id_name, "geom_visualize_neighbor_search");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_set_vert_color
