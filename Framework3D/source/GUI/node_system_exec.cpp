#include "node_system_exec.h"

#include "Nodes/node_exec_eager.hpp"
#include "Nodes/node_register.h"
#include "Nodes/node_tree.hpp"
#include "imgui/imgui-node-editor/imgui_node_editor.h"
USTC_CG_NAMESPACE_OPEN_SCOPE
namespace ed = ax::NodeEditor;

NodeSystemExecution::NodeSystemExecution()
{
    static std::once_flag register_flag;
    std::call_once(register_flag, register_all);
    node_tree = std::make_unique<NodeTree>();
    executor = std::make_unique<EagerNodeTreeExecutor>();
}

Node* NodeSystemExecution::create_node_menu()
{
    Node* node = nullptr;

    auto& registry = get_node_registry();

    for (auto&& value : registry) {
        auto name = value.second->ui_name;
        if (ImGui::MenuItem(name))
            node = node_tree->nodeAddNode(value.second->id_name);
    }

    if (node) {
        node->override_left_pane_info = [node]() {
            auto print_func = [](NodeSocket* socket, int i) {
                if (socket->in_out == PinKind::Input) {
                    ImGui::Text("Input %i:", i);
                }
                else {
                    ImGui::Text("Output %i:", i);
                }
                ImGui::Indent();
                ImGui::Text("directly_linked_links %i", socket->directly_linked_links.size());

                ImGui::Text("directly_linked_sockets %i", socket->directly_linked_sockets.size());
                ImGui::Unindent();
            };

            for (int i = 0; i < node->inputs.size(); ++i) {
                NodeSocket* socket = node->inputs[i];
                print_func(socket, i);
            }

            for (int i = 0; i < node->outputs.size(); ++i) {
                NodeSocket* socket = node->outputs[i];
                print_func(socket, i);
            }
        };
        MarkDirty();
    }

    return node;
}

std::vector<std::unique_ptr<Node>>& NodeSystemExecution::get_nodes()
{
    return node_tree->nodes;
}

std::string NodeSystemExecution::Serialize()
{
    return node_tree->Serialize();
}

void NodeSystemExecution::Deserialize(const std::string& str)
{
    node_tree->Deserialize(str);
    MarkDirty();
}

void NodeSystemExecution::CreateLink(SocketID startPinId, SocketID endPinId)
{
    node_tree->nodeAddLink(startPinId, endPinId);
    MarkDirty();
}

void NodeSystemExecution::RemoveLink(LinkId linkId)
{
    node_tree->RemoveLink(linkId);
    MarkDirty();
}

void NodeSystemExecution::delete_node(NodeId nodeId)
{
    node_tree->delete_node(nodeId);
    MarkDirty();
}

void NodeSystemExecution::trigger_refresh_topology()
{
    node_tree->ensure_topology_cache();
}

void NodeSystemExecution::show_debug_info()
{
    ImGui::Text("Node count: %i", node_tree->nodes.size());
    ImGui::Text("Socket count: %i", node_tree->sockets.size());
    ImGui::Text("Link count: %i", node_tree->links.size());
    ImGui::Text("Topology left to right:");
    for (size_t i = 0; i < node_tree->get_toposort_left_to_right().size(); i++) {
        ImGui::Text("%i,%s", i, node_tree->get_toposort_left_to_right()[i]->typeinfo->ui_name);
    }

    if (node_tree->has_available_link_cycle) {
        ImGui::Text("This graph has a cycle");
    }
    else

    {
        ImGui::Text("This graph doesn't have a cycle");
    }
}

unsigned NodeSystemExecution::GetNextId()
{
    return m_NextId++;
}

bool NodeSystemExecution::CanCreateLink(NodeSocket* a, NodeSocket* b)
{
    return node_tree->CanCreateLink(a, b);
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
