// #define __GNUC__
#include "GCore/Components/CurveComponent.h"
#include "GCore/Components/MeshOperand.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"

namespace USTC_CG::node_create_geom {

static void node_create_grid_declare(NodeDeclarationBuilder &b)
{
    b.add_input<decl::Int>("resolution").min(1).max(20).default_val(2);
    b.add_input<decl::Float>("size").min(1).max(20);
    b.add_output<decl::Geometry>("Geometry");
}

static void node_create_grid_exec(ExeParams params)
{
    int resolution = params.get_input<int>("resolution") + 1;
    float size = params.get_input<float>("size");
    Geometry geometry;
    std::shared_ptr<MeshComponent> mesh =
        std::make_shared<MeshComponent>(&geometry);
    geometry.attach_component(mesh);

    pxr::VtArray<pxr::GfVec3f> points;
    pxr::VtArray<pxr::GfVec2f> texcoord;
    pxr::VtArray<int> faceVertexIndices;
    pxr::VtArray<int> faceVertexCounts;

    for (int i = 0; i < resolution; ++i) {
        for (int j = 0; j < resolution; ++j) {
            float y = size * static_cast<float>(i) / (resolution - 1);
            float z = size * static_cast<float>(j) / (resolution - 1);

            float u = static_cast<float>(i) / (resolution - 1);
            float v = static_cast<float>(j) / (resolution - 1);
            points.push_back(pxr::GfVec3f(0, y, z));
            texcoord.push_back(pxr::GfVec2f(u, v));
        }
    }

    for (int i = 0; i < resolution - 1; ++i) {
        for (int j = 0; j < resolution - 1; ++j) {
            faceVertexCounts.push_back(4);
            faceVertexIndices.push_back(i * resolution + j);
            faceVertexIndices.push_back(i * resolution + j + 1);
            faceVertexIndices.push_back((i + 1) * resolution + j + 1);
            faceVertexIndices.push_back((i + 1) * resolution + j);
        }
    }

    mesh->set_vertices(points);
    mesh->set_face_vertex_indices(faceVertexIndices);
    mesh->set_face_vertex_counts(faceVertexCounts);
    mesh->set_texcoords_array(texcoord);

    params.set_output("Geometry", std::move(geometry));
}

void node_create_circle_declare(NodeDeclarationBuilder &b)
{
    b.add_input<decl::Int>("resolution").min(1).max(100).default_val(10);
    b.add_input<decl::Float>("radius").min(1).max(20);
    b.add_output<decl::Geometry>("Circle");
}

void node_create_circle_exec(ExeParams params)
{
    int resolution = params.get_input<int>("resolution");
    float radius = params.get_input<float>("radius");
    Geometry geometry;
    std::shared_ptr<CurveComponent> curve =
        std::make_shared<CurveComponent>(&geometry);
    geometry.attach_component(curve);

    pxr::VtArray<pxr::GfVec3f> points;

    pxr::GfVec3f center(0.0f, 0.0f, 0.0f);

    float angleStep = 2.0f * M_PI / resolution;

    for (int i = 0; i < resolution; ++i) {
        float angle = i * angleStep;
        pxr::GfVec3f point(
            radius * std::cos(angle) + center[0],
            radius * std::sin(angle) + center[1],
            center[2]);
        points.push_back(point);
    }

    curve->set_vertices(points);
    curve->set_vert_count({ resolution });

    curve->get_usd_curve().CreateWrapAttr(
        pxr::VtValue(pxr::UsdGeomTokens->periodic));

    params.set_output("Circle", std::move(geometry));
}

void node_create_spiral_declare(NodeDeclarationBuilder &b)
{
    b.add_input<decl::Int>("resolution").min(1).max(100).default_val(10);
    b.add_input<decl::Float>("R1").min(0.1).max(10).default_val(1);
    b.add_input<decl::Float>("R2").min(0.1).max(10).default_val(1);
    b.add_input<decl::Float>("Circle Count").min(0.1).max(10).default_val(2);
    b.add_input<decl::Float>("Height").min(0.1).max(10).default_val(1);
    b.add_output<decl::Geometry>("Curve");
}

void node_create_spiral_exec(ExeParams params)
{
    int resolution = params.get_input<int>("resolution");
    float R1 = params.get_input<float>("R1");
    float R2 = params.get_input<float>("R2");
    float circleCount = params.get_input<float>("Circle Count");
    float height = params.get_input<float>("Height");

    Geometry geometry;
    std::shared_ptr<CurveComponent> curve =
        std::make_shared<CurveComponent>(&geometry);
    geometry.attach_component(curve);

    pxr::VtArray<pxr::GfVec3f> points;

    float angleStep = circleCount * 2.0f * M_PI / resolution;
    float radiusIncrement = (R2 - R1) / resolution;
    float heightIncrement = height / resolution;

    for (int i = 0; i < resolution; ++i) {
        float angle = i * angleStep;
        float radius = R1 + radiusIncrement * i;
        float z = heightIncrement * i;
        pxr::GfVec3f point(
            radius * std::cos(angle), radius * std::sin(angle), z);
        points.push_back(point);
    }

    curve->set_vertices(points);
    curve->set_vert_count({ resolution });

    // Since a spiral is not periodic, we don't set a wrap attribute like we did
    // for the circle.

    params.set_output("Curve", std::move(geometry));
}

static void node_register()
{
#define CreateGeom(lower, Upper)                             \
    static NodeTypeInfo lower##_ntype;                       \
    strcpy(lower##_ntype.ui_name, "Create " #Upper);         \
    strcpy(lower##_ntype.id_name, "geom_create_" #lower);    \
    geo_node_type_base(&lower##_ntype);                      \
    lower##_ntype.node_execute = node_create_##lower##_exec; \
    lower##_ntype.declare = node_create_##lower##_declare;   \
    nodeRegisterType(&lower##_ntype);

    CreateGeom(grid, Grid);
    CreateGeom(circle, Circle);
    CreateGeom(spiral, Spiral);
    // CreateGeom(ico_sphere, Ico Sphere);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_create_geom
