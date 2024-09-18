#include "torch/torch.h"

#include "Nodes/all_socket_types.hpp"
#include "Nodes/node.hpp"
#include "Nodes/node_register.h"
#include "Nodes/node_socket.hpp"
#include "RCore/Backend.hpp"
#include "USTC_CG.h"
#include "Utils/Macro/map.h"
#include "boost/python.hpp"
#include "boost/python/numpy.hpp"
#include "entt/meta/resolve.hpp"
#include "rich_type_buffer.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
namespace node_mass_spring {
class MassSpring;
}
namespace node_sph_fluid {
class SPHBase;
}
namespace node_character_animation {
class Animator;
}

static std::map<std::string, std::unique_ptr<SocketTypeInfo>> socket_registry;

#define TYPE_NAME(CASE) \
    case SocketType::CASE: return "Socket" #CASE;

const char* get_socket_typename(SocketType socket)
{
    switch (socket) {
        MACRO_MAP(TYPE_NAME, ALL_SOCKET_TYPES)
        default: return "";
    }
}

#define TYPE_STRING(CASE) \
    case SocketType::CASE: return #CASE;

const char* get_socket_name_string(SocketType socket)
{
    switch (socket) {
        MACRO_MAP(TYPE_STRING, ALL_SOCKET_TYPES)
        default: return "";
    }
}
extern std::map<std::string, NodeTypeInfo*> conversion_node_registry;

SocketTypeInfo* make_standard_socket_type(SocketType socket)
{
    auto type_info = new SocketTypeInfo();
    type_info->type = socket;
    strcpy(type_info->type_name, get_socket_typename(socket));

    for (auto&& node_registry : conversion_node_registry) {
        if (node_registry.second->conversion_from == socket) {
            type_info->conversionTo.emplace(
                node_registry.second->conversion_to);
        }
    }

    return type_info;
}

#define MakeType(Type, Item, Size, Buffer)                             \
    static SocketTypeInfo* make_socket_type_##Type##Size##Buffer()     \
    {                                                                  \
        SocketTypeInfo* socket_type =                                  \
            make_standard_socket_type(SocketType::Type##Size##Buffer); \
        socket_type->cpp_type = entt::resolve<pxr::VtArray<Item>>();   \
        return socket_type;                                            \
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
    SocketTypeInfo* socket_type = make_standard_socket_type(SocketType::Float2);
    socket_type->cpp_type = entt::resolve<pxr::GfVec2f>();
    return socket_type;
};
static SocketTypeInfo* make_socket_type_Float3()
{
    SocketTypeInfo* socket_type = make_standard_socket_type(SocketType::Float3);
    socket_type->cpp_type = entt::resolve<pxr::GfVec3f>();
    return socket_type;
};
static SocketTypeInfo* make_socket_type_Float4()
{
    SocketTypeInfo* socket_type = make_standard_socket_type(SocketType::Float4);
    socket_type->cpp_type = entt::resolve<pxr::GfVec4f>();
    return socket_type;
};
static SocketTypeInfo* make_socket_type_Int2()
{
    SocketTypeInfo* socket_type = make_standard_socket_type(SocketType::Int2);
    socket_type->cpp_type = entt::resolve<pxr::GfVec2i>();
    return socket_type;
};
static SocketTypeInfo* make_socket_type_Int3()
{
    SocketTypeInfo* socket_type = make_standard_socket_type(SocketType::Int3);
    socket_type->cpp_type = entt::resolve<pxr::GfVec3i>();
    return socket_type;
};
static SocketTypeInfo* make_socket_type_Int4()
{
    SocketTypeInfo* socket_type = make_standard_socket_type(SocketType::Int4);
    socket_type->cpp_type = entt::resolve<pxr::GfVec4i>();
    return socket_type;
};
#undef MakeTypeBuffer

static SocketTypeInfo* make_socket_type_Int()
{
    SocketTypeInfo* socket_type = make_standard_socket_type(SocketType::Int);
    socket_type->cpp_type = entt::resolve<int>();

    return socket_type;
}

static SocketTypeInfo* make_socket_type_Float()
{
    SocketTypeInfo* socket_type = make_standard_socket_type(SocketType::Float);
    socket_type->cpp_type = entt::resolve<float>();
    return socket_type;
}

static SocketTypeInfo* make_socket_type_String()
{
    SocketTypeInfo* socket_type = make_standard_socket_type(SocketType::String);

    socket_type->cpp_type = entt::resolve<std::string>();
    return socket_type;
}

static SocketTypeInfo* make_socket_type_Any()
{
    SocketTypeInfo* socket_type = make_standard_socket_type(SocketType::Any);
    // socket_type->cpp_type = entt::resolve<entt::meta_any>();
    return socket_type;
}

static SocketTypeInfo* make_socket_type_Geometry()
{
    SocketTypeInfo* socket_type =
        make_standard_socket_type(SocketType::Geometry);
    socket_type->cpp_type = entt::resolve<Geometry>();
    return socket_type;
}

static SocketTypeInfo* make_socket_type_Lights()
{
    SocketTypeInfo* socket_type = make_standard_socket_type(SocketType::Lights);
    socket_type->cpp_type = entt::resolve<LightArray>();
    return socket_type;
}

static SocketTypeInfo* make_socket_type_Layer()
{
    SocketTypeInfo* socket_type = make_standard_socket_type(SocketType::Layer);
    socket_type->cpp_type = entt::resolve<pxr::UsdStageRefPtr>();
    return socket_type;
}

static SocketTypeInfo* make_socket_type_Camera()
{
    SocketTypeInfo* socket_type = make_standard_socket_type(SocketType::Camera);
    socket_type->cpp_type = entt::resolve<CameraArray>();
    return socket_type;
}

static SocketTypeInfo* make_socket_type_Meshes()
{
    SocketTypeInfo* socket_type = make_standard_socket_type(SocketType::Meshes);
    socket_type->cpp_type = entt::resolve<MeshArray>();
    return socket_type;
}

static SocketTypeInfo* make_socket_type_Texture()
{
    SocketTypeInfo* socket_type =
        make_standard_socket_type(SocketType::Texture);
    socket_type->cpp_type = entt::resolve<TextureHandle>();
    return socket_type;
}

static SocketTypeInfo* make_socket_type_Buffer()
{
    SocketTypeInfo* socket_type = make_standard_socket_type(SocketType::Buffer);
    socket_type->cpp_type = entt::resolve<BufferHandle>();
    return socket_type;
}

static SocketTypeInfo* make_socket_type_Materials()
{
    SocketTypeInfo* socket_type =
        make_standard_socket_type(SocketType::Materials);
    socket_type->cpp_type = entt::resolve<MaterialMap>();
    return socket_type;
}

static SocketTypeInfo* make_socket_type_PyObj()
{
    SocketTypeInfo* socket_type = make_standard_socket_type(SocketType::PyObj);
    socket_type->cpp_type = entt::resolve<boost::python::object>();
    return socket_type;
}

static SocketTypeInfo* make_socket_type_AccelStruct()
{
    SocketTypeInfo* socket_type =
        make_standard_socket_type(SocketType::AccelStruct);
    socket_type->cpp_type = entt::resolve<AccelStructHandle>();
    return socket_type;
}

static SocketTypeInfo* make_socket_type_SocketGroup()
{
    SocketTypeInfo* socket_type =
        make_standard_socket_type(SocketType::SocketGroup);
    socket_type->cpp_type = entt::resolve<SocketGroup>();
    return socket_type;
}

static SocketTypeInfo* make_socket_type_NumpyArray()
{
    SocketTypeInfo* socket_type =
        make_standard_socket_type(SocketType::NumpyArray);
    socket_type->cpp_type = entt::resolve<boost::python::numpy::ndarray>();
    return socket_type;
}

static SocketTypeInfo* make_socket_type_MassSpringSocket()
{
    SocketTypeInfo* socket_type =
        make_standard_socket_type(SocketType::MassSpringSocket);
    socket_type->cpp_type =
        entt::resolve<std::shared_ptr<node_mass_spring::MassSpring>>();
    return socket_type;
}

static SocketTypeInfo* make_socket_type_SPHFluidSocket()
{
    SocketTypeInfo* socket_type =
        make_standard_socket_type(SocketType::SPHFluidSocket);
    socket_type->cpp_type =
        entt::resolve<std::shared_ptr<node_sph_fluid::SPHBase>>();
    return socket_type;
}

static SocketTypeInfo* make_socket_type_AnimatorSocket()
{
    SocketTypeInfo* socket_type =
        make_standard_socket_type(SocketType::AnimatorSocket);
    socket_type->cpp_type =
        entt::resolve<std::shared_ptr<node_character_animation::Animator>>();
    return socket_type;
}

static SocketTypeInfo* make_socket_type_TorchTensor()
{
    SocketTypeInfo* socket_type =
        make_standard_socket_type(SocketType::TorchTensor);
    socket_type->cpp_type = entt::resolve<torch::Tensor>();
    return socket_type;
}

void register_socket(SocketTypeInfo* type_info)
{
    socket_registry[type_info->type_name] =
        std::unique_ptr<SocketTypeInfo>(type_info);
}

void register_sockets()
{
#define REGISTER_NODE(NAME) register_socket(make_socket_type_##NAME());

    MACRO_MAP(REGISTER_NODE, ALL_SOCKET_TYPES)
}
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

bool SocketTypeInfo::canConvertTo(SocketType other) const
{
    if (type == SocketType::Any) {
        return true;
    }
    if (other == SocketType::Any) {
        return true;
    }

    if (!conversionNode(other).empty()) {
        return true;
    }
    return type == other;
}

void NodeSocket::Serialize(nlohmann::json& value)
{
    auto& socket = value[std::to_string(ID.Get())];
    // Repeated storage. Simpler code for iteration.
    socket["ID"] = ID.Get();
    socket["id_name"] = type_info->type_name;
    socket["identifier"] = identifier;
    socket["ui_name"] = ui_name;
    socket["in_out"] = in_out;

    if (dataField.value) {
        switch (type_info->type) {
            case SocketType::Int:
                socket["value"] = default_value_typed<int>();
                break;
            case SocketType::Float:
                socket["value"] = default_value_typed<float>();
                break;
            case SocketType::String:
                socket["value"] = default_value_typed<std::string&>().c_str();
                break;
            default: break;
        }
    }
}

void NodeSocket::DeserializeInfo(nlohmann::json& socket_json)
{
    ID = socket_json["ID"].get<unsigned>();

    type_info =
        socketTypeFind(socket_json["id_name"].get<std::string>().c_str());
    in_out = socket_json["in_out"].get<PinKind>();
    strcpy(ui_name, socket_json["ui_name"].get<std::string>().c_str());
    strcpy(identifier, socket_json["identifier"].get<std::string>().c_str());
}

void NodeSocket::DeserializeValue(const nlohmann::json& value)
{
    if (dataField.value) {
        if (value.find("value") != value.end()) {
            switch (type_info->type) {
                case SocketType::Int:
                    default_value_typed<int&>() = value["value"];
                    break;
                case SocketType::Float:
                    default_value_typed<float&>() = value["value"];
                    break;
                case SocketType::String: {
                    std::string str = value["value"];
                    strcpy(
                        default_value_typed<std::string&>().data(),
                        str.c_str());
                } break;
                default: break;
            }
        }
    }
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
