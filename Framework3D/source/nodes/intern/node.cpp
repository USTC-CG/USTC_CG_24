//  A general file containing functions related to nodes.

#include "Nodes/node.hpp"

#include "Nodes/node_tree.hpp"
#include "Nodes/pin.hpp"
#include "USTC_CG.h"
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
    auto& link = value[std::to_string(ID.Get())];
    link["ID"] = ID.Get();
    link["StartPinID"] = StartPinID.Get();
    link["EndPinID"] = EndPinID.Get();
}

void Node::Serialize(nlohmann::json& value)
{
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

void NodeSocket::Serialize(nlohmann::json& value)
{
    auto& socket = value[std::to_string(ID.Get())];
    // Repeated storage. Simpler code for iteration.
    socket["ID"] = ID.Get();
    socket["id_name"] = type_info->type_name;
    socket["identifier"] = identifier;
    socket["ui_name"] = ui_name;
    socket["in_out"] = in_out;

    if (default_value) {
        switch (type_info->type) {
            case SocketType::Int:
                socket["value"] = default_value_typed<bNodeSocketValueInt>()->value;
                break;
            case SocketType::Float:
                socket["value"] = default_value_typed<bNodeSocketValueFloat>()->value;
                break;
            case SocketType::String:
                socket["value"] = default_value_typed<bNodeSocketValueString>()->value.c_str();
                break;
            default: break;
        }
    }
}

void NodeSocket::DeserializeInfo(nlohmann::json& socket_json)
{
    ID = socket_json["ID"].get<unsigned>();

    type_info = socketTypeFind(socket_json["id_name"].get<std::string>().c_str());
    in_out = socket_json["in_out"].get<PinKind>();
    strcpy(ui_name, socket_json["ui_name"].get<std::string>().c_str());
    strcpy(identifier, socket_json["identifier"].get<std::string>().c_str());
}

void NodeSocket::DeserializeValue(const nlohmann::json& value)
{
    if (default_value) {
        if (value.find("value") != value.end()) {
            switch (type_info->type) {
                case SocketType::Int:
                    default_value_typed<bNodeSocketValueInt>()->value = value["value"];
                    break;
                case SocketType::Float:
                    default_value_typed<bNodeSocketValueFloat>()->value = value["value"];
                    break;
                case SocketType::String: {
                    std::string str = value["value"];
                    strcpy(
                        (default_value_typed<bNodeSocketValueString>()->value).data(), str.c_str());
                } break;
                default: break;
            }
        }
    }
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
