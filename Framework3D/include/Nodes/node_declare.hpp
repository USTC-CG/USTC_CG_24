#pragma once

#include "USTC_CG.h"
#include "pin.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
class NodeTree;
class NodeDeclarationBuilder;

/* Socket or panel declaration. */
class ItemDeclaration {
   public:
    virtual ~ItemDeclaration() = default;
};

using ItemDeclarationPtr = std::unique_ptr<ItemDeclaration>;

class SocketDeclaration : public ItemDeclaration {
   public:
    PinKind in_out;
    SocketType type;
    std::string name;
    std::string identifier;

    virtual NodeSocket* build(NodeTree* ntree, Node* node) const = 0;

    virtual void update_default_value(NodeSocket* socket) const
    {
    }
};

/**
 * Describes a panel containing sockets or other panels.
 */
class PanelDeclaration : public ItemDeclaration { };

class BaseSocketDeclarationBuilder {
    int index_ = -1;

    NodeDeclarationBuilder* node_decl_builder_ = nullptr;
    friend class NodeDeclarationBuilder;
};

template<typename SocketDecl>
class SocketDeclarationBuilder : public BaseSocketDeclarationBuilder {
   protected:
    using Self = typename SocketDecl::Builder;
    static_assert(std::is_base_of_v<SocketDeclaration, SocketDecl>);
    SocketDecl* decl_;

    friend class NodeDeclarationBuilder;
};
#include "socket_types.hpp"

class NodeDeclaration {
   public:
    /* Combined list of socket and panel declarations.
     * This determines order of sockets in the UI and panel content. */
    std::vector<ItemDeclarationPtr> items;
    /* Note: inputs and outputs pointers are owned by the items list. */
    std::vector<SocketDeclaration*> inputs;
    std::vector<SocketDeclaration*> outputs;
};

class NodeDeclarationBuilder {
   private:
    NodeDeclaration& declaration_;
    const NodeTree* ntree_ = nullptr;
    const Node* node_ = nullptr;

   public:
    NodeDeclarationBuilder(
        NodeDeclaration& declaration,
        const NodeTree* ntree = nullptr,
        const Node* node = nullptr);

    template<typename DeclType>
    typename DeclType::Builder& add_input(
        const char* name,
        const char* identifier = "");

    template<typename DeclType>
    typename DeclType::Builder& add_output(
        const char* name,
        const char* identifier = "");
    void finalize();

   private:
    /* Note: in_out can be a combination of SOCK_IN and SOCK_OUT.
     * The generated socket declarations only have a single flag set. */
    template<typename DeclType>
    typename DeclType::Builder& add_socket(
        const char* name,
        const char* identifier_in,
        const char* identifier_out,
        PinKind in_out);

   private:
    std::vector<std::unique_ptr<BaseSocketDeclarationBuilder>> socket_builders_;
};

template<typename DeclType>
typename DeclType::Builder& NodeDeclarationBuilder::add_input(
    const char* name,
    const char* identifier)
{
    return add_socket<DeclType>(name, identifier, "", PinKind::Input);
}

template<typename DeclType>
typename DeclType::Builder& NodeDeclarationBuilder::add_output(
    const char* name,
    const char* identifier)
{
    return add_socket<DeclType>(name, "", identifier, PinKind::Output);
}

template<typename DeclType>
typename DeclType::Builder& NodeDeclarationBuilder::add_socket(
    const char* name,
    const char* identifier_in,
    const char* identifier_out,
    PinKind in_out)
{
    using Builder = typename DeclType::Builder;

    std::unique_ptr<Builder> socket_decl_builder = std::make_unique<Builder>();

    socket_decl_builder->node_decl_builder_ = this;

    std::unique_ptr<DeclType> socket_decl = std::make_unique<DeclType>();
    socket_decl_builder->decl_ = &*socket_decl;
    socket_decl->name = name;
    socket_decl->in_out = in_out;
    socket_decl_builder->index_ = declaration_.inputs.size();

    if (in_out == PinKind::Input) {
        socket_decl->identifier = std::string(identifier_in);
        if (socket_decl->identifier.empty()) {
            socket_decl->identifier = name;
        }

        // Make sure there are no sockets in a same node with the same
        // identifier
        assert(
            (std::find_if(
                 declaration_.inputs.begin(),
                 declaration_.inputs.end(),
                 [&](SocketDeclaration* socket) {
                     return socket->identifier == socket_decl->identifier;
                 }) == declaration_.inputs.end()));
        declaration_.inputs.push_back(socket_decl.get());
    }
    else {
        socket_decl->identifier = std::string(identifier_out);

        if (socket_decl->identifier.empty()) {
            socket_decl->identifier = name;
        }

        assert(
            std::find_if(
                declaration_.outputs.begin(),
                declaration_.outputs.end(),
                [&](SocketDeclaration* socket) {
                    return socket->identifier == socket_decl->identifier;
                }) == declaration_.outputs.end());

        declaration_.outputs.push_back(socket_decl.get());
    }
    declaration_.items.push_back(std::move(socket_decl));

    Builder& socket_decl_builder_ref = *socket_decl_builder;
    socket_builders_.push_back(std::move(socket_decl_builder));

    return socket_decl_builder_ref;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
