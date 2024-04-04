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

   public:
    void finalize(NodeTree* tree) override
    {
        for (int i = 0; i < input_states.size(); ++i) {
            if (input_states[i].is_last_used) {
                resource_allocator.destroy(input_states[i].value);
                input_states[i].is_last_used = false;
            }
        }

        for (int i = 0; i < output_states.size(); ++i) {
            if (output_states[i].is_last_used) {
                resource_allocator.destroy(output_states[i].value);
                output_states[i].is_last_used = false;
            }
        }
    }
};

bool EagerNodeTreeExecutorRender::execute_node(NodeTree* tree, Node* node)
{
    if (EagerNodeTreeExecutor::execute_node(tree, node)) {
        for (auto&& input : node->inputs) {
            if (!node->typeinfo->ALWAYS_REQUIRED && input_states[index_cache[input]].is_last_used) {
                if (input_states[index_cache[input]].value.get())
                    resource_allocator.destroy(input_states[index_cache[input]].value);
                input_states[index_cache[input]].is_last_used = false;
            }
        }
        return true;
    }
    else {
        for (auto&& output : node->outputs) {
            {
                if (output_states[index_cache[output]].value.get())
                    resource_allocator.destroy(output_states[index_cache[output]].value);
            }
        }
    }
    return false;
}

std::unique_ptr<EagerNodeTreeExecutor> CreateEagerNodeTreeExecutorRender()
{
    return std::make_unique<EagerNodeTreeExecutorRender>();
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
