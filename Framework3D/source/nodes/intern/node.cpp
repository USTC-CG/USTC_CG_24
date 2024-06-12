//  A general file containing functions related to nodes.

#include "Nodes/node.hpp"

#include "Nodes/node_socket.hpp"
#include "Nodes/node_tree.hpp"
#include "USTC_CG.h"
#include "entt/meta/resolve.hpp"
USTC_CG_NAMESPACE_OPEN_SCOPE
NodeSocket* nodeAddSocket(
    NodeTree* ntree,
    Node* node,
    PinKind in_out,
    const char* id_name,
    const char* identifier,
    const char* name)
{
    auto socket_type = socketTypeFind(id_name);
    auto socket = new NodeSocket(ntree->UniqueID());

    socket->type_info = socket_type;
    strcpy(socket->identifier, identifier);
    strcpy(socket->ui_name, name);
    socket->in_out = in_out;
    socket->Node = node;
    return socket;
}

void NodeLink::Serialize(nlohmann::json& value)
{
    if (!fromLink) {
        auto& link = value[std::to_string(ID.Get())];
        link["ID"] = ID.Get();

        auto startPin = StartPinID.Get();
        auto endPin = EndPinID.Get();

        link["StartPinID"] = startPin;
        if (nextLink) {
            endPin = nextLink->EndPinID.Get();
        }
        link["EndPinID"] = endPin;
    }
}

Node::Node(NodeTree* node_tree, int id, const char* idname)
    : ID(id),
      ui_name("Unknown"),
      Type(NodeType::Blueprint),
      tree_(node_tree)
{
    valid_ = pre_init_node(idname);
}

Node::Node(NodeTree* node_tree, const char* idname)
    : ui_name("Unknown"),
      Type(NodeType::Blueprint),
      tree_(node_tree)
{
    ID = tree_->UniqueID();
    valid_ = pre_init_node(idname);
    refresh_node();
}

void Node::serialize(nlohmann::json& value)
{
    if (!typeinfo->INVISIBLE) {
        auto& node = value[std::to_string(ID.Get())];
        node["ID"] = ID.Get();
        node["id_name"] = typeinfo->id_name;
        auto& input_socket_json = node["inputs"];
        auto& output_socket_json = node["outputs"];

        for (int i = 0; i < inputs.size(); ++i) {
            input_socket_json[std::to_string(i)] = inputs[i]->ID.Get();
        }

        for (int i = 0; i < outputs.size(); ++i) {
            output_socket_json[std::to_string(i)] = outputs[i]->ID.Get();
        }
    }
}

void Node::register_socket_to_node(NodeSocket* socket, PinKind in_out)
{
    if (in_out == PinKind::Input) {
        inputs.push_back(socket);
    }
    else {
        outputs.push_back(socket);
    }
}

NodeSocket* Node::find_socket(const char* identifier, PinKind in_out) const
{
    const std::vector<NodeSocket*>* socket_group;

    if (in_out == PinKind::Input) {
        socket_group = &inputs;
    }
    else {
        socket_group = &outputs;
    }

    const auto id = find_socket_id(identifier, in_out);
    return (*socket_group)[id];
}

size_t Node::find_socket_id(const char* identifier, PinKind in_out) const
{
    int counter = 0;

    const std::vector<NodeSocket*>* socket_group;

    if (in_out == PinKind::Input) {
        socket_group = &inputs;
    }
    else {
        socket_group = &outputs;
    }

    for (NodeSocket* socket : *socket_group) {
        if (std::string(socket->identifier) == identifier) {
            return counter;
        }
        counter++;
    }
    assert(false);
    return -1;
}

void Node::generate_socket_group_based_on_declaration(
    const SocketDeclaration& socket_declaration,
    const std::vector<NodeSocket*>& old_sockets,
    std::vector<NodeSocket*>& new_sockets)
{
    // TODO: This is a badly implemented zone. Refactor this.
    NodeSocket* new_socket;
    auto old_socket = std::find_if(
        old_sockets.begin(),
        old_sockets.end(),
        [&socket_declaration](NodeSocket* socket) {
            return std::string(socket->identifier) ==
                       socket_declaration.identifier &&
                   socket->in_out == socket_declaration.in_out &&
                   socket->type_info->type == socket_declaration.type;
        });
    if (old_socket != old_sockets.end()) {
        (*old_socket)->Node = this;
        new_socket = *old_socket;
        new_socket->type_info->type = socket_declaration.type;
        socket_declaration.update_default_value(new_socket);
    }
    else {
        new_socket = socket_declaration.build(tree_, this);
        tree_->sockets.emplace_back(new_socket);
    }
    new_sockets.push_back(new_socket);
}

void Node::remove_socket(NodeSocket* socket, PinKind kind)
{
    switch (kind) {
        case PinKind::Output:
            if (std::find(outputs.begin(), outputs.end(), socket) ==
                outputs.end()) {
                // If the sockets is not
                auto out_dated_socket = std::find_if(
                    tree_->sockets.begin(),
                    tree_->sockets.end(),
                    [socket](auto&& ptr) { return socket == ptr.get(); });
                tree_->sockets.erase(out_dated_socket);
            }
            break;
        case PinKind::Input:
            if (std::find(inputs.begin(), inputs.end(), socket) ==
                inputs.end()) {
                // If the sockets is not
                auto out_dated_socket = std::find_if(
                    tree_->sockets.begin(),
                    tree_->sockets.end(),
                    [socket](auto&& ptr) { return socket == ptr.get(); });
                tree_->sockets.erase(out_dated_socket);
            }
            break;
        default: break;
    }
}

void Node::out_date_sockets(
    const std::vector<NodeSocket*>& olds,
    PinKind pin_kind)
{
    for (auto old : olds) {
        remove_socket(old, pin_kind);
    }
}

void Node::refresh_node()
{
    auto ntype = typeinfo;

    assert(ntype->static_declaration);
    auto& node_decl = *ntype->static_declaration;

    auto& old_inputs = get_inputs();
    auto& old_outputs = get_outputs();
    std::vector<NodeSocket*> new_inputs;
    std::vector<NodeSocket*> new_outputs;

    for (const ItemDeclarationPtr& item_decl : node_decl.items) {
        if (auto socket_decl =
                dynamic_cast<const SocketDeclaration*>(item_decl.get())) {
            if (socket_decl->in_out == PinKind::Input) {
                generate_socket_group_based_on_declaration(
                    *socket_decl, old_inputs, new_inputs);
            }
            else {
                generate_socket_group_based_on_declaration(
                    *socket_decl, old_outputs, new_outputs);
            }
        }
        // TODO: Panels
        // else if (
        //     const PanelDeclaration* panel_decl =
        //         dynamic_cast<const PanelDeclaration*>(item_decl.get())) {
        //     refresh_node_panel(*panel_decl, old_panels, *new_panel);
        //     ++new_panel;
        // }
    }
    inputs = new_inputs;
    outputs = new_outputs;

    out_date_sockets(old_inputs, PinKind::Input);
    out_date_sockets(old_outputs, PinKind::Output);
}

void Node::deserialize(const nlohmann::json& node_json)
{
    for (auto&& input_id : node_json["inputs"]) {
        assert(tree_->find_pin(input_id.get<unsigned>()));
        register_socket_to_node(
            tree_->find_pin(input_id.get<unsigned>()), PinKind::Input);
    }

    for (auto&& output_id : node_json["outputs"]) {
        register_socket_to_node(
            tree_->find_pin(output_id.get<unsigned>()), PinKind::Output);
    }

    refresh_node();
}

bool Node::pre_init_node(const char* idname)
{
    typeinfo = nodeTypeFind(idname);
    if (!typeinfo) {
        assert(false);
        return false;
    }
    ui_name = typeinfo->ui_name;
    memcpy(Color, typeinfo->color, sizeof(float) * 4);

    return true;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
