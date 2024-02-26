#pragma once

#include "shape.h"

namespace USTC_CG
{
class Rect : public Shape
{
   public:
    Rect();
    ~Rect();

    void draw(const Settings& settings) override;
};
}  // namespace USTC_CG
