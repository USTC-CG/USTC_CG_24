#pragma once
#include <memory>
#include <string>

#include "USTC_CG.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
enum class NodeSystemType { Geometry, Render, Composition };

struct NodeSystemImpl;
class NodeTree;

class USTC_CG_API NodeSystem {
   public:
    explicit NodeSystem(
        NodeSystemType type,
        const std::string& file_name,
        const std::string& window_name);
    ~NodeSystem();
    void draw_imgui();
    NodeTree* get_tree();

   protected:
    std::string window_name;
    NodeSystemType node_system_type;
    std::unique_ptr<NodeSystemImpl> impl_;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
