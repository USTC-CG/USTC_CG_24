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

void Node::Serialize(nlohmann::json& value)
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

USTC_CG_NAMESPACE_CLOSE_SCOPE
