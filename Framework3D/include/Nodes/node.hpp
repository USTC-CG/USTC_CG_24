#pragma once

#include "Utils/json.hpp"
#include "node_declare.hpp"
#include "node_exec.hpp"
#include "pin.hpp"


USTC_CG_NAMESPACE_OPEN_SCOPE
class Operator;

struct NodeLink {
    LinkId ID;

    Node *fromnode, *tonode;
    NodeSocket *fromsock, *tosock;

    SocketID StartPinID;
    SocketID EndPinID;

    NodeLink(LinkId id, SocketID startPinId, SocketID endPinId)
        : ID(id),
          StartPinID(startPinId),
          EndPinID(endPinId)
    {
    }

    void Serialize(nlohmann::json& value);
};


using ExecFunction = void (*)(ExeParams params);
using NodeDeclareFunction = void (*)(NodeDeclarationBuilder& builder);

// There can be many instances of nodes, while each of them has a type. The
// templates should be declared statically. It contains the information of the
// type of input and output.
struct NodeTypeInfo {
    char id_name[64];
    char ui_name[64];

    float color[4];
    NodeDeclareFunction declare;
    ExecFunction node_execute;
    bool ALWAYS_REQUIRED = false;

    std::unique_ptr<NodeDeclaration> static_declaration;
};

struct Node {
    NodeId ID;
    std::string ui_name;
    std::vector<NodeSocket*> inputs;
    std::vector<NodeSocket*> outputs;

    float Color[4];
    NodeType Type;
    float Size[2];

    Operator* operator_;

    NodeTypeInfo* typeinfo;

    bool REQUIRED = false;
    bool MISSING_INPUT = false;
    std::string execution_failed = {};

    std::function<void()> override_left_pane_info = nullptr;

    bool has_available_linked_inputs = false;
    bool has_available_linked_outputs = false;

    Node(int id = 0, const char* name = "Unknown")
        : ID(id),
          ui_name(name),
          Type(NodeType::Blueprint)
    {
        Size[0] = 0;
        Size[1] = 0;
    }

    void Serialize(nlohmann::json& value);
};

void nodeRegisterType(NodeTypeInfo* type_info);

const std::map<std::string, NodeTypeInfo*>& get_node_registry();

NodeSocket* nodeAddSocket(
    NodeTree* ntree,
    Node* node,
    PinKind pin_kind,
    const char* id_name,
    const char* identifier,
    const char* name);

NodeTypeInfo* nodeTypeFind(const char* idname);
SocketTypeInfo* socketTypeFind(const char* idname);

const char* get_socket_typename(SocketType socket);
USTC_CG_NAMESPACE_CLOSE_SCOPE
