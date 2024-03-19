#pragma once

#include "GCore/GOP.h"
#include "GCore/GlobalUsdStage.h"
#include "Nodes/node.hpp"
#include "Nodes/node_tree.hpp"
#include "USTC_CG.h"
#include "imgui.h"
#include "pxr/usd/usd/primRange.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
class NodeSystemExecution {
   public:
    NodeSystemExecution();

    Node* create_node_menu();

    std::vector<std::unique_ptr<Node>>& get_nodes();

    std::string Serialize();
    void Deserialize(const std::string& str);

    NodeSocket* FindPin(SocketID id)
    {
        return node_tree->FindPin(id);
    }

    NodeLink* FindLink(LinkId id)
    {
        return node_tree->FindLink(id);
    }

    void CreateLink(SocketID startPinId, SocketID endPinId);

    void MarkDirty()
    {
        required_execution = true;
    }

    void RemoveLink(LinkId linkId);

    bool IsPinLinked(SocketID id)
    {
        return node_tree->IsPinLinked(id);
    }

    void delete_node(NodeId id);

    void trigger_refresh_topology();

    void show_debug_info();

    unsigned GetNextId();

    void try_execution()
    {
        if (required_execution) {
            // for (auto&& prim: GlobalUsdStage::global_usd_stage->TraverseAll()) {
            auto& stage = GlobalUsdStage::global_usd_stage;
            stage->RemovePrim(pxr::SdfPath("/geom"));
            stage->RemovePrim(pxr::SdfPath("/TexModel"));

            executor->execute_tree(node_tree.get());
            required_execution = false;
        }
    }

    std::vector<std::unique_ptr<NodeLink>>& get_links()
    {
        return node_tree->links;
    }

    std::unique_ptr<NodeTree> node_tree;
    std::unique_ptr<NodeTreeExecutor> executor;

    bool CanCreateLink(NodeSocket* a, NodeSocket* b);

    std::string filename = "Blueprints.json";

   private:
    bool required_execution = true;

    unsigned m_NextId = 1;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
