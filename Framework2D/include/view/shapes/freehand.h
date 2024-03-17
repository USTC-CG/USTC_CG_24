#pragma once

#include <iostream>
#include <vector>

#include "shape.h"

namespace USTC_CG
{

class Freehand : public Shape
{
   public:
    Freehand() = default;

    // Constructor to initialize a Freehand with start and end coordinates
    Freehand(std::vector<point> points) : points(points)
    {
    }

    virtual ~Freehand() = default;

    // Overrides draw function to implement Freehand-specific drawing logic
    void draw(const Config& config) const override;

    // Overrides Shape's update function to adjust the end point during
    // interaction
    void update(float x, float y) override;

    // Overrides Shape's set_index function to set the index of the line in
    // Freehand
    void set_index(int index) override;

    // Overrides Shape's add_point function to add a point to the Freehand
    void add_point(float x, float y) override;

    // Overrides Shape's get_index function to get the index of the line in
    // Freehand
    int get_index() override;

   private:
    std::vector<point> points{};
};
}  // namespace USTC_CG