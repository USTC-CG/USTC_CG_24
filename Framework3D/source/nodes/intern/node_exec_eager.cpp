#include "Nodes/node_exec_eager.hpp"

#include "Nodes/node_tree.hpp"
#include "USTC_CG.h"
// #include "Utils/Functions/GenericPointer_.hpp"
//  #include "graph/node_exec_graph.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

ExeParams EagerNodeTreeExecutor::prepare_params(NodeTree* tree, Node* node)
{
    ExeParams params{ *node };
    for (auto&& input : node->inputs) {
        GMutablePointer input_ptr;

        if (input_states[index_cache[input]].is_forwarded) {
            // Is set by previous node
            input_ptr = input_states[index_cache[input]].value;
        }
        else if (input->default_value) {
            input_states[index_cache[input]].value.type()->copy_construct(
                default_value_storage(input),
                input_states[index_cache[input]].value.get());
            input_ptr = input_states[index_cache[input]].value;
        }
        else {
            input_ptr = input_states[index_cache[input]].value;
            input_ptr.type()->default_construct(input_ptr.get());
        }
        params.inputs_.push_back(input_ptr);
    }

    for (auto&& output : node->outputs) {
        auto output_ptr = output_states[index_cache[output]].value;
        params.outputs_.push_back(output_ptr);
    }
    return params;
}

void EagerNodeTreeExecutor::execute_node(NodeTree* tree, Node* node)
{
    ExeParams params = prepare_params(tree, node);
    auto typeinfo = node->typeinfo;
    typeinfo->node_execute(params);
}

void EagerNodeTreeExecutor::forward_output_to_input(Node* node)
{
    for (auto&& output : node->outputs) {
        for (int i = 0; i < output->directly_linked_sockets.size(); ++i) {
            auto directly_linked_input_socket =
                output->directly_linked_sockets[i];

            auto& input_state =
                input_states[index_cache[directly_linked_input_socket]];

            auto& output_state = output_states[index_cache[output]];

            auto& cpp_type = *output->type_info->cpp_type;
            auto is_last_target =
                i == output->directly_linked_sockets.size() - 1;

            auto dst_buffer = input_state.value.get();
            auto value_to_forward = output_state.value;
            if (is_last_target) {
                cpp_type.move_construct(value_to_forward.get(), dst_buffer);
            }
            else {
                cpp_type.copy_construct(value_to_forward.get(), dst_buffer);
            }
            input_state.is_forwarded = true;
        }
    }
}

void EagerNodeTreeExecutor::clear()
{
    for (auto&& input_state : input_states) {
        if (input_state.value.get())
            input_state.value.destruct();
        free(input_state.value.get());
    }

    for (auto&& output_state : output_states) {
        if (output_state.value.get())
            output_state.value.destruct();
        free(output_state.value.get());
    }

    input_states.clear();
    output_states.clear();
    index_cache.clear();
}

void EagerNodeTreeExecutor::execute_tree(NodeTree* tree)
{
    tree->ensure_topology_cache();
    clear();

    auto&& left_to_right = tree->get_toposort_left_to_right();

    input_states.resize(tree->input_sockets.size(), { nullptr, false });
    output_states.resize(tree->output_sockets.size(), { nullptr });

    for (int i = 0; i < input_states.size(); ++i) {
        index_cache[tree->input_sockets[i]] = i;
        auto type = tree->input_sockets[i]->type_info->cpp_type;
        input_states[i].value = { type, malloc(type->size()) };
        input_states[i].value.default_construct();
    }

    for (int i = 0; i < output_states.size(); ++i) {
        index_cache[tree->output_sockets[i]] = i;
        auto type = tree->output_sockets[i]->type_info->cpp_type;
        output_states[i].value = { type, malloc(type->size()) };
        output_states[i].value.default_construct();
    }

    for (auto&& node : left_to_right) {
        execute_node(tree, node);
        forward_output_to_input(node);
    }
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
