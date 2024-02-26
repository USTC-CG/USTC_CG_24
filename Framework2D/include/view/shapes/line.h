#pragma once

#include "shape.h"

namespace USTC_CG
{
class Line : public Shape
{
   public:
    Line();
    ~Line();

    void draw(const Settings& settings) override;
};
}
