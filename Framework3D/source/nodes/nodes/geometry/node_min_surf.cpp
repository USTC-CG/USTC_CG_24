#include "GCore/Components/MeshOperand.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"
#include "utils/util_openmesh_bind.h"

/*
** @brief HW4_TutteParameterization
**
** This file presents the basic framework of a "node", which processes inputs
** received from the left and outputs specific variables for downstream nodes to
** use.
** - In the first function, node_declare, you can set up the node's input and
** output variables.
** - The second function, node_exec is the execution part of the node, where we
** need to implement the node's functionality.
** - The third function generates the node's registration information, which
** eventually allows placing this node in the GUI interface.
**
** Your task is to fill in the required logic at the specified locations
** within this template, especially in node_exec.
*/

namespace USTC_CG::node_min_surf {
static void node_min_surf_declare(NodeDeclarationBuilder& b)
{
    // Input-1: Original 3D mesh with boundary
    b.add_input<decl::Geometry>("Input");

    /*
    ** NOTE: You can add more inputs or outputs if necessary. For example, in some cases,
    ** additional information (e.g. other mesh geometry, other parameters) is required to perform
    ** the computation.
    **
    ** Be sure that the input/outputs do not share the same name. You can add one geometry as
    **
    **                b.add_input<decl::Geometry>("Input");
    **
    ** Or maybe you need a value buffer like:
    **
    **                b.add_input<decl::Float1Buffer>("Weights");
    */

    // Output-1: Minimal surface with fixed boundary
    b.add_output<decl::Geometry>("Output");
}

static void node_min_surf_exec(ExeParams params)
{
    // Get the input from params
    auto input = params.get_input<GOperandBase>("Input");

    // (TO BE UPDATED) Avoid processing the node when there is no input
    if (!input.get_component<MeshComponent>()) {
        throw std::runtime_error("Minimal Surface: Need Geometry Input.");
    }
    throw std::runtime_error("Not implemented");

    /* ----------------------------- Preprocess -------------------------------
    ** Create a halfedge structure (using OpenMesh) for the input mesh. The
    ** half-edge data structure is a widely used data structure in geometric
    ** processing, offering convenient operations for traversing and modifying
    ** mesh elements.
    */
    auto halfedge_mesh = operand_to_openmesh(&input);

    /* ---------------- [HW4_TODO] TASK 1: Minimal Surface --------------------
    ** In this task, you are required to generate a 'minimal surface' mesh with
    ** the boundary of the input mesh as its boundary.
    **
    ** Specifically, the positions of the boundary vertices of the input mesh
    ** should be fixed. By solving a global Laplace equation on the mesh,
    ** recalculate the coordinates of the vertices inside the mesh to achieve
    ** the minimal surface configuration.
    **
    ** (Recall the Poisson equation with Dirichlet Boundary Condition in HW3)
    */

    /*
    ** Algorithm Pseudocode for Minimal Surface Calculation
    ** ------------------------------------------------------------------------
    ** 1. Initialize mesh with input boundary conditions.
    **    - For each boundary vertex, fix its position.
    **    - For internal vertices, initialize with initial guess if necessary.
    **
    ** 2. Construct Laplacian matrix for the mesh.
    **    - Compute weights for each edge based on the chosen weighting scheme
    **      (e.g., uniform weights for simplicity).
    **    - Assemble the global Laplacian matrix.
    **
    ** 3. Solve the Laplace equation for interior vertices.
    **    - Apply Dirichlet boundary conditions for boundary vertices.
    **    - Solve the linear system (Laplacian * X = 0) to find new positions
    **      for internal vertices.
    **
    ** 4. Update mesh geometry with new vertex positions.
    **    - Ensure the mesh respects the minimal surface configuration.
    **
    ** Note: This pseudocode outlines the general steps for calculating a
    ** minimal surface mesh given fixed boundary conditions using the Laplace
    ** equation. The specific implementation details may vary based on the mesh
    ** representation and numerical methods used.
    **
    */

    /* ----------------------------- Postprocess ------------------------------
    ** Convert the minimal surface mesh from the halfedge structure back to
    ** GOperandBase format as the node's output.
    */
    auto operand_base = openmesh_to_operand(halfedge_mesh.get());

    // Set the output of the nodes
    params.set_output("Output", std::move(*operand_base));
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Minimal Surface");
    strcpy_s(ntype.id_name, "geom_min_surf");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_min_surf_exec;
    ntype.declare = node_min_surf_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_min_surf
