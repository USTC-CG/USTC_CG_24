#pragma once
#include <memory>

#include "USTC_CG.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

struct NodeSystemImpl;

class USTC_CG_API NodeSystem
{
public:
    explicit NodeSystem();
    ~NodeSystem();
    void draw_imgui();

protected:
    std::unique_ptr<NodeSystemImpl> impl_;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
