#include "GCore/GOP.h"

#include "GCore/Components.h"
#include "pxr/usd/usdGeom/xform.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
GOperandBase::GOperandBase(const GOperandBase& operand)
{
    *(this) = operand;
}

GOperandBase::GOperandBase(GOperandBase&& operand) noexcept
{
    *(this) = std::move(operand);
}

GOperandBase& GOperandBase::operator=(const GOperandBase& operand)
{
    for (auto&& operand_component : operand.components_) {
        this->components_.push_back(operand_component->copy(this));
    }

    return *this;
}

GOperandBase& GOperandBase::operator=(GOperandBase&& operand) noexcept
{
    this->components_ = std::move(operand.components_);
    //this->stage = operand.stage;
    //operand.stage.Reset();

    return *this;
}

void GOperandBase::copy_to(GOperandBaseHandle handle)
{
    for (auto&& component : components_) {
        auto cp_component = component->copy(handle.get());
        handle->attach_component(cp_component);
    }

    //handle->stage = stage;
}

std::string GOperandBase::to_string() const
{
    std::ostringstream out;
    out << "Contains components:\n";
    for (auto&& component : components_) {
        if (component) {
            out << "    " << component->to_string() << "\n";
        }
    }
    return out.str();
}

void GOperandBase::attach_component(const GOperandComponentHandle& component)
{
    if (component->get_attached_operand() != this) {
        logging(
            "A component should never be attached to two operands, unless you "
            "know what you are doing",
            Warning);
    }
    components_.push_back(component);
}

void GOperandBase::detach_component(const GOperandComponentHandle& component)
{
    auto iter = std::find(components_.begin(), components_.end(), component);
    components_.erase(iter);
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
