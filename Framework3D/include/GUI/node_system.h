#pragma once
#include <memory>
#include <string>

#include "USTC_CG.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

struct NodeSystemImpl;

class USTC_CG_API NodeSystem {
   public:
    explicit NodeSystem(const std::string& file_name, const std::string& window_name);
    ~NodeSystem();
    void draw_imgui();

   protected:
    std::string window_name;
    std::unique_ptr<NodeSystemImpl> impl_;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
