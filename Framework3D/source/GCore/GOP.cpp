#include "GCore/GOP.h"

#include "GCore/Components.h"
#include "GCore/Components/MeshOperand.h"
#include "pxr/usd/usdGeom/xform.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
Geometry::Geometry()
{
}

Geometry::~Geometry()
{
}

Geometry::Geometry(const Geometry& operand)
{
    *(this) = operand;
}

Geometry::Geometry(Geometry&& operand) noexcept
{
    *(this) = std::move(operand);
}

Geometry& Geometry::operator=(const Geometry& operand)
{
    for (auto&& operand_component : operand.components_) {
        this->components_.push_back(operand_component->copy(this));
    }

    return *this;
}

Geometry& Geometry::operator=(Geometry&& operand) noexcept
{
    this->components_ = std::move(operand.components_);
    return *this;
}

Geometry Geometry::CreateMesh()
{
    Geometry geometry;
    std::shared_ptr<MeshComponent> mesh =
        std::make_shared<MeshComponent>(&geometry);
    geometry.attach_component(mesh);
    return std::move(geometry);
}

std::string Geometry::to_string() const
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

void Geometry::attach_component(const GeometryComponentHandle& component)
{
    if (component->get_attached_operand() != this) {
        logging(
            "A component should never be attached to two operands, unless you "
            "know what you are doing",
            Warning);
    }
    components_.push_back(component);
}

void Geometry::detach_component(const GeometryComponentHandle& component)
{
    auto iter = std::find(components_.begin(), components_.end(), component);
    components_.erase(iter);
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
