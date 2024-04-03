#pragma once
#include <map>
#include <vector>

#include "USTC_CG.h"
#include "Utils/Functions/GenericPointer.hpp"
#include "node_exec.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
struct NodeSocket;
class NodeTree;
struct Node;

struct RuntimeInputState {
    GMutablePointer value = nullptr;
    bool is_forwarded = false;
};

struct RuntimeOutputState {
    GMutablePointer value = nullptr;
};

// Provide single threaded execution. The aim of this executor is simplicity and
// robustness.

class EagerNodeTreeExecutor final : public NodeTreeExecutor {
   public:
    void compile(NodeTree* tree);
    void prepare_memory();
    void prepare_tree(NodeTree* tree)override;
    void execute_tree(NodeTree* tree)override;
    GMutablePointer FindPtr(NodeSocket* socket);
    void sync_node_from_external_storage(NodeSocket* socket, void* data) override;
    void sync_node_to_external_storage(NodeSocket* socket, void* data) override;

   private:
    ExeParams prepare_params(NodeTree* tree, Node* node);
    bool execute_node(NodeTree* tree, Node* node);
    void forward_output_to_input(Node* node);
    void clear();


private:
    std::vector<RuntimeInputState> input_states;
    std::vector<RuntimeOutputState> output_states;
    std::map<NodeSocket*, size_t> index_cache;
    std::vector<Node*> nodes_to_execute;
    std::vector<NodeSocket*> input_of_nodes_to_execute;
    std::vector<NodeSocket*> output_of_nodes_to_execute;
    ptrdiff_t nodes_to_execute_count = 0;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
