#pragma once

#include "Utils/json.hpp"
#include "node_declare.hpp"
#include "node_exec.hpp"
#include "node_socket.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
class Operator;

struct NodeLink {
    LinkId ID;

    Node* from_node = nullptr;
    Node* to_node = nullptr;
    NodeSocket* from_sock = nullptr;
    NodeSocket* to_sock = nullptr;

    SocketID StartPinID;
    SocketID EndPinID;

    // Used for invisible nodes when conversion
    NodeLink* fromLink = nullptr;
    NodeLink* nextLink = nullptr;

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

enum class NodeTypeOfGrpah {
    Geometry,
    Function,
    Render,
    Composition,
    Conversion
};

// There can be many instances of nodes, while each of them has a type. The
// templates should be declared statically. It contains the information of the
// type of input and output.
struct NodeTypeInfo {
    char id_name[64];
    char ui_name[64];

    NodeTypeOfGrpah node_type_of_grpah;

    float color[4];
    NodeDeclareFunction declare;
    ExecFunction node_execute;
    bool ALWAYS_REQUIRED = false;
    bool INVISIBLE = false;

    std::unique_ptr<NodeDeclaration> static_declaration;

    SocketType conversion_from;
    SocketType conversion_to;
};

struct Node {
    NodeId ID;
    std::string ui_name;

    float Color[4];
    NodeType Type;

    unsigned Size[2];

    NodeTypeInfo* typeinfo;

    bool REQUIRED = false;
    bool MISSING_INPUT = false;
    std::string execution_failed = {};

    std::function<void()> override_left_pane_info = nullptr;

    bool has_available_linked_inputs = false;
    bool has_available_linked_outputs = false;

    explicit Node(NodeTree* node_tree, int id, const char* idname);

    Node(NodeTree* node_tree, const char* idname);

    void serialize(nlohmann::json& value);

    void add_socket(NodeSocket* socket, PinKind in_out)
    {
        if (in_out == PinKind::Input) {
            inputs.push_back(socket);
        }
        else {
            outputs.push_back(socket);
        }
    }

    NodeSocket* find_socket(const char* identifier, PinKind in_out) const;
    size_t find_socket_id(const char* identifier, PinKind in_out) const;

    [[nodiscard]] const std::vector<NodeSocket*>& get_inputs() const
    {
        return inputs;
    }

    [[nodiscard]] const std::vector<NodeSocket*>& get_outputs() const
    {
        return outputs;
    }

    [[nodiscard]] std::vector<NodeSocket*>& get_inputs()
    {
        return inputs;
    }

    [[nodiscard]] std::vector<NodeSocket*>& get_outputs()
    {
        return outputs;
    }

    bool valid()
    {
        return valid_;
    }

   private:
    bool pre_init_node(const char* idname);

    std::vector<NodeSocket*> inputs;
    std::vector<NodeSocket*> outputs;

    NodeTree* tree_;
    bool valid_ = false;
};

void nodeRegisterType(NodeTypeInfo* type_info);

const std::map<std::string, NodeTypeInfo*>& get_geo_node_registry();
const std::map<std::string, NodeTypeInfo*>& get_render_node_registry();
const std::map<std::string, NodeTypeInfo*>& get_func_node_registry();
const std::map<std::string, NodeTypeInfo*>& get_composition_node_registry();

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
