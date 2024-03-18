#include "util_openmesh_bind.h"

#include "GCore/Components/MeshOperand.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
std::shared_ptr<PolyMesh> operand_to_openmesh(GOperandBase* mesh_oeprand)
{
    auto openmesh = std::make_shared<PolyMesh>();
    auto topology = mesh_oeprand->get_component<MeshComponent>();

    for (const auto& vv : topology->vertices) {
        OpenMesh::Vec3f v;
        v[0] = vv[0];
        v[1] = vv[1];
        v[2] = vv[2];
        openmesh->add_vertex(v);
    }

    auto faceVertexIndices = topology->faceVertexIndices;
    auto faceVertexCounts = topology->faceVertexCounts;

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

std::shared_ptr<GOperandBase> openmesh_to_operand(PolyMesh* openmesh)
{
    // TODO: test
    auto operand_base = std::make_shared<GOperandBase>();
    std::shared_ptr<MeshComponent> mesh =
        std::make_shared<MeshComponent>(operand_base.get());
    operand_base->attach_component(mesh);

    auto& points = mesh->vertices;
    auto& faceVertexIndices = mesh->faceVertexIndices;
    auto& faceVertexCounts = mesh->faceVertexCounts;

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
    return operand_base;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
