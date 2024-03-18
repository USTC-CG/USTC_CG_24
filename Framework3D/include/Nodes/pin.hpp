#pragma once

#include "GCore/Components.h"
#include "USTC_CG.h"
#include "Utils/Functions/CPPType.hpp"
#include "Utils/json.hpp"
#include "id.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
#define TypeSizeEnum(Type, Size) Type##Size##Buffer
enum class SocketType : uint32_t {
    Geometry,
    Int,
    Float,
    String,
    TypeSizeEnum(Float, 1),
    TypeSizeEnum(Float, 2),
    TypeSizeEnum(Float, 3),
    TypeSizeEnum(Float, 4),
    TypeSizeEnum(Int, 1),
    TypeSizeEnum(Int, 2),
    TypeSizeEnum(Int, 3),
    TypeSizeEnum(Int, 4),
    // Bool,
    // Int,
    // String,
    // Object,
    // Function,
    // Delegate
};

#undef TypeSizeEnum

enum class PinKind { Output, Input };

enum class NodeType {
    Blueprint,
    Simple,
    Comment,
};

struct Node;
struct NodeLink;

struct GOperandComponent;

struct SocketTypeInfo {
    char type_name[64];
    const CPPType* cpp_type;
    SocketType type;
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

    void* default_value = nullptr;

    NodeSocket(int id = 0) : ID(id), Node(nullptr), in_out(PinKind::Input)
    {
    }

    void Serialize(nlohmann::json& value);
    void DeserializeInfo(nlohmann::json& value);
    void DeserializeValue(const nlohmann::json& value);

    /** Utility to access the value of the socket. */
    template<typename T>
    T* default_value_typed();
    template<typename T>
    const T* default_value_typed() const;

    ~NodeSocket()
    {
        free(default_value);
    }
};

template<typename T>
T* NodeSocket::default_value_typed()
{
    return static_cast<T*>(this->default_value);
}

template<typename T>
const T* NodeSocket::default_value_typed() const
{
    return static_cast<const T*>(this->default_value);
}

template<typename T>
struct bNodeSocketValue {
    T value;
    T min;
    T max;
};

using bNodeSocketValueInt = bNodeSocketValue<int>;
using bNodeSocketValueFloat = bNodeSocketValue<float>;

struct bNodeSocketValueString {
    std::string value;
};

// TODO: template this.
void* default_value_storage(NodeSocket* socket);
void* default_value_min(NodeSocket* socket);
void* default_value_max(NodeSocket* socket);

USTC_CG_NAMESPACE_CLOSE_SCOPE
