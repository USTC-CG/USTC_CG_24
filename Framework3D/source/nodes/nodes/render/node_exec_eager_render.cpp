#include "Nodes/node_exec_eager.hpp"
#include "Nodes/node_tree.hpp"
#include "USTC_CG.h"
#include "resource_allocator_instance.hpp"
// #include "Utils/Functions/GenericPointer_.hpp"
//  #include "graph/node_exec_graph.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
class EagerNodeTreeExecutorRender : public EagerNodeTreeExecutor {
   protected:
    bool execute_node(NodeTree* tree, Node* node) override;
};

bool EagerNodeTreeExecutorRender::execute_node(NodeTree* tree, Node* node)
{
    if (EagerNodeTreeExecutor::execute_node(tree, node)) {
        for (auto&& input : node->inputs) {
            if (input_states[index_cache[input]].is_last_used) {
                resource_allocator.destroy(input_states[index_cache[input]].value);
            }
        }
        return true;
    }
    return false;
}

std::unique_ptr<EagerNodeTreeExecutor> CreateEagerNodeTreeExecutorRender()
{
    return std::make_unique<EagerNodeTreeExecutorRender>();
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
