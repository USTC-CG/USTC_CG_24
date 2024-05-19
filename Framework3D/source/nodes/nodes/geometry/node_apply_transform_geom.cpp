#include <memory>

#include "GCore/Components/XformComponent.h"
#include "GCore/Components/MeshOperand.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"
#include "pxr/base/gf/matrix4f.h"
#include "pxr/base/gf/rotation.h"

namespace USTC_CG::node_apply_transform_geom {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Geometry>("Geometry");

    b.add_input<decl::Float>("Translate X").min(-10).max(10).default_val(0);
    b.add_input<decl::Float>("Translate Y").min(-10).max(10).default_val(0);
    b.add_input<decl::Float>("Translate Z").min(-10).max(10).default_val(0);

    b.add_input<decl::Float>("Rotate X").min(-180).max(180).default_val(0);
    b.add_input<decl::Float>("Rotate Y").min(-180).max(180).default_val(0);
    b.add_input<decl::Float>("Rotate Z").min(-180).max(180).default_val(0);

    b.add_input<decl::Float>("Scale X").min(0.1).max(10).default_val(1);
    b.add_input<decl::Float>("Scale Y").min(0.1).max(10).default_val(1);
    b.add_input<decl::Float>("Scale Z").min(0.1).max(10).default_val(1);

    b.add_output<decl::Geometry>("Geometry");
}

static void node_exec(ExeParams params)
{
    auto geometry = params.get_input<GOperandBase>("Geometry");

    auto t_x = params.get_input<float>("Translate X");
    auto t_y = params.get_input<float>("Translate Y");
    auto t_z = params.get_input<float>("Translate Z");

    auto r_x = params.get_input<float>("Rotate X");
    auto r_y = params.get_input<float>("Rotate Y");
    auto r_z = params.get_input<float>("Rotate Z");

    auto s_x = params.get_input<float>("Scale X");
    auto s_y = params.get_input<float>("Scale Y");
    auto s_z = params.get_input<float>("Scale Z");

    pxr::GfMatrix4d final_transform;
    final_transform.SetIdentity();

    pxr::GfMatrix4d t;
    t.SetTranslate(pxr::GfVec3d(t_x, t_y, t_z));
    pxr::GfMatrix4d s;
    s.SetScale(pxr::GfVec3d(s_x, s_y, s_z));

    pxr::GfMatrix4d mat_r_x;
    mat_r_x.SetRotate(pxr::GfRotation{ { 1, 0, 0 }, r_x });
    pxr::GfMatrix4d mat_r_y;
    mat_r_y.SetRotate(pxr::GfRotation{ { 0, 1, 0 }, r_y });
    pxr::GfMatrix4d mat_r_z;
    mat_r_z.SetRotate(pxr::GfRotation{ { 0, 0, 1 }, r_z });

	auto transform = mat_r_x * mat_r_y * mat_r_z * s * t;
	final_transform = final_transform * transform;

    auto mesh = geometry.get_component<MeshComponent>();
    for (int i = 0; i < mesh->vertices.size(); i++)
    {
        mesh->vertices[i] = final_transform.TransformAffine(mesh->vertices[i]);
    }

    params.set_output("Geometry", std::move(geometry));
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Apply Transform Geometry");
    strcpy_s(ntype.id_name, "geom_apply_transform_geom");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_apply_transform_geom
