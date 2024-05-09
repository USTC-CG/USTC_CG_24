#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <chrono>
#include <iostream>
#include <memory>

#include "GCore/GOP.h"
#include "GCore/Components/MeshOperand.h"
#include "GCore/Components/PointsComponent.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"
#include "sph_fluid/wcsph.h"
#include "sph_fluid/iisph.h"
#include "sph_fluid/sph_base.h"
#include "utils/util_openmesh_bind.h"
#include "sph_fluid/utils.h"

namespace USTC_CG::node_sph_fluid {

using namespace Eigen;

static void node_sph_fluid_declare(NodeDeclarationBuilder& b)
{

    b.add_input<decl::SPHFluidSocket>("SPH Class");

    b.add_input<decl::Geometry>("fluid Particles");
    b.add_input<decl::Geometry>("boundary Particles");

    // Simulation parameters 
    b.add_input<decl::Float3>("sim box min");
    b.add_input<decl::Float3>("sim box max");

    // general parameters
    b.add_input<decl::Float>("dt").default_val(0.01).min(0.0).max(0.5);
    b.add_input<decl::Float>("viscosity").default_val(0.03).min(0.0).max(0.5);
    b.add_input<decl::Float>("gravity").default_val(-9.8);

    // WCSPH parameters 
    b.add_input<decl::Float>("stiffness").default_val(500).min(100).max(10000);
    b.add_input<decl::Float>("exponent").default_val(7).min(1).max(10);

    // --------- (HW Optional) if you implement IISPH, please uncomment the following lines ------------

    b.add_input<decl::Float>("omega").default_val(0.4).min(0.).max(1.);
    b.add_input<decl::Int>("max iter").default_val(20).min(0).max(1000);

    // -----------------------------------------------------------------------------------------------------------

    // Useful switches (0 or 1). You can add more if you like.
    b.add_input<decl::Int>("enable time profiling").default_val(0).min(0).max(1);
    b.add_input<decl::Int>("enable debug output").default_val(0).min(0).max(1);
    b.add_input<decl::Int>("enable step pause").default_val(0).min(0).max(1);

    // Optional switches
    b.add_input<decl::Int>("enable IISPH").default_val(0).min(0).max(1);

    b.add_input<decl::Int>("enable sim 2D").default_val(0).min(0).max(1);

    // Current time in node system 
    b.add_input<decl::Float>("time_code");

    // Output 
    b.add_output<decl::SPHFluidSocket>("SPH Class");
    b.add_output<decl::Geometry>("points");
    b.add_output<decl::Geometry>("boundary points");
    b.add_output<decl::Float3Buffer>("point colors");
}

static void node_sph_fluid_exec(ExeParams params)
{
    auto time_code = params.get_input<float>("time_code");

    auto sph_base = params.get_input<std::shared_ptr<SPHBase>>("SPH Class");

    // ------------------------------ Load particles -----------------------------------------------
    auto fluid_particle_geo = params.get_input<GOperandBase>("fluid Particles");
    auto boundary_particle_geo = params.get_input<GOperandBase>("boundary Particles");

    auto fluid_particle_pos = usd_vertices_to_eigen(fluid_particle_geo.get_component<PointsComponent>()->vertices);
    auto boundary_particle_pos = usd_vertices_to_eigen(boundary_particle_geo.get_component<PointsComponent>()->vertices);
    // --------------------------------------------------------------------------------------------------------------

    // ------------------------------ Load simulation box -----------------------------------------------
    auto sim_box_min = params.get_input<pxr::GfVec3f>("sim box min");
    auto sim_box_max = params.get_input<pxr::GfVec3f>("sim box max");
    if (sim_box_max[0] < sim_box_min[0] || sim_box_max[1] < sim_box_min[1] ||
        sim_box_max[2] < sim_box_min[2]) {
        throw std::runtime_error("Invalid simulation box.");
    }
    Vector3d sim_box_min_eigen(sim_box_min[0], sim_box_min[1], sim_box_min[2]);
    Vector3d sim_box_max_eigen(sim_box_max[0], sim_box_max[1], sim_box_max[2]);
    // --------------------------------------------------------------------------------------------------


    if (time_code == 0) {  // If time = 0, reset and initialize the sph base class
            if (sph_base != nullptr)
				sph_base.reset();

            bool enable_IISPH =  params.get_input<int>("enable IISPH") == 1 ? true : false;
            bool enable_sim_2d = params.get_input<int>("enable sim 2D") == 1 ? true : false;
            
            if (enable_IISPH) {

		    	sph_base = std::make_shared<IISPH>(fluid_particle_pos, boundary_particle_pos, sim_box_min_eigen, sim_box_max_eigen, enable_sim_2d);
			}
			else {
				sph_base = std::make_shared<WCSPH>(fluid_particle_pos,  boundary_particle_pos,sim_box_min_eigen, sim_box_max_eigen,  enable_sim_2d);
            }


            sph_base->dt() = params.get_input<float>("dt");
            sph_base->viscosity() = params.get_input<float>("viscosity");
            sph_base->gravity()  = { 0, 0, params.get_input<float>("gravity") };
            sph_base->enable_sim_2d = enable_sim_2d; 

            // Useful switches
            sph_base->enable_time_profiling = params.get_input<int>("enable time profiling") == 1 ? true : false;
            sph_base->enable_debug_output = params.get_input<int>("enable debug output") == 1 ? true : false;
            sph_base->enable_step_pause = params.get_input<int>("enable step pause") == 1 ? true : false;

            if (enable_IISPH) {
			// --------- (HW Optional) if you implement IISPH please uncomment the following lines -----------		
            
                std::dynamic_pointer_cast<IISPH>(sph_base)->max_iter() = params.get_input<int>("max iter");
                std::dynamic_pointer_cast<IISPH>(sph_base)->omega() = params.get_input<float>("omega");

            // --------------------------------------------------------------------------------------------------------
            }
            else {
                std::dynamic_pointer_cast<WCSPH>(sph_base)->stiffness() = params.get_input<float>("stiffness");
                std::dynamic_pointer_cast<WCSPH>(sph_base)->exponent() = params.get_input<float>("exponent");
            }

    }
    else  // otherwise, step forward the simulation
    {
       sph_base->step(); 
    }

    // ------------------------- construct necessary output ---------------
    params.set_output("SPH Class", sph_base);


    //-------------------------
    auto build_output_geo = [&](MatrixXd vertices) {
        auto geometry = GOperandBase();
        auto points_component = std::make_shared<PointsComponent>(&geometry);
        geometry.attach_component(points_component);

        points_component->vertices = eigen_to_usd_vertices(vertices);
        float point_width = 0.05;
        points_component->width = pxr::VtArray<float>(vertices.rows(), point_width);

        return geometry;
    };


    
    auto geometry = build_output_geo(sph_base->get_fluid_particle_X());
    auto boundary_geo = build_output_geo(sph_base->get_boundary_X());
    //-------------------------

    auto color = eigen_to_usd_vertices(sph_base->get_vel_color_jet());

	params.set_output("point colors", std::move(color));
    params.set_output("points", std::move(geometry));
    params.set_output("boundary points", std::move(boundary_geo));
    // ----------------------------------------------------------------------------------------

}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "SPH Fluid");
    strcpy_s(ntype.id_name, "geom_sph_fluid");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_sph_fluid_exec;
    ntype.declare = node_sph_fluid_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_sph_fluid
