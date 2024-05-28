#pragma once
#include <map>
#include <vector>

#include "USTC_CG.h"
#include "entt/meta/meta.hpp"
#include "node_exec.hpp"
#include "node_tree.hpp"

namespace nvrhi {
class IDevice;
}

USTC_CG_NAMESPACE_OPEN_SCOPE
struct NodeSocket;
struct Node;

struct RuntimeInputState {
    entt::meta_any value;
    bool is_forwarded = false;
    bool is_last_used = false;
};

struct RuntimeOutputState {
    entt::meta_any value;
    bool is_last_used = false;
};

// Provide single threaded execution. The aim of this executor is simplicity and
// robustness.

class EagerNodeTreeExecutor : public NodeTreeExecutor {
   public:
    void compile(NodeTree* tree);
    void prepare_memory();
    void prepare_tree(NodeTree* tree) override;
    void execute_tree(NodeTree* tree) override;

    entt::meta_any* FindPtr(NodeSocket* socket);
    void sync_node_from_external_storage(NodeSocket* socket, const entt::meta_any& data) override;
    void sync_node_to_external_storage(NodeSocket* socket, entt::meta_any& data) override;

   protected:
    virtual ExeParams prepare_params(NodeTree* tree, Node* node);
    virtual bool execute_node(NodeTree* tree, Node* node);
    void forward_output_to_input(Node* node);
    void clear();

    std::vector<RuntimeInputState> input_states;
    std::vector<RuntimeOutputState> output_states;
    std::map<NodeSocket*, size_t> index_cache;
    std::vector<Node*> nodes_to_execute;
    std::vector<NodeSocket*> input_of_nodes_to_execute;
    std::vector<NodeSocket*> output_of_nodes_to_execute;
    ptrdiff_t nodes_to_execute_count = 0;
};

inline std::unique_ptr<EagerNodeTreeExecutor> CreateEagerNodeTreeExecutor()
{
    return std::make_unique<EagerNodeTreeExecutor>();
}
class EagerNodeTreeExecutorRender : public EagerNodeTreeExecutor {
   protected:
    bool execute_node(NodeTree* tree, Node* node) override;

   public:
    void finalize(NodeTree* tree) override;
    virtual void set_device(
        nvrhi::IDevice*
            device);  // Make this virtual to send it to vtable. A better practice should definitely
                      // be better solving the 'resource allocator' setting issue.

    virtual void reset_allocator();
};

std::unique_ptr<EagerNodeTreeExecutor> CreateEagerNodeTreeExecutorRender();
std::unique_ptr<EagerNodeTreeExecutor> CreateEagerNodeTreeExecutorSimulation();

USTC_CG_NAMESPACE_CLOSE_SCOPE
