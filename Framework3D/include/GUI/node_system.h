#pragma once
#include <complex.h>

#include <memory>
#include <string>

#include "USTC_CG.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
struct NodeTreeExecutor;

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
    NodeTreeExecutor* get_executor() const;

    float cached_last_time_code();
    void set_required_time_code(float time_code_to_render);

protected:
    std::string window_name;
    NodeSystemType node_system_type;
    std::unique_ptr<NodeSystemImpl> impl_;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
