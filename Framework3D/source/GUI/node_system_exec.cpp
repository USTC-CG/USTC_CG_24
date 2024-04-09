#include "node_system_exec.h"

#include "Nodes/GlobalUsdStage.h"
#include "Nodes/node_exec_eager.hpp"
#include "Nodes/node_register.h"
#include "Nodes/node_tree.hpp"
#include "imgui/imgui-node-editor/imgui_node_editor.h"
#include "pxr/usd/usdGeom/metrics.h"
#include "pxr/usd/usdGeom/tokens.h"
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

    USTC_CG::logging("Create node system not implemented for this type.", Error);

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

void NodeSystemExecution::try_execution()
{
    if (required_execution) {
        executor->execute(node_tree.get());
        required_execution = false;
    }
}

bool NodeSystemExecution::CanCreateLink(NodeSocket* a, NodeSocket* b)
{
    return node_tree->CanCreateLink(a, b);
}

Node* NodeSystemExecution::default_node_menu(const std::map<std::string, NodeTypeInfo*>& registry)
{
    Node* node = nullptr;

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

// This is NOT best practice.
void GeoNodeSystemExecution::try_execution()
{
    if (required_execution) {
        auto& stage = GlobalUsdStage::global_usd_stage;
        stage->RemovePrim(pxr::SdfPath("/geom"));
        stage->RemovePrim(pxr::SdfPath("/TexModel"));

        executor->execute(node_tree.get());
        required_execution = false;
    }
}

Node* GeoNodeSystemExecution::create_node_menu()
{
    auto& geo_node_registry = get_geo_node_registry();
    auto& func_node_registry = get_func_node_registry();

    Node* node = nullptr;

    if (ImGui::BeginMenu("Function Nodes")) {
        for (auto&& value : func_node_registry) {
            auto name = value.second->ui_name;
            if (ImGui::MenuItem(name))
                node = node_tree->nodeAddNode(value.second->id_name);
        }
        ImGui::EndMenu();
    }

    for (auto&& value : geo_node_registry) {
        auto name = value.second->ui_name;
        if (ImGui::MenuItem(name))
            node = node_tree->nodeAddNode(value.second->id_name);
    }

    return node;
}

Node* RenderNodeSystemExecution::create_node_menu()
{
    auto& render_registry = get_render_node_registry();
    auto& func_node_registry = get_func_node_registry();

    Node* node = nullptr;

    if (ImGui::BeginMenu("Function Nodes")) {
        for (auto&& value : func_node_registry) {
            auto name = value.second->ui_name;
            if (ImGui::MenuItem(name))
                node = node_tree->nodeAddNode(value.second->id_name);
        }
        ImGui::EndMenu();
    }

    for (auto&& value : render_registry) {
        auto name = value.second->ui_name;
        if (ImGui::MenuItem(name))
            node = node_tree->nodeAddNode(value.second->id_name);
    }

    return node;
}

void CompositionNodeSystemExecution::try_execution()
{
    if (required_execution) {
        auto& stage = GlobalUsdStage::global_usd_stage;
        // stage->RemovePrim(pxr::SdfPath("/Reference"));
        pxr::UsdGeomSetStageUpAxis(stage, pxr::UsdGeomTokens->z);

        executor->execute(node_tree.get());
        required_execution = false;
    }
}

Node* CompositionNodeSystemExecution::create_node_menu()
{
    auto& composition_registry = get_composition_node_registry();
    auto& func_node_registry = get_func_node_registry();

    Node* node = nullptr;

    if (ImGui::BeginMenu("Function Nodes")) {
        for (auto&& value : func_node_registry) {
            auto name = value.second->ui_name;
            if (ImGui::MenuItem(name))
                node = node_tree->nodeAddNode(value.second->id_name);
        }
        ImGui::EndMenu();
    }

    for (auto&& value : composition_registry) {
        auto name = value.second->ui_name;
        if (ImGui::MenuItem(name))
            node = node_tree->nodeAddNode(value.second->id_name);
    }

    return node;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
