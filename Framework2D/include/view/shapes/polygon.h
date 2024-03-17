#pragma once

#include <iostream>
#include <vector>

#include "shape.h"

namespace USTC_CG
{

class Polygon : public Shape
{
   public:
    Polygon() = default;

    // Constructor to initialize a Polygon with start and end coordinates
    Polygon(std::vector<point> points) : points(points)
    {
    }

    virtual ~Polygon() = default;

    // Overrides draw function to implement Polygon-specific drawing logic
    void draw(const Config& config) const override;

    // Overrides Shape's update function to adjust the end point during
    // interaction
    void update(float x, float y) override;

    // Overrides Shape's set_index function to set the index of the line in
    // polygon
    void set_index(int index) override;

    // Overrides Shape's add_point function to add a point to the polygon
    void add_point(float x, float y) override;

    // Overrides Shape's get_index function to get the index of the line in
    // polygon
    int get_index() override;

   private:
    std::vector<point> points{};
};
}  // namespace USTC_CG