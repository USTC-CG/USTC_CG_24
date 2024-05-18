#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <chrono>
#include <iostream>
#include <memory>
#include <unordered_set>

#include "GCore/Components/MeshOperand.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"
#include "mass_spring/FastMassSpring.h"
#include "mass_spring/MassSpring.h"
#include "mass_spring/utils.h"
#include "utils/util_openmesh_bind.h"

namespace USTC_CG::node_mass_spring_with_control_points {

using namespace USTC_CG::node_mass_spring;
using namespace Eigen;

static void node_mass_spring_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::MassSpringSocket>("Mass Spring");
    b.add_input<decl::Geometry>("Simulated Mesh");
    b.add_input<decl::Geometry>("Controller");

    // Simulation parameters 
    b.add_input<decl::Float>("stiffness").default_val(1000).min(100).max(10000);
    b.add_input<decl::Float>("h").default_val(0.01).min(0.0).max(0.5);
    b.add_input<decl::Float>("damping").default_val(0.995).min(0.0).max(1.0);
    b.add_input<decl::Float>("gravity").default_val(-9.8);

    // Useful switches (0 or 1). You can add more if you like.
    b.add_input<decl::Int>("time integrator type").default_val(0).min(0).max(1); // 0 for implicit Euler, 1 for semi-implicit Euler
    b.add_input<decl::Int>("enable time profiling").default_val(0).min(0).max(1);
    b.add_input<decl::Int>("enable damping").default_val(0).min(0).max(1);
    b.add_input<decl::Int>("enable debug output").default_val(0).min(0).max(1);
    b.add_input<decl::Int>("enable sphere collision").default_val(0).min(0).max(1);

    // Optional switches
    b.add_input<decl::Int>("enable Liu13").default_val(0).min(0).max(1);

    // Current time in node system
    b.add_input<decl::Float>("time_code");

    // Output 
    b.add_output<decl::MassSpringSocket>("Mass Spring Class");
    b.add_output<decl::Geometry>("Output Mesh");
}

static void node_mass_spring_exec(ExeParams params)
{
    auto time_code = params.get_input<float>("time_code");

    auto mass_spring = params.get_input<std::shared_ptr<MassSpring>>("Mass Spring");

    auto controller_geom = params.get_input<GOperandBase>("Controller");
    auto controller_mesh = controller_geom.get_component<MeshComponent>();
    auto control_points = controller_mesh->controlPoints;

    auto geometry = params.get_input<GOperandBase>("Simulated Mesh");
    auto mesh = geometry.get_component<MeshComponent>();
    auto fixed_points = mesh->controlPoints;

    if (mesh->faceVertexCounts.size() == 0)
        throw std::runtime_error("Read simulated mesh USD error.");
    if (controller_mesh->faceVertexCounts.size() == 0)
		throw std::runtime_error("Read controller mesh USD error.");


    if (time_code == 0) {  // If time = 0, reset and initialize the mass spring class
        if (mesh) {
            if (mass_spring != nullptr)
				mass_spring.reset();

            auto edges =
                get_edges(usd_faces_to_eigen(mesh->faceVertexCounts, mesh->faceVertexIndices));
            auto vertices = usd_vertices_to_eigen(mesh->vertices);
            const float k = params.get_input<float>("stiffness");
            const float h = params.get_input<float>("h");

            bool enable_liu13 =  params.get_input<int>("enable Liu13") == 1 ? true : false;
            if (enable_liu13) { 
				mass_spring = std::make_shared<FastMassSpring>(vertices, edges, k, h);
			}
			else
				mass_spring = std::make_shared<MassSpring>(vertices, edges);

            if (!mass_spring->set_dirichlet_bc_mask(VtIntArray_to_vector_bool(fixed_points)))
                throw std::runtime_error("Mass Spring: set fixed points error.");
            if (!mass_spring->init_dirichlet_bc_vertices_control_pair(
                    usd_vertices_to_eigen(controller_mesh->vertices),
                    VtIntArray_to_vector_bool(control_points)))
                                throw std::runtime_error(
                                    "Mass Spring: init control points error.");

            // simulation parameters
            mass_spring->stiffness = k;
            mass_spring->h = params.get_input<float>("h");
            mass_spring->gravity = { 0, 0, params.get_input<float>("gravity")};
            mass_spring->damping = params.get_input<float>("damping");

            mass_spring->enable_sphere_collision = params.get_input<int>("enable sphere collision") == 1 ? true : false;
			mass_spring->enable_damping = params.get_input<int>("enable damping") == 1 ? true : false;
			mass_spring->time_integrator = params.get_input<int>("time integrator type") == 0 ? MassSpring::IMPLICIT_EULER : MassSpring::SEMI_IMPLICIT_EULER;
            mass_spring->enable_time_profiling = params.get_input<int>("enable time profiling") == 1 ? true : false;
            mass_spring->enable_debug_output = params.get_input<int>("enable debug output") == 1 ? true : false;
        }
        else {
            mass_spring = nullptr;
            throw std::runtime_error("Mass Spring: Need Geometry Input.");
        }
    }
    else  // otherwise, step forward the simulation
    {
        mass_spring->update_dirichlet_bc_vertices(usd_vertices_to_eigen(controller_mesh->vertices));
        mass_spring->step(); 
    }

    mesh->vertices = eigen_to_usd_vertices(mass_spring->getX());

    params.set_output("Mass Spring Class", mass_spring);
    params.set_output("Output Mesh", geometry);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Mass Spring With Control Points");
    strcpy_s(ntype.id_name, "geom_mass_spring_with_control_points");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_mass_spring_exec;
    ntype.declare = node_mass_spring_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_mass_spring
