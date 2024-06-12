#pragma once

#include <memory>
#include <vector>

#include "USTC_CG.h"
#include "node.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
class NodeDeclaration;
struct NodeTypeInfo;
class SocketDeclaration;
struct Node;
struct NodeSocket;

class NodeTree {
   public:
    NodeTree() : has_available_link_cycle(false)
    {
        links.reserve(32);
        sockets.reserve(32);
        nodes.reserve(32);
        input_sockets.reserve(32);
        output_sockets.reserve(32);
        toposort_right_to_left.reserve(32);
        toposort_left_to_right.reserve(32);
    }

    std::vector<std::unique_ptr<NodeLink>> links;
    std::vector<std::unique_ptr<NodeSocket>> sockets;
    std::vector<std::unique_ptr<Node>> nodes;
    bool has_available_link_cycle;

    unsigned input_socket_id(NodeSocket* socket);
    unsigned output_socket_id(NodeSocket* socket);

    std::vector<NodeSocket*> input_sockets;
    std::vector<NodeSocket*> output_sockets;

    [[nodiscard]] const std::vector<Node*>& get_toposort_right_to_left() const
    {
        return toposort_right_to_left;
    }

    [[nodiscard]] const std::vector<Node*>& get_toposort_left_to_right() const
    {
        return toposort_left_to_right;
    }

    // The left to right topology is holding the memory
    std::vector<Node*> toposort_right_to_left;
    std::vector<Node*> toposort_left_to_right;

    void clear();

    NodeSocket* find_pin(SocketID id) const;

    NodeLink* find_link(LinkId id) const;

    bool is_pin_linked(SocketID id) const;

    Node* add_node(const char* str);

    unsigned UniqueID();

    void update_socket_vectors_and_owner_node();
    void ensure_topology_cache();
    NodeLink* add_link(
        Node* fromnode,
        NodeSocket* fromsock,
        Node* tonode,
        NodeSocket* tosock);

    void add_link(SocketID startPinId, SocketID endPinId);

    void remove_link(LinkId linkId);

    void delete_node(NodeId nodeId);
    static bool can_create_link(
        NodeSocket* node_socket,
        NodeSocket* node_socket1);
    static bool can_create_direct_link(
        NodeSocket* socket1,
        NodeSocket* socket2);
    static bool can_create_convert_link(
        NodeSocket* node_socket,
        NodeSocket* node_socket1);

    void refresh_node_socket(
        Node* node,
        const SocketDeclaration& socket_declaration,
        const std::vector<NodeSocket*>& old_sockets,
        std::vector<NodeSocket*>& new_sockets);

   private:
    void delete_socket(SocketID socketId);

    void update_directly_linked_links_and_sockets();

    void build_sockets_from_type_info(Node* node);
    void try_fill_value_by_deserialization(Node* node);

    // There is definitely better solution. However this is the most
    std::unordered_set<unsigned> used_ids;

    unsigned current_id = 1;

   public:
    std::string Serialize();
    void Deserialize(const std::string& str);

    void SetDirty(bool dirty = true)
    {
        this->dirty_ = dirty;
    }
    bool GetDirty()
    {
        return dirty_;
    }

   private:
    bool dirty_ = true;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
