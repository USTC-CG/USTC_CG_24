#include "Nodes/node_exec_eager.hpp"
#include "Nodes/node_tree.hpp"
#include "USTC_CG.h"
// #include "Utils/Functions/GenericPointer_.hpp"
//  #include "graph/node_exec_graph.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
USTC_CG_NAMESPACE_OPEN_SCOPE
class EagerNodeTreeExecutorSimulation : public EagerNodeTreeExecutor {
   public:
    void prepare_tree(NodeTree* node_tree) override;
    void execute_tree(NodeTree* tree) override;
    ~EagerNodeTreeExecutorSimulation();

   protected:
    bool execute_node(NodeTree* tree, Node* node) override;

    std::map<std::string, GMutablePointer> storage;
    std::vector<GMutablePointer> to_destroy;
};

void EagerNodeTreeExecutorSimulation::prepare_tree(NodeTree* node_tree)
{
    EagerNodeTreeExecutor::prepare_tree(node_tree);
    std::set<std::string> refreshed;

    for (int i = 0; i < input_states.size(); ++i) {
        // If the input is an 'Any' type, the value type is a GMutablePointer. If the input is
        // connected, we need to prepare the memory it points to.
        if (input_states[i].value.is_type<GMutablePointer>()) {
            auto& linked_sockets = input_of_nodes_to_execute[i]->directly_linked_sockets;
            assert(linked_sockets.size() <= 1);

            if (!linked_sockets.empty()) {
                auto input = input_of_nodes_to_execute[i];
                auto node = input->Node;
                // Implementation for now.
                assert(node);

                auto output_state = output_states[index_cache[linked_sockets[0]]];

                auto create_new_storage = [&output_state, this, i]() {
                    auto storage_ptr = GMutablePointer{ output_state.value.type(),
                                                        malloc(output_state.value.type()->size()) };
                    storage_ptr.default_construct();

                    return storage_ptr;
                };

                if (std::string(node->typeinfo->id_name) == "geom_storage_in") {
                    std::string name;
                    CPPType::get<std::string>().copy_assign(
                        default_value_storage(node->inputs[0]), &name);
                    name = std::string(name.c_str());

                    refreshed.emplace(name);

                    GMutablePointer ptr;

                    if (storage.find(name) != storage.end()) {
                        ptr = storage.at(name);

                        if (output_state.value.type() != ptr.type()) {
                            ptr.destruct();
                            free(ptr.get());
                            ptr = storage[name] = create_new_storage();
                        }
                    }
                    else {
                        ptr = storage[name] = create_new_storage();
                    }
                    // This really prepares the memory of the underlying GMutablePointer
                    input_states[i].value.type()->copy_assign(&ptr, input_states[i].value.get());
                }
                else {
                    auto ptr = create_new_storage();
                    to_destroy.push_back(ptr);
                    // This really prepares the memory of the underlying GMutablePointer
                    input_states[i].value.type()->copy_assign(&ptr, input_states[i].value.get());
                }
            }
        }
    }
    std::set<std::string> keysToDelete;
    for (auto&& value : storage) {
        if (!refreshed.contains(value.first)) {
            keysToDelete.emplace(value.first);
        }
    }
    for (auto& key : keysToDelete) {
        storage.erase(key);
    }
}

void EagerNodeTreeExecutorSimulation::execute_tree(NodeTree* tree)
{
    EagerNodeTreeExecutor::execute_tree(tree);
}

EagerNodeTreeExecutorSimulation::~EagerNodeTreeExecutorSimulation()
{
    for (auto&& value : storage) {
        value.second.destruct();
    }
    for (auto destroy : to_destroy) {
        destroy.destruct();
    }
}

bool EagerNodeTreeExecutorSimulation::execute_node(NodeTree* tree, Node* node)
{
    // Identify the special storage node, and do a special execution here.

    if (node->REQUIRED) {  // requirement info is valid.
        if (std::string(node->typeinfo->id_name) == "geom_storage_out") {
            auto input = node->inputs[0];
            std::string name;
            CPPType::get<std::string>().copy_assign(default_value_storage(input), &name);
            name = std::string(name.c_str());
            if (storage.find(name) != storage.end()) {
                auto pointer = storage.at(name);

                // Check all the connected input type

                for (auto input : node->outputs[0]->directly_linked_sockets) {
                    if (pointer.type() != input_states[index_cache[input]].value.type()) {
                        node->execution_failed = "Type Mismatch";
                        return false;
                    }
                }

                CPPType::get<GMutablePointer>().copy_assign(
                    &pointer, output_states[index_cache[node->outputs[0]]].value.get());

                node->execution_failed = {};
                return true;
            }
            else {
                node->execution_failed = "No Matching Storage In";
                return false;
            }
        }
    }

    return EagerNodeTreeExecutor::execute_node(tree, node);
}

std::unique_ptr<EagerNodeTreeExecutor> CreateEagerNodeTreeExecutorSimulation()
{
    return std::make_unique<EagerNodeTreeExecutorSimulation>();
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
