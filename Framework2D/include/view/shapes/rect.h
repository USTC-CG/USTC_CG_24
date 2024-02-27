#pragma once

#include "shape.h"

namespace USTC_CG
{
class Rect : public Shape
{
   public:
    Rect() = default;

    // Initialize a rectangle with start and end points
    Rect(
        float start_point_x,
        float start_point_y,
        float end_point_x,
        float end_point_y)
        : start_point_x_(start_point_x),
          start_point_y_(start_point_y),
          end_point_x_(end_point_x),
          end_point_y_(end_point_y)
    {
    }

    virtual ~Rect() = default;

    // Draws the rectangle on the screen
    // Overrides draw function to implement rectangle-specific drawing logic
    void draw(const Config& config) const override;

    // Overrides Shape's update function to adjust the rectangle size during
    // interaction
    void update(float x, float y) override;

   private:
    // Coordinates of the top-left and bottom-right corners of the rectangle
    float start_point_x_ = 0.0f, start_point_y_ = 0.0f;
    float end_point_x_ = 0.0f, end_point_y_ = 0.0f;
};
}  // namespace USTC_CG
