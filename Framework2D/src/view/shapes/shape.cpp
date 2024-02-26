#include "view/shapes/shape.h"

namespace USTC_CG
{
Shape::Shape()
{
}

Shape::~Shape()
{
}

void Shape::set_start(const ImVec2& s)
{
    start_point = s;
}

void Shape::set_end(const ImVec2& e)
{
    end_point = e;
}

}  // namespace USTC_CG