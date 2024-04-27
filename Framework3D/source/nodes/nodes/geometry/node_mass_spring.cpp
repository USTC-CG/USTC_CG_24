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
#include "utils/util_openmesh_bind.h"

namespace USTC_CG::node_mass_spring {

// -------------------- helper functions (No need to modify) --------------------

Eigen::MatrixXi usd_faces_to_eigen(
    const pxr::VtArray<int>& faceVertexCount,
    const pxr::VtArray<int>& faceVertexIndices)
{
    unsigned nFaces = faceVertexCount.size();
    Eigen::MatrixXi F(nFaces, 3);
    unsigned count = 0;
    for (int i = 0; i < nFaces; i++) {
        for (int j = 0; j < 3; j++) {
            F(i, j) = faceVertexIndices[count];
            count += 1;
        }
    }
    return F;
}
Eigen::MatrixXd usd_vertices_to_eigen(const pxr::VtArray<pxr::GfVec3f>& v)
{
    unsigned nVertices = v.size();
    Eigen::MatrixXd V(nVertices, 3);
    for (int i = 0; i < nVertices; i++) {
        for (int j = 0; j < 3; j++) {
            V(i, j) = v[i][j];
        }
    }
    return V;
}

pxr::VtArray<pxr::GfVec3f> eigen_to_usd_vertices(const Eigen::MatrixXd& V)
{
    pxr::VtArray<pxr::GfVec3f> vertices;
    for (int i = 0; i < V.rows(); i++) {
        vertices.push_back(pxr::GfVec3f(V(i, 0), V(i, 1), V(i, 2)));
    }
    return vertices;
}

// Here F is of shape [nFaces, 3] for triangular mesh
EdgeSet get_edges(const Eigen::MatrixXi& F)
{
    EdgeSet edges;
    for (int i = 0; i < F.rows(); i++) {
        for (int j = 0; j < F.cols(); j++) {
            int v0 = F(i, j);
            int v1 = F(i, (j + 1) % F.cols());
            if (v0 > v1) {
                std::swap(v0, v1);
            }
            edges.insert(std::make_pair(v0, v1));
        }
    }
    return edges;
}

using namespace Eigen;

// --------------------------------------------------------------------------

static void node_mass_spring_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Geometry>("Mesh");

    b.add_input<decl::MassSpringSocket>("Mass Spring");

    // Simulation parameters 
    b.add_input<decl::Float>("stiffness").default_val(1000).min(100).max(10000);
    b.add_input<decl::Float>("h").default_val(0.01).min(0.0).max(0.5);
    b.add_input<decl::Float>("damping").default_val(0.995).min(0.0).max(1.0);
    b.add_input<decl::Float>("gravity").default_val(-9.8);

    // --------- HW Optional: if you implement sphere collision, please uncomment the following lines ------------
    b.add_input<decl::Float>("collision penalty_k").default_val(10000).min(100).max(100000); 
    b.add_input<decl::Float>("collision scale factor").default_val(1.1).min(1.0).max(2.0); 
    b.add_input<decl::Float>("sphere radius").default_val(0.4).min(0.0).max(5.0);; 
    b.add_input<decl::Float3>("sphere center");
    // -----------------------------------------------------------------------------------------------------------

    // Useful switches (0 or 1). You can add more if you like.
    b.add_input<decl::Int>("time integrator type").default_val(0).min(0).max(1); // 0 for implicit Euler, 1 for semi-implicit Euler
    b.add_input<decl::Int>("enable time profiling").default_val(0).min(0).max(1);
    b.add_input<decl::Int>("enable damping").default_val(0).min(0).max(1);
    b.add_input<decl::Int>("enable debug output").default_val(0).min(0).max(1);

    // Optional switches
    b.add_input<decl::Int>("enable Liu13").default_val(0).min(0).max(1);
    b.add_input<decl::Int>("enable sphere collision").default_val(0).min(0).max(1);

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

    auto geometry = params.get_input<GOperandBase>("Mesh");
    auto mesh = geometry.get_component<MeshComponent>();
    if (mesh->faceVertexCounts.size() == 0)
    {
        throw std::runtime_error("Read USD error.");
    }


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
                // HW Optional 
				mass_spring = std::make_shared<FastMassSpring>(vertices, edges, k, h);
			}
			else
				mass_spring = std::make_shared<MassSpring>(vertices, edges);

            // simulation parameters
            mass_spring->stiffness = k;
            mass_spring->h = params.get_input<float>("h");
            mass_spring->gravity = { 0, 0, params.get_input<float>("gravity")};
            mass_spring->damping = params.get_input<float>("damping");

            // Optional parameters
			// --------- HW Optional: if you implement sphere collision, please uncomment the following lines ------------
            mass_spring->collision_penalty_k = params.get_input<float>("collision penalty_k");
            mass_spring->collision_scale_factor = params.get_input<float>("collision scale factor");
            auto c = params.get_input<pxr::GfVec3f>("sphere center"); 
            mass_spring->sphere_center = {c[0], c[1], c[2]};
            mass_spring->sphere_radius = params.get_input<float>("sphere radius");
			// --------------------------------------------------------------------------------------------------------

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
        mass_spring->step(); 
    }

    mesh->vertices = eigen_to_usd_vertices(mass_spring->getX());

    params.set_output("Mass Spring Class", mass_spring);
    params.set_output("Output Mesh", geometry);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Mass Spring");
    strcpy_s(ntype.id_name, "geom_mass_spring");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_mass_spring_exec;
    ntype.declare = node_mass_spring_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_mass_spring
