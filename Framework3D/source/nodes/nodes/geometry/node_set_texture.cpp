#include "GCore/Components/MaterialComponent.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"

namespace USTC_CG::node_set_texture {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Geometry>("Geometry");
    b.add_input<decl::String>("Texture Name").default_val("");
    b.add_output<decl::Geometry>("Geometry");
}

static void node_exec(ExeParams params)
{
    auto texture = params.get_input<std::string>("Texture Name");

    auto geometry = params.get_input<GOperandBase>("Geometry");
    auto material = geometry.get_component<MaterialComponent>();
    if (!material) {
        material = std::make_shared<MaterialComponent>(&geometry);
    }
    material->textures.clear();
    material->textures.push_back(texture);
    geometry.attach_component(material);

    params.set_output("Geometry", std::move(geometry));
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Mesh Add Texture");
    strcpy_s(ntype.id_name, "geom_set_texture");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_set_texture
