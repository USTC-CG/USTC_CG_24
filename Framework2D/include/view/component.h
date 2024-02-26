#pragma once

#include <string>

namespace USTC_CG
{
class Component
{
   public:
    Component(const std::string& label);
    Component() = default;
    virtual ~Component() = default;

    virtual void draw() = 0;

   protected:
    const std::string label = "";
};
}  // namespace USTC_CG