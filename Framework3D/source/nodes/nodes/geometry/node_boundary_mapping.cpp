#include "GCore/Components/MeshOperand.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"
#include "utils/util_openmesh_bind.h"

/*
** @brief HW4_TutteParameterization
**
** This file contains two nodes whose primary function is to map the boundary of a mesh to a plain
** convex closed curve (circle of square), setting the stage for subsequent Laplacian equation
** solution and mesh parameterization tasks.
**
** Key to this node's implementation is the adept manipulation of half-edge data structures
** to identify and modify the boundary of the mesh.
**
** Task Overview:
** - The two execution functions (node_map_boundary_to_square_exec,
** node_map_boundary_to_circle_exec) require an update to accurately map the mesh boundary to a and
** circles. This entails identifying the boundary edges, evenly distributing boundary vertices along
** the square's perimeter, and ensuring the internal vertices' positions remain unchanged.
** - A focus on half-edge data structures to efficiently traverse and modify mesh boundaries.
*/

namespace USTC_CG::node_boundary_mapping {

/*
** HW4_TODO: Node to map the mesh boundary to a circle.
*/

static void node_map_boundary_to_circle_declare(NodeDeclarationBuilder& b)
{
    // Input-1: Original 3D mesh with boundary
    b.add_input<decl::Geometry>("Input");

    // Output-1: Processed 3D mesh whose boundary is mapped to a square and the interior vertices
    // remains the same
    b.add_output<decl::Geometry>("Output");
}

static void node_map_boundary_to_circle_exec(ExeParams params)
{
    // Get the input from params
    auto input = params.get_input<GOperandBase>("Input");

    // (TO BE UPDATED) Avoid processing the node when there is no input
    if (!input.get_component<MeshComponent>()) {
        throw std::runtime_error("Boundary Mapping: Need Geometry Input.");
    }
    throw std::runtime_error("Not implemented");

    /* ----------------------------- Preprocess -------------------------------
    ** Create a halfedge structure (using OpenMesh) for the input mesh. The
    ** half-edge data structure is a widely used data structure in geometric
    ** processing, offering convenient operations for traversing and modifying
    ** mesh elements.
    */
    auto halfedge_mesh = operand_to_openmesh(&input);

    /* ----------- [HW4_TODO] TASK 2.1: Boundary Mapping (to circle) ------------
    ** In this task, you are required to map the boundary of the mesh to a circle
    ** shape while ensuring the internal vertices remain unaffected. This step is
    ** crucial for setting up the mesh for subsequent parameterization tasks.
    **
    ** Algorithm Pseudocode for Boundary Mapping to Circle
    ** ------------------------------------------------------------------------
    ** 1. Identify the boundary loop(s) of the mesh using the half-edge structure.
    **
    ** 2. Calculate the total length of the boundary loop to determine the spacing
    **    between vertices when mapped to a square.
    **
    ** 3. Sequentially assign each boundary vertex a new position along the square's
    **    perimeter, maintaining the calculated spacing to ensure proper distribution.
    **
    ** 4. Keep the interior vertices' positions unchanged during this process.
    **
    ** Note: How to distribute the points on the circle?
    **
    ** Note: It would be better to normalize the boundary to a unit circle in [0,1]x[0,1] for
    ** texture mapping.
    */

    /* ----------------------------- Postprocess ------------------------------
    ** Convert the result mesh from the halfedge structure back to GOperandBase format as the node's
    ** output.
    */
    auto operand_base = openmesh_to_operand(halfedge_mesh.get());

    auto& output = input;
    output.get_component<MeshComponent>()->vertices =
        operand_base->get_component<MeshComponent>()->vertices;

    // Set the output of the nodes
    params.set_output("Output", std::move(output));
}

/*
** HW4_TODO: Node to map the mesh boundary to a square.
*/

static void node_map_boundary_to_square_declare(NodeDeclarationBuilder& b)
{
    // Input-1: Original 3D mesh with boundary
    b.add_input<decl::Geometry>("Input");

    // Output-1: Processed 3D mesh whose boundary is mapped to a square and the interior vertices
    // remains the same
    b.add_output<decl::Geometry>("Output");
}

static void node_map_boundary_to_square_exec(ExeParams params)
{
    // Get the input from params
    auto input = params.get_input<GOperandBase>("Input");

    // (TO BE UPDATED) Avoid processing the node when there is no input
    if (!input.get_component<MeshComponent>()) {
        throw std::runtime_error("Input does not contain a mesh");
    }
    throw std::runtime_error("Not implemented");

    /* ----------------------------- Preprocess -------------------------------
    ** Create a halfedge structure (using OpenMesh) for the input mesh.
    */
    auto halfedge_mesh = operand_to_openmesh(&input);

    /* ----------- [HW4_TODO] TASK 2.2: Boundary Mapping (to square) ------------
    ** In this task, you are required to map the boundary of the mesh to a circle
    ** shape while ensuring the internal vertices remain unaffected.
    **
    ** Algorithm Pseudocode for Boundary Mapping to Square
    ** ------------------------------------------------------------------------
    ** (omitted)
    **
    ** Note: Can you perserve the 4 corners of the square after boundary mapping?
    **
    ** Note: It would be better to normalize the boundary to a unit circle in [0,1]x[0,1] for
    ** texture mapping.
    */

    /* ----------------------------- Postprocess ------------------------------
    ** Convert the result mesh from the halfedge structure back to GOperandBase format as the node's
    ** output.
    */
    auto operand_base = openmesh_to_operand(halfedge_mesh.get());

    // Set the output of the nodes
    params.set_output("Output", std::move(*operand_base));
}

static void node_register()
{
    static NodeTypeInfo ntype_square, ntype_circle;

    strcpy(ntype_square.ui_name, "Map Boundary to Square");
    strcpy_s(ntype_square.id_name, "geom_map_boundary_to_square");

    geo_node_type_base(&ntype_square);
    ntype_square.node_execute = node_map_boundary_to_square_exec;
    ntype_square.declare = node_map_boundary_to_square_declare;
    nodeRegisterType(&ntype_square);

    strcpy(ntype_circle.ui_name, "Map Boundary to Circle");
    strcpy_s(ntype_circle.id_name, "geom_map_boundary_to_circle");

    geo_node_type_base(&ntype_circle);
    ntype_circle.node_execute = node_map_boundary_to_circle_exec;
    ntype_circle.declare = node_map_boundary_to_circle_declare;
    nodeRegisterType(&ntype_circle);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_boundary_mapping
