#pragma once

#include <cassert>
#include <vector>

#include "USTC_CG.h"
#include "Utils/Functions/GenericPointer.hpp"
// #include "Utils/Functions/GenericPointer.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
struct NodeSocket;
struct Node;
struct NodeTree;

struct ExeParams {
    const Node& node_;

    explicit ExeParams(const Node& node) : node_(node)
    {
    }

    /**
     * Get the input value for the input socket with the given identifier.
     */
    template<typename T>
    T get_input(const char* identifier) const
    {
        const int index = this->get_input_index(identifier);
        const T& value = *static_cast<T*>(inputs_[index].get());
        return value;
    }

    /**
     * Store the output value for the given socket identifier.
     */
    template<typename T>
    void set_output(const char* identifier, T&& value)
    {
        using DecayT = std::decay_t<T>;

        const int index = this->get_output_index(identifier);
        *outputs_[index].get<DecayT>() = std::forward<T>(value);
    }

   private:
    int get_input_index(const char* identifier) const;
    int get_output_index(const char* identifier);

    friend class EagerNodeTreeExecutor;

   private:
    std::vector<GMutablePointer> inputs_;
    std::vector<GMutablePointer> outputs_;
};

// This executes a tree. The execution strategy is left to its children.
struct NodeTreeExecutor {
    virtual ~NodeTreeExecutor() = default;
    virtual void prepare_tree(NodeTree* tree) = 0;
    virtual void execute_tree(NodeTree* tree) = 0;
    virtual void finalize(NodeTree* tree)
    {
    }
    virtual void sync_node_from_external_storage(NodeSocket* socket, void* data)
    {
    }

    virtual void sync_node_to_external_storage(NodeSocket* socket, void* data)
    {
    }
    void execute(NodeTree* tree)
    {
        prepare_tree(tree);
        execute_tree(tree);
    }
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
