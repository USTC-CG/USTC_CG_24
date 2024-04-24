#include "Nodes/all_socket_types.hpp"
#include "Nodes/node.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "USTC_CG.h"
#include "Utils/Macro/map.h"
#include "rich_type_buffer.hpp"
USTC_CG_NAMESPACE_OPEN_SCOPE
namespace node_mass_spring {
    class MassSpring;
}

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
    node_decl_builder.finalize();
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

void nodeRegisterType(NodeTypeInfo* type_info)
{
    switch (type_info->node_type_of_grpah) {
        case NodeTypeOfGrpah::Geometry: geo_node_registry[type_info->id_name] = type_info; break;
        case NodeTypeOfGrpah::Render: render_node_registry[type_info->id_name] = type_info; break;
        case NodeTypeOfGrpah::Function: func_node_registry[type_info->id_name] = type_info; break;
        case NodeTypeOfGrpah::Composition:
            composition_node_registry[type_info->id_name] = type_info;
            break;
        default: logging("Unknown graph type of node.", Error);
    }

    if (type_info->declare) {
        type_info->static_declaration = std::make_unique<NodeDeclaration>();
    }
    build_node_declaration(*type_info, *type_info->static_declaration, nullptr, nullptr);
}

NodeTypeInfo* nodeTypeFind(const char* idname)
{
    if (idname[0]) {
        NodeTypeInfo* nt;

        auto find_type = [idname, &nt](const std::map<std::string, NodeTypeInfo*>& node_registry) {
            if (node_registry.find(std::string(idname)) != node_registry.end()) {
                nt = node_registry.at(std::string(idname));
            }
        };

        find_type(get_geo_node_registry());
        find_type(get_render_node_registry());
        find_type(get_func_node_registry());
        find_type(get_composition_node_registry());

        if (nt)
            return nt;
    }
    throw std::runtime_error("Id name not found.");
}

static std::map<std::string, std::unique_ptr<SocketTypeInfo>> socket_registry;

SocketTypeInfo* socketTypeFind(const char* idname)
{
    if (idname[0]) {
        auto& registry = socket_registry;
        auto nt = registry.at(std::string(idname)).get();
        if (nt) {
            return nt;
        }
    }

    return nullptr;
}

#define TYPE_NAME(CASE) \
    case SocketType::CASE: return "Socket" #CASE;

const char* get_socket_typename(SocketType socket)
{
    switch (socket) {
        MACRO_MAP(TYPE_NAME, ALL_SOCKET_TYPES)
        default: return "";
    }
}

SocketTypeInfo* make_standard_socket_type(SocketType socket)
{
    auto type_info = new SocketTypeInfo();
    type_info->type = socket;
    strcpy(type_info->type_name, get_socket_typename(socket));
    return type_info;
}

#define MakeType(Type, Item, Size, Buffer)                                                    \
    static SocketTypeInfo* make_socket_type_##Type##Size##Buffer()                            \
    {                                                                                         \
        SocketTypeInfo* socktype = make_standard_socket_type(SocketType::Type##Size##Buffer); \
        socktype->cpp_type = &CPPType::get<pxr::VtArray<Item>>();                             \
        return socktype;                                                                      \
    }

MakeType(Float, float, 1, Buffer);
MakeType(Float, pxr::GfVec2f, 2, Buffer);
MakeType(Float, pxr::GfVec3f, 3, Buffer);
MakeType(Float, pxr::GfVec4f, 4, Buffer);
MakeType(Int, int, 1, Buffer);
MakeType(Int, pxr::GfVec2i, 2, Buffer);
MakeType(Int, pxr::GfVec3i, 3, Buffer);
MakeType(Int, pxr::GfVec4i, 4, Buffer);
static SocketTypeInfo* make_socket_type_Float2()
{
    SocketTypeInfo* socktype = make_standard_socket_type(SocketType::Float2);
    socktype->cpp_type = &CPPType::get<pxr::GfVec2f>();
    return socktype;
};
static SocketTypeInfo* make_socket_type_Float3()
{
    SocketTypeInfo* socktype = make_standard_socket_type(SocketType::Float3);
    socktype->cpp_type = &CPPType::get<pxr::GfVec3f>();
    return socktype;
};
static SocketTypeInfo* make_socket_type_Float4()
{
    SocketTypeInfo* socktype = make_standard_socket_type(SocketType::Float4);
    socktype->cpp_type = &CPPType::get<pxr::GfVec4f>();
    return socktype;
};
MakeType(Int, pxr::GfVec2i, 2);
MakeType(Int, pxr::GfVec3i, 3);
MakeType(Int, pxr::GfVec4i, 4);
#undef MakeTypeBuffer

static SocketTypeInfo* make_socket_type_Int()
{
    SocketTypeInfo* socktype = make_standard_socket_type(SocketType::Int);
    socktype->cpp_type = &CPPType::get<int>();
    return socktype;
}

static SocketTypeInfo* make_socket_type_Float()
{
    SocketTypeInfo* socktype = make_standard_socket_type(SocketType::Float);
    socktype->cpp_type = &CPPType::get<float>();
    return socktype;
}

static SocketTypeInfo* make_socket_type_String()
{
    SocketTypeInfo* socktype = make_standard_socket_type(SocketType::String);
    socktype->cpp_type = &CPPType::get<std::string>();
    return socktype;
}

static SocketTypeInfo* make_socket_type_Any()
{
    SocketTypeInfo* socktype = make_standard_socket_type(SocketType::Any);
    socktype->cpp_type = &CPPType::get<GMutablePointer>();
    socktype->canLinkTo = [](SocketType type) { return type != SocketType::Any; };
    return socktype;
}

static SocketTypeInfo* make_socket_type_Geometry()
{
    SocketTypeInfo* socktype = make_standard_socket_type(SocketType::Geometry);
    socktype->cpp_type = &CPPType::get<GOperandBase>();
    return socktype;
}

static SocketTypeInfo* make_socket_type_Lights()
{
    SocketTypeInfo* socktype = make_standard_socket_type(SocketType::Lights);
    socktype->cpp_type = &CPPType::get<LightArray>();
    return socktype;
}

static SocketTypeInfo* make_socket_type_Layer()
{
    SocketTypeInfo* socktype = make_standard_socket_type(SocketType::Layer);
    socktype->cpp_type = &CPPType::get<pxr::UsdStageRefPtr>();
    return socktype;
}

static SocketTypeInfo* make_socket_type_Camera()
{
    SocketTypeInfo* socktype = make_standard_socket_type(SocketType::Camera);
    socktype->cpp_type = &CPPType::get<CameraArray>();
    return socktype;
}

static SocketTypeInfo* make_socket_type_Meshes()
{
    SocketTypeInfo* socktype = make_standard_socket_type(SocketType::Meshes);
    socktype->cpp_type = &CPPType::get<MeshArray>();
    return socktype;
}

static SocketTypeInfo* make_socket_type_Texture()
{
    SocketTypeInfo* socktype = make_standard_socket_type(SocketType::Texture);
    socktype->cpp_type = &CPPType::get<TextureHandle>();
    return socktype;
}

static SocketTypeInfo* make_socket_type_Materials()
{
    SocketTypeInfo* socktype = make_standard_socket_type(SocketType::Materials);
    socktype->cpp_type = &CPPType::get<MaterialMap>();
    return socktype;
}

static SocketTypeInfo* make_socket_type_MassSpringSocket()
{
    SocketTypeInfo* socktype = make_standard_socket_type(SocketType::MassSpringSocket);
    socktype->cpp_type = &CPPType::get<std::shared_ptr<node_mass_spring::MassSpring>>();
    return socktype;
}

void register_socket(SocketTypeInfo* type_info)
{
    socket_registry[type_info->type_name] = std::unique_ptr<SocketTypeInfo>(type_info);
}

void register_sockets()
{
#define REGISTER_NODE(NAME) register_socket(make_socket_type_##NAME());

    MACRO_MAP(REGISTER_NODE, ALL_SOCKET_TYPES)
}

void register_all()
{
    register_cpp_types();
    register_nodes();
    register_sockets();
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
