#include <openvdb/openvdb.h>
#include <openvdb/points/PointDataGrid.h>
#include <openvdb/tools/ParticlesToLevelSet.h>
#include <openvdb/tools/VolumeToMesh.h>

#include "GCore/Components/MeshOperand.h"
#include "GCore/Components/PointsComponent.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"

namespace USTC_CG::node_points_to_mesh {
static void node_declare(NodeDeclarationBuilder &b)
{
    b.add_input<decl::Geometry>("Points");
    b.add_output<decl::Geometry>("Mesh");
}

static void node_exec(ExeParams params)
{
    auto points_geometry = params.get_input<GOperandBase>("points");

    auto points = points_geometry.get_component<PointsComponent>();

    if (!points) {
        throw std::runtime_error("Input does not contain points");
    }

    auto mesh_geometry = GOperandBase();
    auto mesh_component = std::make_shared<MeshComponent>(&mesh_geometry);
    mesh_geometry.attach_component(mesh_component);

    pxr::VtArray<pxr::GfVec3f> points_vertices = points->vertices;

    pxr::VtArray<pxr::GfVec3f> &mesh_vertices = mesh_component->vertices;
    pxr::VtArray<int> &faceVertexCounts = mesh_component->faceVertexCounts;
    pxr::VtArray<int> &faceVertexIndices = mesh_component->faceVertexIndices;

    // Create a particle set
    std::vector<openvdb::Vec3f> particle_position;

    for (int i = 0; i < points_vertices.size(); ++i) {
        const pxr::GfVec3f &point = points_vertices[i];
        particle_position.emplace_back(point[0], point[1], point[2]);
    }

    // Use OpenVDB to convert the points_vertices into a mesh represented with face and vertices
    const float voxelSize = 0.50f, halfWidth = 2.0f;
    openvdb::FloatGrid::Ptr grid =
        openvdb::createLevelSet<openvdb::FloatGrid>(voxelSize, halfWidth);
    openvdb::tools::ParticlesToLevelSet<openvdb::FloatGrid> raster(*grid);

    raster.setGrainSize(1);  // a value of zero disables threading
    raster.rasterizeSpheres(particle_position, 1);

    std::vector<openvdb::Vec3s> converted_points;
    std::vector<openvdb::Vec4I> converted_quads;
    std::vector<openvdb::Vec3I> converted_triangles;

    openvdb::tools::volumeToMesh(
        *grid, converted_points, converted_triangles, converted_quads, 1, 0);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Points To Mesh");
    strcpy_s(ntype.id_name, "geom_points_to_mesh");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_points_to_mesh
