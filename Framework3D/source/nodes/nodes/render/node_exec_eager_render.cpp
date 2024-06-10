#include "Nodes/node_exec_eager.hpp"
#include "Nodes/node_tree.hpp"
#include "USTC_CG.h"
#include "resource_allocator_instance.hpp"
// #include "Utils/Functions/GenericPointer_.hpp"
//  #include "graph/node_exec_graph.h"

#include "entt/meta/context.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE

bool EagerNodeTreeExecutorRender::execute_node(NodeTree* tree, Node* node)
{
    if (EagerNodeTreeExecutor::execute_node(tree, node)) {
        for (auto&& input : node->get_inputs()) {
            if (!node->typeinfo->ALWAYS_REQUIRED && input_states[index_cache[input]].is_last_used) {
                if (input_states[index_cache[input]].value)
                    resource_allocator.destroy(input_states[index_cache[input]].value);
                input_states[index_cache[input]].is_last_used = false;
            }
        }
        return true;
    }
    else {
        for (auto&& output : node->get_outputs()) {
            {
                if (output_states[index_cache[output]].value)
                    resource_allocator.destroy(output_states[index_cache[output]].value);
            }
        }
    }
    return false;
}

void EagerNodeTreeExecutorRender::finalize(NodeTree* tree)
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

void EagerNodeTreeExecutorRender::set_device(nvrhi::IDevice* device)
{
    resource_allocator.set_device(device);
}

void EagerNodeTreeExecutorRender::reset_allocator()
{
    resource_allocator.terminate();
}

std::unique_ptr<EagerNodeTreeExecutor> CreateEagerNodeTreeExecutorRender()
{
    return std::make_unique<EagerNodeTreeExecutorRender>();
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
