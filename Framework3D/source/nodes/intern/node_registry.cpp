#include "../nodes/functions/NODES_FILES_DIR.h"
#include "Nodes/all_socket_types.hpp"
#include "Nodes/node.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "USTC_CG.h"
#include "boost/python.hpp"
#include "boost/python/numpy.hpp"
#include "entt/meta/resolve.hpp"
USTC_CG_NAMESPACE_OPEN_SCOPE
static void reset_declaration(NodeDeclaration& declaration)
{
    std::destroy_at(&declaration);
    new (&declaration) NodeDeclaration();
}

void build_node_declaration(
    const NodeTypeInfo& typeinfo,
    NodeDeclaration& r_declaration,
    const NodeTree* ntree,
    const Node* node)
{
    reset_declaration(r_declaration);
    NodeDeclarationBuilder node_decl_builder{ r_declaration, ntree, node };
    typeinfo.declare(node_decl_builder);
}

static std::map<std::string, NodeTypeInfo*> geo_node_registry;

const std::map<std::string, NodeTypeInfo*>& get_geo_node_registry()
{
    return geo_node_registry;
}

static std::map<std::string, NodeTypeInfo*> render_node_registry;

const std::map<std::string, NodeTypeInfo*>& get_render_node_registry()
{
    return render_node_registry;
}

static std::map<std::string, NodeTypeInfo*> func_node_registry;

const std::map<std::string, NodeTypeInfo*>& get_func_node_registry()
{
    return func_node_registry;
}

static std::map<std::string, NodeTypeInfo*> composition_node_registry;

const std::map<std::string, NodeTypeInfo*>& get_composition_node_registry()
{
    return composition_node_registry;
}

std::map<std::string, NodeTypeInfo*> conversion_node_registry;

const std::map<std::string, NodeTypeInfo*>& get_conversion_node_registry()
{
    return conversion_node_registry;
}

void nodeRegisterType(NodeTypeInfo* type_info)
{
    switch (type_info->node_type_of_grpah) {
        case NodeTypeOfGrpah::Geometry:
            geo_node_registry[type_info->id_name] = type_info;
            break;
        case NodeTypeOfGrpah::Render:
            render_node_registry[type_info->id_name] = type_info;
            break;
        case NodeTypeOfGrpah::Function:
            func_node_registry[type_info->id_name] = type_info;
            break;
        case NodeTypeOfGrpah::Composition:
            composition_node_registry[type_info->id_name] = type_info;
            break;
        case NodeTypeOfGrpah::Conversion:
            conversion_node_registry[type_info->id_name] = type_info;
            break;
        default: logging("Unknown graph type of node.", Error);
    }

    if (type_info->declare) {
        type_info->static_declaration = std::make_unique<NodeDeclaration>();
    }
    build_node_declaration(
        *type_info, *type_info->static_declaration, nullptr, nullptr);
}

NodeTypeInfo* nodeTypeFind(const char* idname)
{
    if (idname[0]) {
        NodeTypeInfo* nt;

        auto find_type =
            [idname,
             &nt](const std::map<std::string, NodeTypeInfo*>& node_registry) {
                if (node_registry.find(std::string(idname)) !=
                    node_registry.end()) {
                    nt = node_registry.at(std::string(idname));
                }
            };

        find_type(get_geo_node_registry());
        find_type(get_render_node_registry());
        find_type(get_func_node_registry());
        find_type(get_composition_node_registry());
        find_type(get_conversion_node_registry());

        if (nt)
            return nt;
    }
    throw std::runtime_error("Id name not found.");
}

void register_all()
{
    SetEnvironmentVariable(
        "PYTHONPATH",
        FUNC_NODES_FILES_DIR "/scripts;" RENDER_NODES_FILES_DIR "/scripts");

    Py_Initialize();
    boost::python::numpy::initialize();
    register_cpp_types();
    register_nodes();
    register_sockets();
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
