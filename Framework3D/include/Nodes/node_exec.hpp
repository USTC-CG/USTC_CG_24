#pragma once

#include <cassert>
#include <iostream>
#include <vector>

#include "USTC_CG.h"
#include "entt/meta/meta.hpp"
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
        if constexpr (std::is_same_v<T, entt::meta_any>) {
            const int index = this->get_input_index(identifier);
            return *inputs_[index];
        }
        else {
            const int index = this->get_input_index(identifier);
            const T& value = inputs_[index]->cast<const T&>();
            return value;
        }
    }

    /**
     * Store the output value for the given socket identifier.
     */
    template<typename T>
    void set_output(const char* identifier, T&& value)
    {
        using DecayT = std::decay_t<T>;

        const int index = this->get_output_index(identifier);

        if (outputs_[index]->type()) {
            outputs_[index]->cast<DecayT&>() = std::forward<T>(value);
        }
        else {
            *outputs_[index] = std::forward<T>(value);
        }
    }

   private:
    int get_input_index(const char* identifier) const;
    int get_output_index(const char* identifier);

    friend class EagerNodeTreeExecutor;

   private:
    std::vector<entt::meta_any*> inputs_;
    std::vector<entt::meta_any*> outputs_;
};

// This executes a tree. The execution strategy is left to its children.
struct NodeTreeExecutor {
   public:
    NodeTreeExecutor() : context(entt::locator<entt::meta_ctx>::value_or())
    {
    }

    virtual ~NodeTreeExecutor() = default;
    virtual void prepare_tree(NodeTree* tree) = 0;
    virtual void execute_tree(NodeTree* tree) = 0;
    virtual void finalize(NodeTree* tree)
    {
    }
    virtual void sync_node_from_external_storage(NodeSocket* socket, const entt::meta_any& data)
    {
    }

    virtual void sync_node_to_external_storage(NodeSocket* socket, entt::meta_any& data)
    {
    }
    void execute(NodeTree* tree)
    {
        prepare_tree(tree);
        execute_tree(tree);
    }

    entt::meta_ctx& get_meta_ctx() const
    {
        return context;
    }

   protected:
    entt::meta_ctx& context;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
