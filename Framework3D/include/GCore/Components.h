#pragma once

#include "GOP.h"
#include "USTC_CG.h"
#include "Utils/Logging/Logging.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
struct GeometryComponent {
    virtual ~GeometryComponent() = default;

    explicit GeometryComponent(Geometry* attached_operand)
        : attached_operand(attached_operand)
    {
        scratch_buffer_path = pxr::SdfPath(
            "/scratch_buffer/component_" +
            std::to_string(reinterpret_cast<long long>(this)));
    }

    virtual GeometryComponentHandle copy(Geometry* operand) const = 0;
    virtual std::string to_string() const = 0;

    [[nodiscard]] Geometry* get_attached_operand() const
    {
        return attached_operand;
    }

   protected:
    Geometry* attached_operand;
    pxr::SdfPath scratch_buffer_path;
};

// DeclareComponent(OpenMeshComponent);

USTC_CG_NAMESPACE_CLOSE_SCOPE
