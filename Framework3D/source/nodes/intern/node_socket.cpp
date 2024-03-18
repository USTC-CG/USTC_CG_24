#include "Nodes/pin.hpp"
#include "USTC_CG.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

void* default_value_storage(NodeSocket* socket)
{
    switch (socket->type_info->type) {
        case SocketType::Int:
            return &socket->default_value_typed<bNodeSocketValueInt>()->value;
        case SocketType::Float:
            return &socket->default_value_typed<bNodeSocketValueFloat>()->value;
        case SocketType::String:
            return &socket->default_value_typed<bNodeSocketValueString>()->value;
        default: return nullptr;
    }
}

void* default_value_min(NodeSocket* socket)
{
    switch (socket->type_info->type) {
        case SocketType::Int:
            return &socket->default_value_typed<bNodeSocketValueInt>()->min;
        case SocketType::Float:
            return &socket->default_value_typed<bNodeSocketValueFloat>()->min;
        default: return nullptr;
    }
}

void* default_value_max(NodeSocket* socket)
{
    switch (socket->type_info->type) {
        case SocketType::Int:
            return &socket->default_value_typed<bNodeSocketValueInt>()->max;
        case SocketType::Float:
            return &socket->default_value_typed<bNodeSocketValueFloat>()->max;
        default: return nullptr;
    }
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
