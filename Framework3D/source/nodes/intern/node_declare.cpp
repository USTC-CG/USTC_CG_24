#include "Nodes/node_declare.hpp"

#include "Nodes/node.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
NodeSocket* decl::Geometry::build(NodeTree* ntree, Node* node) const
{
    NodeSocket* socket = nodeAddSocket(
        ntree,
        node,
        this->in_out,
        get_socket_typename(SocketType::Geometry),
        this->identifier.c_str(),
        this->name.c_str());
    return socket;
}

NodeSocket* decl::Int::build(NodeTree* ntree, Node* node) const
{
    NodeSocket* socket = nodeAddSocket(
        ntree,
        node,
        this->in_out,
        get_socket_typename(SocketType::Int),
        this->identifier.c_str(),
        this->name.c_str());
    update_default_value(socket);

    return socket;
}

void decl::Int::update_default_value(NodeSocket* socket) const
{
    if (!socket->default_value) {
        // TODO: When shall we destroy these?
        auto default_value = new bNodeSocketValueInt();
        default_value->min = soft_min;
        default_value->max = soft_max;
        default_value->value =
            std::max(std::min(default_value_, soft_max), soft_min);
        socket->default_value = default_value;
    }
}

NodeSocket* decl::Float::build(NodeTree* ntree, Node* node) const
{
    NodeSocket* socket = nodeAddSocket(
        ntree,
        node,
        this->in_out,
        get_socket_typename(SocketType::Float),
        this->identifier.c_str(),
        this->name.c_str());
    update_default_value(socket);

    return socket;
}

#define BufferBuild(Type, Size)                                              \
    NodeSocket* decl::Type##Size##Buffer::build(NodeTree* ntree, Node* node) \
        const                                                                \
    {                                                                        \
        NodeSocket* socket = nodeAddSocket(                                  \
            ntree,                                                           \
            node,                                                            \
            this->in_out,                                                    \
            get_socket_typename(SocketType::Type##Size##Buffer),             \
            this->identifier.c_str(),                                        \
            this->name.c_str());                                             \
        return socket;                                                       \
    }

BufferBuild(Float, 1);
BufferBuild(Float, 2);
BufferBuild(Float, 3);
BufferBuild(Float, 4);
BufferBuild(Int, 1);
BufferBuild(Int, 2);
BufferBuild(Int, 3);
BufferBuild(Int, 4);

void decl::Float::update_default_value(NodeSocket* socket) const
{
    if (!socket->default_value) {
        // TODO: When shall we destroy these?
        auto default_value = new bNodeSocketValueFloat();
        default_value->min = soft_min;
        default_value->max = soft_max;
        default_value->value =
            std::max(std::min(default_value_, soft_max), soft_min);
        socket->default_value = default_value;
    }
}

NodeSocket* decl::String::build(NodeTree* ntree, Node* node) const
{
    NodeSocket* socket = nodeAddSocket(
        ntree,
        node,
        this->in_out,
        get_socket_typename(SocketType::String),
        this->identifier.c_str(),
        this->name.c_str());
    update_default_value(socket);

    return socket;
}

void decl::String::update_default_value(NodeSocket* socket) const
{
    if (!socket->default_value) {
        // TODO: When shall we destroy these?
        auto default_value = new bNodeSocketValueString();
        default_value->value.resize(256);
        memcpy(
            default_value->value.data(),
            default_value_.data(),
            default_value_.size());
        socket->default_value = default_value;
    }
}

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
