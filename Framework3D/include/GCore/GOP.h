#pragma once

#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/stage.h>

#include <memory>
#include <string>

#include "USTC_CG.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
struct GeometryComponent;
class Geometry;
using GeometryHandle = std::shared_ptr<Geometry>;
using GeometryComponentHandle = std::shared_ptr<GeometryComponent>;

class USTC_CG_API Geometry {
   public:
    Geometry();

    virtual ~Geometry();

    Geometry(const Geometry& operand);
    Geometry(Geometry&& operand) noexcept;

    Geometry& operator=(const Geometry& operand);
    Geometry& operator=(Geometry&& operand) noexcept;

    static Geometry CreateMesh();

    friend bool operator==(const Geometry& lhs, const Geometry& rhs)
    {
        return lhs.components_ == rhs.components_;
        //&& lhs.stage == rhs.stage;
    }

    friend bool operator!=(const Geometry& lhs, const Geometry& rhs)
    {
        return !(lhs == rhs);
    }

    virtual std::string to_string() const;

    template<typename OperandType>
    std::shared_ptr<OperandType> get_component(size_t idx = 0) const;
    void attach_component(const GeometryComponentHandle& component);
    void detach_component(const GeometryComponentHandle& component);

    [[nodiscard]] const std::vector<GeometryComponentHandle>& get_components()
        const
    {
        return components_;
    }

   protected:
    std::vector<GeometryComponentHandle> components_;
};

template<typename OperandType>
std::shared_ptr<OperandType> Geometry::get_component(size_t idx) const
{
    size_t counter = 0;
    for (int i = 0; i < components_.size(); ++i) {
        auto ptr = std::dynamic_pointer_cast<OperandType>(components_[i]);
        if (ptr) {
            if (counter < idx) {
                counter++;
            }
            else {
                return ptr;
            }
        }
    }
    return nullptr;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
