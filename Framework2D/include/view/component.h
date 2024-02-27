#pragma once

#include <string>

namespace USTC_CG
{

class Component
{
   public:
    explicit Component(const std::string& label) : label_(label)
    {
    }
    virtual ~Component() = default;

    virtual void draw() = 0;

   protected:
    std::string label_;
};

}  // namespace USTC_CG
