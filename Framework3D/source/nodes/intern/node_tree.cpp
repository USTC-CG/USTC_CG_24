#include "Nodes/node_tree.hpp"

#include <stack>

#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_socket.hpp"
#include "Utils/json.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
unsigned NodeTree::input_socket_id(NodeSocket* socket)
{
    return std::distance(
        input_sockets.begin(),
        std::find(input_sockets.begin(), input_sockets.end(), socket));
}

unsigned NodeTree::output_socket_id(NodeSocket* socket)
{
    return std::distance(
        output_sockets.begin(),
        std::find(output_sockets.begin(), output_sockets.end(), socket));
}

void NodeTree::clear()
{
    links.clear();
    sockets.clear();
    nodes.clear();
    input_sockets.clear();
    output_sockets.clear();
    toposort_right_to_left.clear();
    toposort_left_to_right.clear();
}

NodeSocket* NodeTree::find_pin(SocketID id) const
{
    if (!id)
        return nullptr;

    for (auto& socket : sockets) {
        if (socket->ID == id) {
            return socket.get();
        }
    }
    return nullptr;
}

NodeLink* NodeTree::find_link(LinkId id) const
{
    if (!id)
        return nullptr;

    for (auto& link : links) {
        if (link->ID == id) {
            return link.get();
        }
    }

    return nullptr;
}

bool NodeTree::is_pin_linked(SocketID id) const
{
    return !find_pin(id)->directly_linked_links.empty();
}

Node* NodeTree::add_node(const char* idname)
{
    auto node = std::make_unique<Node>(this, idname);
    auto bare = node.get();
    nodes.push_back(std::move(node));
    return bare;
}

unsigned NodeTree::UniqueID()
{
    while (used_ids.find(current_id) != used_ids.end()) {
        current_id++;
    }

    return current_id++;
}

NodeLink* NodeTree::add_link(
    Node* fromnode,
    NodeSocket* fromsock,
    Node* tonode,
    NodeSocket* tosock)
{
    SetDirty(true);

    if (fromsock->in_out == PinKind::Input) {
        std::swap(fromnode, tonode);
        std::swap(fromsock, tosock);
    }

    std::string node_name;
    node_name = fromsock->type_info->conversionNode(tosock->type_info->type);

    NodeLink* bare_ptr = nullptr;
    if (!node_name.empty()) {
        auto middle_node = add_node(node_name.c_str());
        assert(middle_node->get_inputs().size() == 1);
        assert(middle_node->get_outputs().size() == 1);

        auto middle_tosock = middle_node->get_inputs()[0];
        auto middle_fromsock = middle_node->get_outputs()[0];

        auto firstLink =
            add_link(fromnode, fromsock, middle_node, middle_tosock);

        auto nextLink = add_link(middle_node, middle_fromsock, tonode, tosock);
        assert(firstLink);
        assert(nextLink);
        firstLink->nextLink = nextLink;
        nextLink->fromLink = firstLink;
    }
    else {
        auto link =
            std::make_unique<NodeLink>(UniqueID(), fromsock->ID, tosock->ID);

        link->from_node = fromnode;
        link->from_sock = fromsock;
        link->to_node = tonode;
        link->to_sock = tosock;
        bare_ptr = link.get();
        links.push_back(std::move(link));
    }
    ensure_topology_cache();
    return bare_ptr;
}

void NodeTree::add_link(SocketID startPinId, SocketID endPinId)
{
    SetDirty(true);
    auto socket1 = find_pin(startPinId);
    auto socket2 = find_pin(endPinId);

    if (socket1 && socket2)
        add_link(socket1->Node, socket1, socket2->Node, socket2);
}

void NodeTree::remove_link(LinkId linkId)
{
    SetDirty(true);

    auto link = std::find_if(links.begin(), links.end(), [linkId](auto& link) {
        return link->ID == linkId;
    });
    if (link != links.end()) {
        if ((*link)->nextLink) {
            auto nextLinkId = (*link)->nextLink->ID;
            delete_node((*link)->to_node->ID);
            remove_link(nextLinkId);
        }

        links.erase(link);
    }
}

void NodeTree::delete_node(NodeId nodeId)
{
    auto id = std::find_if(nodes.begin(), nodes.end(), [nodeId](auto&& node) {
        return node->ID == nodeId;
    });
    if (id != nodes.end()) {
        for (auto& socket : (*id)->get_inputs()) {
            delete_socket(socket->ID);
        }

        for (auto& socket : (*id)->get_outputs()) {
            delete_socket(socket->ID);
        }

        nodes.erase(id);
    }
}

bool NodeTree::can_create_link(NodeSocket* a, NodeSocket* b)
{
    if (!a || !b || a == b || a->in_out == b->in_out || a->Node == b->Node)
        return false;

    auto in = a->in_out == PinKind::Input ? a : b;
    auto out = a->in_out == PinKind::Output ? a : b;

    if (!in->directly_linked_sockets.empty()) {
        return false;
    }
    if (can_create_direct_link(out, in)) {
        return true;
    }

    if (can_create_convert_link(out, in)) {
        return true;
    }

    return false;
}

bool NodeTree::can_create_direct_link(NodeSocket* socket1, NodeSocket* socket2)
{
    return socket1->type_info->type == socket2->type_info->type;
}

bool NodeTree::can_create_convert_link(NodeSocket* out, NodeSocket* in)
{
    return out->type_info->canConvertTo(in->type_info->type);
}

void NodeTree::delete_socket(SocketID socketId)
{
    auto id =
        std::find_if(sockets.begin(), sockets.end(), [socketId](auto&& socket) {
            return socket->ID == socketId;
        });

    if (id != sockets.end()) {
        sockets.erase(id);
    }
}

void NodeTree::update_directly_linked_links_and_sockets()
{
    for (auto&& node : nodes) {
        for (auto socket : node->get_inputs()) {
            socket->directly_linked_links.clear();
            socket->directly_linked_sockets.clear();
        }
        for (auto socket : node->get_outputs()) {
            socket->directly_linked_links.clear();
            socket->directly_linked_sockets.clear();
        }
        node->has_available_linked_inputs = false;
        node->has_available_linked_outputs = false;
    }
    for (auto&& link : links) {
        link->from_sock->directly_linked_links.push_back(link.get());
        link->from_sock->directly_linked_sockets.push_back(link->to_sock);
        link->to_sock->directly_linked_links.push_back(link.get());
        if (link) {
            link->from_node->has_available_linked_outputs = true;
            link->to_node->has_available_linked_inputs = true;
        }
    }

    for (NodeSocket* socket : input_sockets) {
        if (socket) {
            for (NodeLink* link : socket->directly_linked_links) {
                /* Do this after sorting the input links. */
                socket->directly_linked_sockets.push_back(link->from_sock);
            }
        }
    }
}

void NodeTree::update_socket_vectors_and_owner_node()
{
    input_sockets.clear();
    output_sockets.clear();
    for (auto&& socket : sockets) {
        if (socket->in_out == PinKind::Input) {
            input_sockets.push_back(socket.get());
        }
        else {
            output_sockets.push_back(socket.get());
        }
    }
}

enum class ToposortDirection {
    LeftToRight,
    RightToLeft,
};

struct ToposortNodeState {
    bool is_done = false;
    bool is_in_stack = false;
};

template<typename T>
using Vector = std::vector<T>;

static void toposort_from_start_node(
    const NodeTree& ntree,
    const ToposortDirection direction,
    Node& start_node,
    std::unordered_map<Node*, ToposortNodeState>& node_states,
    Vector<Node*>& r_sorted_nodes,
    bool& r_cycle_detected)
{
    struct Item {
        Node* node;
        int socket_index = 0;
        int link_index = 0;
        int implicit_link_index = 0;
    };

    std::stack<Item> nodes_to_check;
    nodes_to_check.push({ &start_node });
    node_states[&start_node].is_in_stack = true;
    while (!nodes_to_check.empty()) {
        Item& item = nodes_to_check.top();
        Node& node = *item.node;
        bool pushed_node = false;

        auto handle_linked_node = [&](Node& linked_node) {
            ToposortNodeState& linked_node_state = node_states[&linked_node];
            if (linked_node_state.is_done) {
                /* The linked node has already been visited. */
                return true;
            }
            if (linked_node_state.is_in_stack) {
                r_cycle_detected = true;
            }
            else {
                nodes_to_check.push({ &linked_node });
                linked_node_state.is_in_stack = true;
                pushed_node = true;
            }
            return false;
        };

        const auto& sockets = (direction == ToposortDirection::LeftToRight)
                                  ? node.get_inputs()
                                  : node.get_outputs();
        while (true) {
            if (item.socket_index == sockets.size()) {
                /* All sockets have already been visited. */
                break;
            }
            NodeSocket& socket = *sockets[item.socket_index];
            const auto& linked_links = socket.directly_linked_links;
            if (item.link_index == linked_links.size()) {
                /* All links connected to this socket have already been visited.
                 */
                item.socket_index++;
                item.link_index = 0;
                continue;
            }

            NodeSocket& linked_socket =
                *socket.directly_linked_sockets[item.link_index];
            Node& linked_node = *linked_socket.Node;
            if (handle_linked_node(linked_node)) {
                /* The linked node has already been visited. */
                item.link_index++;
                continue;
            }
            break;
        }

        if (!pushed_node) {
            ToposortNodeState& node_state = node_states[&node];
            node_state.is_done = true;
            node_state.is_in_stack = false;
            r_sorted_nodes.push_back(&node);
            nodes_to_check.pop();
        }
    }
}

static void update_toposort(
    const NodeTree& ntree,
    const ToposortDirection direction,
    Vector<Node*>& r_sorted_nodes,
    bool& r_cycle_detected)
{
    const NodeTree& tree_runtime = ntree;
    r_sorted_nodes.clear();
    r_sorted_nodes.reserve(tree_runtime.nodes.size());
    r_cycle_detected = false;

    std::unordered_map<Node*, ToposortNodeState> node_states(
        tree_runtime.nodes.size());
    for (auto&& node : tree_runtime.nodes) {
        if (!node_states.contains(node.get())) {
            node_states[node.get()] = ToposortNodeState{};
        }
    }
    for (auto&& node : tree_runtime.nodes) {
        if (node_states[node.get()].is_done) {
            /* Ignore nodes that are done already. */
            continue;
        }
        if ((direction == ToposortDirection::LeftToRight)
                ? node->has_available_linked_outputs
                : node->has_available_linked_inputs) {
            /* Ignore non-start nodes. */
            continue;
        }
        toposort_from_start_node(
            ntree,
            direction,
            *node,
            node_states,
            r_sorted_nodes,
            r_cycle_detected);
    }

    if (r_sorted_nodes.size() < tree_runtime.nodes.size()) {
        r_cycle_detected = true;
        for (auto&& node : tree_runtime.nodes) {
            if (node_states[node.get()].is_done) {
                /* Ignore nodes that are done already. */
                continue;
            }
            /* Start toposort at this node which is somewhere in the middle
            of a
             * loop. */
            toposort_from_start_node(
                ntree,
                direction,
                *node,
                node_states,
                r_sorted_nodes,
                r_cycle_detected);
        }
    }

    assert(tree_runtime.nodes.size() == r_sorted_nodes.size());
}

void NodeTree::ensure_topology_cache()
{
    update_socket_vectors_and_owner_node();

    update_directly_linked_links_and_sockets();

    update_toposort(
        *this,
        ToposortDirection::LeftToRight,
        toposort_left_to_right,
        has_available_link_cycle);
    update_toposort(
        *this,
        ToposortDirection::RightToLeft,
        toposort_right_to_left,
        has_available_link_cycle);
}

void NodeTree::refresh_node_socket(
    Node* node,
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
        (*old_socket)->Node = node;
        new_socket = *old_socket;
        new_socket->type_info->type = socket_declaration.type;
        socket_declaration.update_default_value(new_socket);
    }
    else {
        new_socket = socket_declaration.build(this, node);
        sockets.emplace_back(new_socket);
    }
    new_sockets.push_back(new_socket);
}

void NodeTree::build_sockets_from_type_info(Node* node)
{
}

void NodeTree::try_fill_value_by_deserialization(Node* node)
{
}

std::string NodeTree::Serialize()
{
    nlohmann::json value;

    auto& node_info = value["nodes_info"];
    for (auto&& node : nodes) {
        node->serialize(node_info);
    }

    auto& links_info = value["links_info"];
    for (auto&& link : links) {
        link->Serialize(links_info);
    }

    auto& sockets_info = value["sockets_info"];
    for (auto&& socket : sockets) {
        socket->Serialize(sockets_info);
    }

    std::ostringstream s;
    s << value;
    return s.str();
}

void NodeTree::Deserialize(const std::string& str)
{
    nlohmann::json value;
    std::istringstream in(str);
    in >> value;
    clear();

    // To avoid reuse of ID, push up the ID in the beginning

    for (auto&& socket_json : value["sockets_info"]) {
        used_ids.emplace(socket_json["ID"]);
    }

    for (auto&& node_json : value["nodes_info"]) {
        used_ids.emplace(node_json["ID"]);
    }

    for (auto&& link_json : value["links_info"]) {
        used_ids.emplace(link_json["ID"]);
    }

    for (auto&& socket_json : value["sockets_info"]) {
        auto socket = std::make_unique<NodeSocket>();
        socket->DeserializeInfo(socket_json);
        sockets.push_back(std::move(socket));
    }

    for (auto&& node_json : value["nodes_info"]) {
        auto id = node_json["ID"].get<unsigned>();
        auto id_name = node_json["id_name"].get<std::string>();

        auto node = std::make_unique<Node>(this, id, id_name.c_str());

        if (!node->valid())
            continue;

        node->deserialize(node_json);
        nodes.push_back(std::move(node));
    }

    // Get the saved value in the sockets
    for (auto&& node_socket : sockets) {
        const auto& socket_value =
            value["sockets_info"][std::to_string(node_socket->ID.Get())];
        node_socket->DeserializeValue(socket_value);
    }

    for (auto&& link_json : value["links_info"]) {
        add_link(
            link_json["StartPinID"].get<unsigned>(),
            link_json["EndPinID"].get<unsigned>());
    }

    ensure_topology_cache();
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
