#include "Nodes/node_declare.hpp"

#include "Nodes/all_socket_types.hpp"
#include "Nodes/node.hpp"
#include "Nodes/socket_types/render_socket_types.hpp"
#include "Nodes/socket_types/stage_socket_types.hpp"
#include "Nodes/socket_types/geo_socket_types.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE

void decl::Int::update_default_value(NodeSocket* socket) const
{
    if (!socket->default_value) {
        // TODO: When shall we destroy these?
        auto default_value = new bNodeSocketValueInt();
        default_value->min = soft_min;
        default_value->max = soft_max;
        default_value->value = std::max(std::min(default_value_, soft_max), soft_min);
        socket->default_value = default_value;
    }
}

void decl::Float::update_default_value(NodeSocket* socket) const
{
    if (!socket->default_value) {
        // TODO: When shall we destroy these?
        auto default_value = new bNodeSocketValueFloat();
        default_value->min = soft_min;
        default_value->max = soft_max;
        default_value->value = std::max(std::min(default_value_, soft_max), soft_min);
        socket->default_value = default_value;
    }
}

void decl::String::update_default_value(NodeSocket* socket) const
{
    if (!socket->default_value) {
        // TODO: When shall we destroy these?
        auto default_value = new bNodeSocketValueString();
        default_value->value.resize(256);
        memcpy(default_value->value.data(), default_value_.data(), default_value_.size());
        socket->default_value = default_value;
    }
}

#define BUILD_TYPE(NAME)                                             \
    NodeSocket* decl::NAME::build(NodeTree* ntree, Node* node) const \
    {                                                                \
        NodeSocket* socket = nodeAddSocket(                          \
            ntree,                                                   \
            node,                                                    \
            this->in_out,                                            \
            get_socket_typename(SocketType::NAME),                   \
            this->identifier.c_str(),                                \
            this->name.c_str());                                     \
        update_default_value(socket);                                \
                                                                     \
        return socket;                                               \
    }

MACRO_MAP(BUILD_TYPE, ALL_SOCKET_TYPES)

NodeDeclarationBuilder::NodeDeclarationBuilder(
    NodeDeclaration& declaration,
    const NodeTree* ntree,
    const Node* node)
    : declaration_(declaration),
      ntree_(ntree),
      node_(node)
{
}

void NodeDeclarationBuilder::finalize()
{
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
