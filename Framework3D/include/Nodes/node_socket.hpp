#pragma once

#include "GCore/Components.h"
#include "USTC_CG.h"
#include "Utils/json.hpp"
#include "all_socket_types.hpp"
#include "entt/meta/meta.hpp"
#include "id.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
#define TypeSizeEnum(Type, Size) Type##Size##Buffer
enum class SocketType : uint32_t { ALL_SOCKET_TYPES };
#undef TypeSizeEnum

const char* get_socket_name_string(SocketType socket);

enum class PinKind { Output, Input };

enum class NodeType {
    Blueprint,
    NodeGroup,
    GroupIn,
    GroupOut,
    Simple,
    Comment,
};

struct Node;
struct NodeLink;

struct GOperandComponent;

struct SocketTypeInfo {
    char type_name[64];
    entt::meta_type cpp_type;
    SocketType type;

    bool canConvertTo(SocketType other) const;

    std::string conversionNode(SocketType to_type) const
    {
        if (conversionTo.contains(to_type)) {
            return std::string("conv_") + get_socket_name_string(type) +
                   "_to_" + get_socket_name_string(to_type);
        }
        return {};
    }

    std::unordered_set<SocketType> conversionTo;
};

struct NodeSocket {
    char identifier[64];
    char ui_name[64];

    /** Only valid when #topology_cache_is_dirty is false. */
    std::vector<NodeLink*> directly_linked_links;
    std::vector<NodeSocket*> directly_linked_sockets;

    SocketID ID;
    Node* Node;

    SocketTypeInfo* type_info;
    PinKind in_out;

    // This is for simple data fields in the node graph.
    struct bNodeSocketValue {
        entt::meta_any value;
        entt::meta_any min;
        entt::meta_any max;
    } dataField;

    explicit NodeSocket(int id = 0)
        : identifier{},
          ui_name{},
          ID(id),
          Node(nullptr),
          type_info(nullptr),
          in_out(PinKind::Input)
    {
    }

    void Serialize(nlohmann::json& value);
    void DeserializeInfo(nlohmann::json& value);
    void DeserializeValue(const nlohmann::json& value);

    /** Utility to access the value of the socket. */
    template<typename T>
    T default_value_typed();
    template<typename T>
    const T& default_value_typed() const;

    friend bool operator==(const NodeSocket& lhs, const NodeSocket& rhs)
    {
        return strcmp(lhs.identifier, rhs.identifier) == 0 &&
               lhs.dataField.value == rhs.dataField.value;
    }

    friend bool operator!=(const NodeSocket& lhs, const NodeSocket& rhs)
    {
        return !(lhs == rhs);
    }

    ~NodeSocket()
    {
    }
};

template<typename T>
T NodeSocket::default_value_typed()
{
    if constexpr (std::is_same_v<T, std::string>) {
        return std::string(dataField.value.cast<std::string>().data());
    }
    return dataField.value.cast<T>();
}

template<typename T>
const T& NodeSocket::default_value_typed() const
{
    return dataField.value.cast<const T&>();
}
USTC_CG_NAMESPACE_CLOSE_SCOPE
