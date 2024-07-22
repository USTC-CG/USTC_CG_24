#include "util_openmesh_bind.h"

#include "GCore/Components/MeshOperand.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
std::shared_ptr<PolyMesh> operand_to_openmesh(Geometry* mesh_oeprand)
{
    auto openmesh = std::make_shared<PolyMesh>();
    auto topology = mesh_oeprand->get_component<MeshComponent>();

    for (const auto& vv : topology->get_vertices()) {
        OpenMesh::Vec3f v;
        v[0] = vv[0];
        v[1] = vv[1];
        v[2] = vv[2];
        openmesh->add_vertex(v);
    }

    auto faceVertexIndices = topology->get_face_vertex_indices();
    auto faceVertexCounts = topology->get_face_vertex_counts();

    int vertexIndex = 0;
    for (int i = 0; i < faceVertexCounts.size(); i++) {
        // Create a vector of vertex handles for the face
        std::vector<PolyMesh::VertexHandle> face_vhandles;
        for (int j = 0; j < faceVertexCounts[i]; j++) {
            int index = faceVertexIndices[vertexIndex];
            // Get the vertex handle from the index
            PolyMesh::VertexHandle vh = openmesh->vertex_handle(index);
            // Add it to the vector
            face_vhandles.push_back(vh);
            vertexIndex++;
        }
        // Add the face to the mesh
        openmesh->add_face(face_vhandles);
    }
    return openmesh;
}

std::shared_ptr<Geometry> openmesh_to_operand(PolyMesh* openmesh)
{
    // TODO: test
    auto geometry = std::make_shared<Geometry>();
    std::shared_ptr<MeshComponent> mesh =
        std::make_shared<MeshComponent>(geometry.get());
    geometry->attach_component(mesh);

    pxr::VtArray<pxr::GfVec3f> points;
    pxr::VtArray<int> faceVertexIndices;
    pxr::VtArray<int> faceVertexCounts;

    // Set the points
    for (const auto& v : openmesh->vertices()) {
        const auto& p = openmesh->point(v);
        points.push_back(pxr::GfVec3f(p[0], p[1], p[2]));
    }
    // Set the topology
    for (const auto& f : openmesh->faces()) {
        size_t count = 0;
        for (const auto& vf : f.vertices()) {
            faceVertexIndices.push_back(vf.idx());
            count += 1;
        }
        faceVertexCounts.push_back(count);
    }

    mesh->set_vertices(points);
    mesh->set_face_vertex_indices(faceVertexIndices);
    mesh->set_face_vertex_counts(faceVertexCounts);
    return geometry;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
