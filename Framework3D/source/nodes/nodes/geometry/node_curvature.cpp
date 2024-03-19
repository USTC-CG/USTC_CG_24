#include "GCore/Components/MeshOperand.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"
#include "utils/util_openmesh_bind.h"

namespace USTC_CG::node_curvature {
static void node_curvature_declare(NodeDeclarationBuilder& b)
{
    // Input-1: Original 3D mesh with boundary
    b.add_input<decl::Geometry>("Input");
    // Output-1: The curvature at each vertex (Gauss curvature)
    b.add_output<decl::Float1Buffer>("Output");
}

static void node_curvature_exec(ExeParams params)
{
    // An example of halfedge mesh processing
    // In this function we would demonstrate:
    // 1. Get the number of items (vertex, face, halfedge, edge) in halfedge mesh (n_vertices()...)
    // 2. Visit the items (vertices()...)
    // 3. Get the index of an item (handle.idx())
    // 4. 3D position of a vertex (point())
    // 5. Get the related items from one (smart) halfedge handle:
    //   - .to(), .from(), .next(), .prev(), .opp()...
    // 6. Visit neighbors
    //   - .outgoing_halfedges(), voh_range(), ...

    // Get the input from params
    auto input = params.get_input<GOperandBase>("Input");

    // (TO BE UPDATED) Avoid processing the node when there is no input
    if (!input.get_component<MeshComponent>()) {
        // throw std::runtime_error("CurvatureNode: Input doesn't contain a mesh.");
        throw std::runtime_error("Curvature: Need Geometry Input.");
    }

    // This is the halfedge mesh we get
    auto halfedge_mesh = operand_to_openmesh(&input);
    // Store the output
    // One can use the function n_vertices(), n_faces(), n_halfedges() to get the numbers of items
    pxr::VtArray<float> rst(halfedge_mesh->n_vertices());

    // For each vertex, we compute the Gauss curvature��
    // First, we need to iterate through all the vertices:
    // - OpenMesh uses "handles" to represent each
    // - The following iteration visit all the "VertexHandles"
    // - One can visit other items using:
    //   - halfedge_mesh->faces()
    //   - halfedge_mesh->halfedges()
    //   - halfedge_mesh->edges()
    for (const auto& vertex_handle : halfedge_mesh->vertices()) {
        float area_v = 0.f;
        float theta_sum = 0.f;
        // Use the point() method to find the 3D location of a vertex
        const auto& position = halfedge_mesh->point(vertex_handle);
        // For each vertex v, the gauss curvature can be computed by:
        //   K_v = (2 PI - \sum_{f\in N(v)} \theta_f) / Area(v)
        // We need to visit the faces near current vertex:
        // - You can get the nearby faces using vertex_handle.faces()
        //   if the type of the handle is SmartVertexHandle, which records
        //   the mesh information in the handle.
        // - The same as the iteration:
        //   for (const auto& face_handle : halfedge_mesh->vf_range(vertex_handle))
        // Here we visit the outgoing halfedges to compute the angles and areas
        for (const auto& halfedge_handle : vertex_handle.outgoing_halfedges()) {
            // vertex_handle, v1, v2 forms a face near v
            const auto& v1 = halfedge_handle.to();
            const auto& v2 = halfedge_handle.prev().opp().to();
            const auto& vec1 = halfedge_mesh->point(v1) - position;
            const auto& vec2 = halfedge_mesh->point(v2) - position;
            // The area formed by vec1 and vec2:
            float area = (vec1.cross(vec2)).norm() / 2;
            // The angle between vec1 and vec2:
            float cos = vec1.dot(vec2) / (vec1.norm() * vec2.norm());
            float theta = acosf(cos);
            // Assemble the sumations
            area_v += area / 3.0f;
            theta_sum += theta;
        }
        // Finaly we come to the Gauss curvature of this vertex
        float K = (2 * M_PI - theta_sum) / area_v;
        // Use vertex_handle.idx() to get the index of the vertex
        rst[vertex_handle.idx()] = K;
    }
    auto operand_base = openmesh_to_operand(halfedge_mesh.get());

    // Set the output of the nodes
    params.set_output("Output", rst);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Curvature");
    strcpy_s(ntype.id_name, "geom_curvature");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_curvature_exec;
    ntype.declare = node_curvature_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_curvature
