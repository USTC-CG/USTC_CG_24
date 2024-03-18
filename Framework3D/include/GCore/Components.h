#pragma once

#include "GOP.h"
#include "USTC_CG.h"
#include "Utils/Logging/Logging.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
struct GOperandComponent {
    virtual ~GOperandComponent() = default;

    explicit GOperandComponent(GOperandBase* attached_operand) : attached_operand(attached_operand)
    {
    }

    virtual GOperandComponentHandle copy(GOperandBase* operand) const = 0;
    virtual std::string to_string() const = 0;

    virtual std::string name() const
    {
        return { "Base GOperandComponent" };
    }


    [[nodiscard]] GOperandBase* get_attached_operand() const
    {
        return attached_operand;
    }

   protected:
    GOperandBase* attached_operand;
};

// DeclareComponent(OpenMeshComponent);

USTC_CG_NAMESPACE_CLOSE_SCOPE
