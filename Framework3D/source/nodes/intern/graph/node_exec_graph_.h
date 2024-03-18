/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

/** \file
 * \ingroup fn
 *
 * This file contains a graph data structure that allows composing multiple
 * lazy-functions into a combined lazy-function.
 *
 * There are two types of GraphNodes in the graph:
 * - #FunctionGraphNode: Corresponds to a #LazyFunction. The inputs and outputs of
 * the function become input and output sockets of the GraphNode.
 * - #InterfaceGraphNode: Is used to indicate inputs and outputs of the entire graph.
 * It can have an arbitrary number of sockets.
 */

#include "Utils/Functions/mixins.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
template<typename T>
using Vector = std::vector<T>;
template<typename T>
using Span = std::span<T>;

class Socket;
class InputSocket;
class OutputSocket;
class GraphNode;
class Graph;

/**
 * A #Socket is the interface of a #GraphNode. Every #Socket is either an
 * #InputSocket or #OutputSocket. Links can be created from output sockets to
 * input sockets.
 */
class Socket : NonCopyable, NonMovable {
protected:
    /**
     * The GraphNode the socket belongs to.
     */
    GraphNode* GraphNode_;
    /**
     * Data type of the socket. Only sockets with the same type can be linked.
     */
    const CPPType* type_;
    /**
     * Indicates whether this is an #InputSocket or #OutputSocket.
     */
    bool is_input_;
    /**
     * Index of the socket. E.g. 0 for the first input and the first output
     * socket.
     */
    int index_in_GraphNode_;
    /**
     * Index of the socket in the entire graph. Every socket has a different
     * index.
     */
    int index_in_graph_;

    friend Graph;

public:
    bool is_input() const;
    bool is_output() const;

    int index() const;
    int index_in_graph() const;

    InputSocket& as_input();
    OutputSocket& as_output();
    const InputSocket& as_input() const;
    const OutputSocket& as_output() const;

    const GraphNode& GraphNode() const;
    GraphNode& GraphNode();

    const CPPType& type() const;

    std::string name() const;
    std::string detailed_name() const;
};

class InputSocket : public Socket {
private:
    /**
     * An input can have at most one link connected to it. The linked socket is
     * the "origin" because it's where the data is coming from. The type of the
     * origin must be the same as the type of this socket.
     */
    OutputSocket* origin_;
    /**
     * Can be null or a non-owning pointer to a value of the type of the socket.
     * This value will be used when the input is used but not linked.
     *
     * This is technically not needed, because one could just create a separate
     * GraphNode that just outputs the value, but that would have more overhead.
     * Especially because it's commonly the case that most inputs are unlinked.
     */
    const void* default_value_ = nullptr;

    friend Graph;

public:
    OutputSocket* origin();
    const OutputSocket* origin() const;

    const void* default_value() const;
    void set_default_value(const void* value);
};

class OutputSocket : public Socket {
private:
    /**
     * An output can be linked to an arbitrary number of inputs of the same
     * type.
     */
    Vector<InputSocket*> targets_;

    friend Graph;

public:
    Span<InputSocket*> targets();
    Span<const InputSocket*> targets() const;
};

/**
 * A #GraphNode has input and output sockets. Every GraphNode is either a #FunctionGraphNode or
 * an #InterfaceGraphNode.
 */
class GraphNode : NonCopyable, NonMovable {
protected:
    /**
     * The function this GraphNode corresponds to. If this is null, the GraphNode is an
     * #InterfaceGraphNode. The function is not owned by this #GraphNode nor by the
     * #Graph.
     */
    const LazyFunction* fn_ = nullptr;
    /**
     * Input sockets of the GraphNode.
     */
    Span<InputSocket*> inputs_;
    /**
     * Output sockets of the GraphNode.
     */
    Span<OutputSocket*> outputs_;
    /**
     * An index that is set when calling #Graph::update_GraphNode_indices. This can
     * be used to create efficient mappings from GraphNodes to other data using just
     * an array instead of a hash map.
     *
     * This is technically not necessary but has better performance than always
     * using hash maps.
     */
    int index_in_graph_ = -1;

    friend Graph;

public:
    bool is_interface() const;
    bool is_function() const;
    int index_in_graph() const;

    Span<const InputSocket*> inputs() const;
    Span<const OutputSocket*> outputs() const;
    Span<InputSocket*> inputs();
    Span<OutputSocket*> outputs();

    const InputSocket& input(int index) const;
    const OutputSocket& output(int index) const;
    InputSocket& input(int index);
    OutputSocket& output(int index);

    std::string name() const;
};

/**
 * A #GraphNode that corresponds to a specific #LazyFunction.
 */
class FunctionGraphNode final : public GraphNode {
public:
    const LazyFunction& function() const;
};

/**
 * A #GraphNode that does *not* correspond to a #LazyFunction. Instead it can be used
 * to indicate inputs and outputs of the entire graph. It can have an arbitrary
 * number of inputs and outputs.
 */
class InterfaceGraphNode final : public GraphNode {
private:
    friend GraphNode;
    friend Socket;
    friend Graph;

    Vector<std::string> socket_names_;
};

/**
 * Interface input sockets are actually output sockets on the input GraphNode. This
 * renaming makes the code less confusing.
 */
using GraphInputSocket = OutputSocket;
using GraphOutputSocket = InputSocket;

/**
 * A container for an arbitrary number of GraphNodes and links between their sockets.
 */
class Graph : NonCopyable, NonMovable {
private:
    /**
     * Used to allocate GraphNodes and sockets in the graph.
     */
    LinearAllocator<> allocator_;
    /**
     * Contains all GraphNodes in the graph so that it is efficient to iterate over
     * them. The first two GraphNodes are the interface input and output GraphNodes.
     */
    Vector<GraphNode*> GraphNodes_;

    InterfaceGraphNode* graph_input_GraphNode_ = nullptr;
    InterfaceGraphNode* graph_output_GraphNode_ = nullptr;

    Vector<GraphInputSocket*> graph_inputs_;
    Vector<GraphOutputSocket*> graph_outputs_;

    /**
     * Number of sockets in the graph. Can be used as array size when indexing
     * using `Socket::index_in_graph`.
     */
    int socket_num_ = 0;

public:
    Graph();
    ~Graph();

    /**
     * Get all GraphNodes in the graph. The index in the span corresponds to
     * #GraphNode::index_in_graph.
     */
    Span<const GraphNode*> GraphNodes() const;
    Span<GraphNode*> GraphNodes();

    Span<const FunctionGraphNode*> function_GraphNodes() const;
    Span<FunctionGraphNode*> function_GraphNodes();

    Span<GraphInputSocket*> graph_inputs();
    Span<GraphOutputSocket*> graph_outputs();

    Span<const GraphInputSocket*> graph_inputs() const;
    Span<const GraphOutputSocket*> graph_outputs() const;

    /**
     * Add a new function GraphNode with sockets that match the passed in
     * #LazyFunction.
     */
    FunctionGraphNode& add_function(const LazyFunction& fn);

    /**
     * Add inputs and outputs to the graph.
     */
    GraphInputSocket& add_input(const CPPType& type, std::string name = "");
    GraphOutputSocket& add_output(const CPPType& type, std::string name = "");

    /**
     * Add a link between the two given sockets.
     * This has undefined behavior when the input is linked to something else
     * already.
     */
    void add_link(OutputSocket& from, InputSocket& to);

    /**
     * If the socket is linked, remove the link.
     */
    void clear_origin(InputSocket& socket);

    /**
     * Make sure that #GraphNode::index_in_graph is up to date.
     */
    void update_GraphNode_indices();
    /**
     * Make sure that #Socket::index_in_graph is up to date.
     */
    void update_socket_indices();

    /**
     * Number of sockets in the graph.
     */
    int socket_num() const;

    /**
     * Can be used to assert that #update_GraphNode_indices has been called.
     */
    bool GraphNode_indices_are_valid() const;

    /**
     * Optional configuration options for the dot graph generation. This allows
     * creating visualizations for specific purposes.
     */
    class ToDotOptions {
    public:
        virtual std::string socket_name(const Socket& socket) const;
        virtual std::optional<std::string> socket_font_color(
            const Socket& socket) const;
        virtual void add_edge_attributes(
            const OutputSocket& from,
            const InputSocket& to,
            dot::DirectedEdge& dot_edge) const;
    };

    /**
     * Utility to generate a dot graph string for the graph. This can be used
     * for debugging.
     */
    std::string to_dot(const ToDotOptions& options = {}) const;
};

/* -------------------------------------------------------------------- */
/** \name #Socket Inline Methods
 * \{ */

inline bool Socket::is_input() const
{
    return is_input_;
}

inline bool Socket::is_output() const
{
    return !is_input_;
}

inline int Socket::index() const
{
    return index_in_GraphNode_;
}

inline int Socket::index_in_graph() const
{
    return index_in_graph_;
}

inline InputSocket& Socket::as_input()
{
    assert(this->is_input());
    return *static_cast<InputSocket*>(this);
}

inline OutputSocket& Socket::as_output()
{
    assert(this->is_output());
    return *static_cast<OutputSocket*>(this);
}

inline const InputSocket& Socket::as_input() const
{
    assert(this->is_input());
    return *static_cast<const InputSocket*>(this);
}

inline const OutputSocket& Socket::as_output() const
{
    assert(this->is_output());
    return *static_cast<const OutputSocket*>(this);
}

inline const GraphNode& Socket::GraphNode() const
{
    return *GraphNode_;
}

inline GraphNode& Socket::GraphNode()
{
    return *GraphNode_;
}

inline const CPPType& Socket::type() const
{
    return *type_;
}

/** \} */

/* -------------------------------------------------------------------- */
/** \name #InputSocket Inline Methods
 * \{ */

inline const OutputSocket* InputSocket::origin() const
{
    return origin_;
}

inline OutputSocket* InputSocket::origin()
{
    return origin_;
}

inline const void* InputSocket::default_value() const
{
    return default_value_;
}

inline void InputSocket::set_default_value(const void* value)
{
    default_value_ = value;
}

/** \} */

/* -------------------------------------------------------------------- */
/** \name #OutputSocket Inline Methods
 * \{ */

inline Span<const InputSocket*> OutputSocket::targets() const
{
    return targets_;
}

inline Span<InputSocket*> OutputSocket::targets()
{
    return targets_;
}

/** \} */

/* -------------------------------------------------------------------- */
/** \name #GraphNode Inline Methods
 * \{ */

inline bool GraphNode::is_interface() const
{
    return fn_ == nullptr;
}

inline bool GraphNode::is_function() const
{
    return fn_ != nullptr;
}

inline int GraphNode::index_in_graph() const
{
    return index_in_graph_;
}

inline Span<const InputSocket*> GraphNode::inputs() const
{
    return inputs_;
}

inline Span<const OutputSocket*> GraphNode::outputs() const
{
    return outputs_;
}

inline Span<InputSocket*> GraphNode::inputs()
{
    return inputs_;
}

inline Span<OutputSocket*> GraphNode::outputs()
{
    return outputs_;
}

inline const InputSocket& GraphNode::input(const int index) const
{
    return *inputs_[index];
}

inline const OutputSocket& GraphNode::output(const int index) const
{
    return *outputs_[index];
}

inline InputSocket& GraphNode::input(const int index)
{
    return *inputs_[index];
}

inline OutputSocket& GraphNode::output(const int index)
{
    return *outputs_[index];
}

/** \} */

/* -------------------------------------------------------------------- */
/** \name #FunctionGraphNode Inline Methods
 * \{ */

inline const LazyFunction& FunctionGraphNode::function() const
{
    assert(fn_ != nullptr);
    return *fn_;
}

/** \} */

/* -------------------------------------------------------------------- */
/** \name #Graph Inline Methods
 * \{ */

inline Span<const GraphNode*> Graph::GraphNodes() const
{
    return GraphNodes_;
}

inline Span<GraphNode*> Graph::GraphNodes()
{
    return GraphNodes_;
}

inline Span<const FunctionGraphNode*> Graph::function_GraphNodes() const
{
    return GraphNodes_.as_span().drop_front(2).cast<const FunctionGraphNode*>();
}

inline Span<FunctionGraphNode*> Graph::function_GraphNodes()
{
    return GraphNodes_.as_span().drop_front(2).cast<FunctionGraphNode*>();
}

inline Span<GraphInputSocket*> Graph::graph_inputs()
{
    return graph_inputs_;
}

inline Span<GraphOutputSocket*> Graph::graph_outputs()
{
    return graph_outputs_;
}

inline Span<const GraphInputSocket*> Graph::graph_inputs() const
{
    return graph_inputs_;
}

inline Span<const GraphOutputSocket*> Graph::graph_outputs() const
{
    return graph_outputs_;
}

inline int Graph::socket_num() const
{
    return socket_num_;
}

/** \} */
USTC_CG_NAMESPACE_CLOSE_SCOPE
