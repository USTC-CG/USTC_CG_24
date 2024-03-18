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

struct EagerNodeTreeExecutor : public NodeTreeExecutor {
    void execute_tree(NodeTree* tree) override;

   private:
    ExeParams prepare_params(NodeTree* tree, Node* node);
    void execute_node(NodeTree* tree, Node* node);
    void forward_output_to_input(Node* node);
    void clear();

    std::vector<RuntimeInputState> input_states;
    std::vector<RuntimeOutputState> output_states;
    std::map<NodeSocket*, size_t> index_cache;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
