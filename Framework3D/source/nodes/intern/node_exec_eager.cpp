#include "Nodes/node_exec_eager.hpp"

#include "Nodes/node_tree.hpp"
#include "USTC_CG.h"
// #include "Utils/Functions/GenericPointer_.hpp"
//  #include "graph/node_exec_graph.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

ExeParams EagerNodeTreeExecutor::prepare_params(NodeTree* tree, Node* node)
{
    node->MISSING_INPUT = false;

    ExeParams params{ *node };
    for (auto&& input : node->inputs) {
        GMutablePointer input_ptr;

        if (input_states[index_cache[input]].is_forwarded) {
            // Is set by previous node
            input_ptr = input_states[index_cache[input]].value;
        }
        else if (input->default_value) {
            // Has default value
            input_states[index_cache[input]].value.type()->copy_construct(
                default_value_storage(input), input_states[index_cache[input]].value.get());
            input_ptr = input_states[index_cache[input]].value;
        }
        else {
            // Node not filled. Cannot run this node.
            input_ptr = input_states[index_cache[input]].value;
            input_ptr.type()->default_construct(input_ptr.get());

            node->MISSING_INPUT = true;
        }
        params.inputs_.push_back(input_ptr);
    }

    for (auto&& output : node->outputs) {
        auto output_ptr = output_states[index_cache[output]].value;
        params.outputs_.push_back(output_ptr);
    }

    return params;
}

bool EagerNodeTreeExecutor::execute_node(NodeTree* tree, Node* node)
{
    ExeParams params = prepare_params(tree, node);
    if (node->MISSING_INPUT) {
        return false;
    }
    auto typeinfo = node->typeinfo;
    try {
        typeinfo->node_execute(params);
        node->execution_failed = {};
    }
    catch (std::runtime_error(err)) {
        node->execution_failed = err.what();
        return false;
    }
    return true;
}

void EagerNodeTreeExecutor::forward_output_to_input(Node* node)
{
    for (auto&& output : node->outputs) {
        if (output->directly_linked_sockets.empty()) {
            auto& output_state = output_states[index_cache[output]];
            assert(output_state.is_last_used == false);
            output_state.is_last_used = true;
        }
        else {
            int last_used_id = -1;

            for (int i = 0; i < output->directly_linked_sockets.size(); ++i) {
                auto directly_linked_input_socket = output->directly_linked_sockets[i];

                if (index_cache.find(directly_linked_input_socket) != index_cache.end()) {
                    if (directly_linked_input_socket->Node->REQUIRED) {
                        last_used_id =
                            std::max(last_used_id, int(index_cache[directly_linked_input_socket]));
                    }

                    auto& input_state = input_states[index_cache[directly_linked_input_socket]];

                    auto& output_state = output_states[index_cache[output]];

                    auto& cpp_type = *output->type_info->cpp_type;
                    auto is_last_target = i == output->directly_linked_sockets.size() - 1;

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
            if (last_used_id == -1) {
                output_states[index_cache[output]].is_last_used = true;
            }
            else {
                assert(input_states[last_used_id].is_last_used == false);

                input_states[last_used_id].is_last_used = true;
            }
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
    nodes_to_execute.clear();
    nodes_to_execute_count = 0;
    input_of_nodes_to_execute.clear();
    output_of_nodes_to_execute.clear();
}

void EagerNodeTreeExecutor::compile(NodeTree* tree)
{
    if (tree->has_available_link_cycle) {
        return;
    }

    nodes_to_execute = tree->get_toposort_left_to_right();
    for (int i = nodes_to_execute.size() - 1; i >= 0; i--) {
        nodes_to_execute[i]->REQUIRED = false;
    }

    for (int i = nodes_to_execute.size() - 1; i >= 0; i--) {
        auto node = nodes_to_execute[i];

        if (node->typeinfo->ALWAYS_REQUIRED) {
            node->REQUIRED = true;
        }

        if (node->REQUIRED) {
            for (auto input : node->inputs) {
                assert(input->directly_linked_sockets.size() <= 1);
                for (auto directly_linked_socket : input->directly_linked_sockets) {
                    directly_linked_socket->Node->REQUIRED = true;
                }
            }
        }
    }

    auto split =
        std::stable_partition(nodes_to_execute.begin(), nodes_to_execute.end(), [](Node* node) {
            return node->REQUIRED;
        });

    // Now the nodes is split into two parts, and the topology sequence is correct.

    nodes_to_execute_count = std::distance(nodes_to_execute.begin(), split);

    for (int i = 0; i < nodes_to_execute_count; ++i) {
        input_of_nodes_to_execute.insert(
            input_of_nodes_to_execute.end(),
            nodes_to_execute[i]->inputs.begin(),
            nodes_to_execute[i]->inputs.end());

        output_of_nodes_to_execute.insert(
            output_of_nodes_to_execute.end(),
            nodes_to_execute[i]->outputs.begin(),
            nodes_to_execute[i]->outputs.end());
    }
}

void EagerNodeTreeExecutor::prepare_memory()
{
    for (int i = 0; i < input_states.size(); ++i) {
        index_cache[input_of_nodes_to_execute[i]] = i;
        auto type = input_of_nodes_to_execute[i]->type_info->cpp_type;
        input_states[i].value = { type, malloc(type->size()) };
        input_states[i].value.default_construct();
    }

    for (int i = 0; i < output_states.size(); ++i) {
        index_cache[output_of_nodes_to_execute[i]] = i;
        auto type = output_of_nodes_to_execute[i]->type_info->cpp_type;
        output_states[i].value = { type, malloc(type->size()) };
        output_states[i].value.default_construct();
    }
}

void EagerNodeTreeExecutor::prepare_tree(NodeTree* tree)
{
    tree->ensure_topology_cache();
    clear();

    compile(tree);

    input_states.resize(input_of_nodes_to_execute.size(), { nullptr, false });
    output_states.resize(output_of_nodes_to_execute.size(), { nullptr });

    prepare_memory();
}

void EagerNodeTreeExecutor::execute_tree(NodeTree* tree)
{
    for (int i = 0; i < nodes_to_execute_count; ++i) {
        auto node = nodes_to_execute[i];
        auto result = execute_node(tree, node);
        if (result) {
            forward_output_to_input(node);
        }
    }
}

GMutablePointer EagerNodeTreeExecutor::FindPtr(NodeSocket* socket)
{
    GMutablePointer ptr;
    if (socket->in_out == PinKind::Input) {
        ptr = input_states[index_cache[socket]].value;
    }
    else {
        ptr = output_states[index_cache[socket]].value;
    }
    return ptr;
}

void EagerNodeTreeExecutor::sync_node_from_external_storage(NodeSocket* socket, void* data)
{
    if (index_cache.find(socket) != index_cache.end()) {
        GMutablePointer ptr = FindPtr(socket);
        ptr.type()->copy_construct(data, ptr.get());
    }
}

void EagerNodeTreeExecutor::sync_node_to_external_storage(NodeSocket* socket, void* data)
{
    if (index_cache.find(socket) != index_cache.end()) {
        GMutablePointer ptr = FindPtr(socket);
        ptr.type()->copy_construct(ptr.get(), data);
    }
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
