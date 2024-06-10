#include "Nodes/node_exec_eager.hpp"
#include "Nodes/node_tree.hpp"
#include "USTC_CG.h"
// #include "Utils/Functions/GenericPointer_.hpp"
//  #include "graph/node_exec_graph.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <set>

#include "entt/meta/resolve.hpp"
USTC_CG_NAMESPACE_OPEN_SCOPE
class EagerNodeTreeExecutorSimulation : public EagerNodeTreeExecutor {
   public:
    void prepare_tree(NodeTree* node_tree) override;
    void execute_tree(NodeTree* tree) override;
    ~EagerNodeTreeExecutorSimulation();

   protected:
    bool execute_node(NodeTree* tree, Node* node) override;

    std::map<std::string, entt::meta_any> storage;
};

void EagerNodeTreeExecutorSimulation::prepare_tree(NodeTree* node_tree)
{
    EagerNodeTreeExecutor::prepare_tree(node_tree);
    std::set<std::string> refreshed;

    // After executing the tree, storage all the required info
    for (int i = 0; i < input_of_nodes_to_execute.size(); ++i) {
        auto socket = input_of_nodes_to_execute[i];
        if (socket->type_info->type == SocketType::Any) {
            if (std::string(socket->Node->typeinfo->id_name) ==
                "geom_storage_in") {
                auto node = socket->Node;
                entt::meta_any data;
                if (!socket->directly_linked_sockets.empty()) {
                    auto input = node->get_inputs()[0];
                    std::string name =
                        input->default_value_typed<std::string>();
                    if (storage.find(name) == storage.end()) {
                        data = socket->directly_linked_sockets[0]
                                   ->type_info->cpp_type.construct();
                        storage[name] = data;
                    }
                    refreshed.emplace(name);
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
    refreshed.clear();
}

void EagerNodeTreeExecutorSimulation::execute_tree(NodeTree* tree)
{
    EagerNodeTreeExecutor::execute_tree(tree);

    // After executing the tree, storage all the required info
    for (int i = 0; i < input_of_nodes_to_execute.size(); ++i) {
        auto socket = input_of_nodes_to_execute[i];
        if (socket->type_info->type == SocketType::Any) {
            if (std::string(socket->Node->typeinfo->id_name) ==
                "geom_storage_in") {
                auto node = socket->Node;
                entt::meta_any data;
                sync_node_to_external_storage(
                    input_of_nodes_to_execute[i], data);

                auto input = node->get_inputs()[0];
                std::string name = input->default_value_typed<std::string>();
                storage[name] = data;
            }
        }
    }
}

EagerNodeTreeExecutorSimulation::~EagerNodeTreeExecutorSimulation()
{
    storage.clear();
}

bool EagerNodeTreeExecutorSimulation::execute_node(NodeTree* tree, Node* node)
{
    // Identify the special storage node, and do a special execution here.

    if (node->REQUIRED) {  // requirement info is valid.
        if (std::string(node->typeinfo->id_name) == "geom_storage_out") {
            auto input = node->get_inputs()[0];
            std::string name = input->default_value_typed<std::string>();
            if (storage.find(name) != storage.end()) {
                auto& storaged_value = storage.at(name);

                // Check all the connected input type

                for (auto input :
                     node->get_outputs()[0]->directly_linked_sockets) {
                    if (storaged_value.type() &&
                        storaged_value.type() !=
                            input_states[index_cache[input]].value.type()) {
                        node->execution_failed = "Type Mismatch, filling default value.";
                        return false;
                    }
                }

                output_states[index_cache[node->get_outputs()[0]]].value =
                    storaged_value;

                node->execution_failed = {};
                return true;
            }
            else {
                node->execution_failed =
                    "No cache can be found with name " + name + " (yet).";
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
