#include "Nodes/node_exec_eager.hpp"
#include "Nodes/node_tree.hpp"
#include "USTC_CG.h"
#include "entt/meta/context.hpp"
#include "node_global_params.h"
#include "resource_allocator_instance.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE

bool EagerNodeTreeExecutorRender::execute_node(NodeTree* tree, Node* node)
{
    if (EagerNodeTreeExecutor::execute_node(tree, node)) {
        for (auto&& input : node->get_inputs()) {
            auto& input_state = input_states[index_cache[input]];
            if (!node->typeinfo->ALWAYS_REQUIRED && input_state.is_last_used) {
                if (input_state.value && !input_state.keep_alive)
                    resource_allocator.destroy(input_state.value);
                input_state.is_last_used = false;
            }
        }
        return true;
    }
    for (auto&& output : node->get_outputs()) {
        {
            if (output_states[index_cache[output]].value)
                resource_allocator.destroy(
                    output_states[index_cache[output]].value);
        }
    }
    return false;
}

ExeParams EagerNodeTreeExecutorRender::prepare_params(
    NodeTree* tree,
    Node* node)
{
    auto result = EagerNodeTreeExecutor::prepare_params(tree, node);
    result.global_param = *global_param;

    return result;
}

void EagerNodeTreeExecutorRender::try_storage()
{
    for (auto&& value : storage) {
        resource_allocator.destroy(value.second);
    }
    EagerNodeTreeExecutor::try_storage();
}

void EagerNodeTreeExecutorRender::remove_storage(
    const std::set<std::string>::value_type& key)
{
    resource_allocator.destroy(storage[key]);
    EagerNodeTreeExecutor::remove_storage(key);
}

void EagerNodeTreeExecutorRender::set_global_param(RenderGlobalParams* param)
{
    this->global_param = param;
}

void EagerNodeTreeExecutorRender::finalize(NodeTree* tree)
{
    for (int i = 0; i < input_states.size(); ++i) {
        if (input_states[i].is_last_used && !input_states[i].keep_alive) {
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
    for (auto&& value : storage) {
        resource_allocator.destroy(value.second);
    }
    resource_allocator.terminate();
    storage.clear();
}

EagerNodeTreeExecutorRender::~EagerNodeTreeExecutorRender()
{
}

std::unique_ptr<EagerNodeTreeExecutor> CreateEagerNodeTreeExecutorRender()
{
    return std::make_unique<EagerNodeTreeExecutorRender>();
}

std::unique_ptr<EagerNodeTreeExecutor> CreateEagerNodeTreeExecutor()
{
    return std::make_unique<EagerNodeTreeExecutor>();
}

std::unique_ptr<EagerNodeTreeExecutor> CreateEagerNodeTreeExecutorGeom()
{
    return std::make_unique<EagerNodeTreeExecutorGeom>();
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
