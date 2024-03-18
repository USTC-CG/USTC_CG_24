#pragma once

#include"USTC_CG.h"

#include <memory>
#include <string>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/stage.h>

USTC_CG_NAMESPACE_OPEN_SCOPE
struct GOperandComponent;
class GOperandBase;
using GOperandHandle = std::shared_ptr<GOperandBase>;
using GOperandComponentHandle = std::shared_ptr<GOperandComponent>;

using GOperandBaseHandle = std::shared_ptr<GOperandBase>;

class USTC_CG_API GOperandBase
{
public:
    GOperandBase()
    {
    }

    virtual ~GOperandBase() = default;

    GOperandBase(const GOperandBase& operand);
    GOperandBase(GOperandBase&& operand) noexcept;

    GOperandBase& operator=(const GOperandBase& operand);
    GOperandBase& operator=(GOperandBase&& operand) noexcept;


    friend bool operator==(const GOperandBase& lhs, const GOperandBase& rhs)
    {
        return lhs.components_ == rhs.components_;
               //&& lhs.stage == rhs.stage;
    }

    friend bool operator!=(const GOperandBase& lhs, const GOperandBase& rhs)
    {
        return !(lhs == rhs);
    }

    virtual void copy_to(GOperandBaseHandle handle);

    virtual std::string to_string() const;


    template<typename OperandType>
    std::shared_ptr<OperandType> get_component(size_t idx = 0) const;
    void attach_component(const GOperandComponentHandle& component);
    void detach_component(const GOperandComponentHandle& component);

    [[nodiscard]] const std::vector<GOperandComponentHandle>&
    get_components() const
    {
        return components_;
    }

protected:
    std::vector<GOperandComponentHandle> components_;
};

template<typename OperandType>
std::shared_ptr<OperandType> GOperandBase::get_component(size_t idx) const
{
    size_t counter = 0;
    for (int i = 0; i < components_.size(); ++i)
    {
        auto ptr = std::dynamic_pointer_cast<OperandType>(components_[i]);
        if (ptr)
        {
            if (counter < idx)
            {
                counter++;
            }
            else
            {
                return ptr;
            }
        }
    }
    return nullptr;
}


USTC_CG_NAMESPACE_CLOSE_SCOPE
