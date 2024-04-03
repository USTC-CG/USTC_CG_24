#pragma once

#include "USTC_CG.h"
#include "make_standard_type.hpp"
#include "Utils/Macro/map.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
namespace decl {

DECLARE_SOCKET_TYPE(Geometry)

// Here we don't use a template style since there are not many types following
// this style... Later we may have all kinds of images and shaders.

class IntBuilder;

class Int : public SocketDeclaration {
   public:
    Int()
    {
        type = SocketType::Int;
    }

    NodeSocket* build(NodeTree* ntree, Node* node) const override;

    void update_default_value(NodeSocket* socket) const override;

    using Builder = IntBuilder;
    // TODO: Throw error on mac.
    // using DefaultValueType = bNodeSocketValue;

    int soft_min = INT_MIN / 2;
    int soft_max = INT_MAX / 2;
    int default_value_ = 0;
};

class IntBuilder : public SocketDeclarationBuilder<Int> {
   public:
    IntBuilder& min(int val)
    {
        decl_->soft_min = val;
        return *this;
    }

    IntBuilder& max(int val)
    {
        decl_->soft_max = val;
        return *this;
    }

    IntBuilder& default_val(int val)
    {
        decl_->default_value_ = val;
        return *this;
    }
};

class FloatBuilder;

class Float : public SocketDeclaration {
   public:
    Float()
    {
        type = SocketType::Float;
    }

    NodeSocket* build(NodeTree* ntree, Node* node) const override;

    void update_default_value(NodeSocket* socket) const override;

    using Builder = FloatBuilder;

    float soft_min = -FLT_MAX / 2.0f;
    float soft_max = FLT_MAX / 2.0f;
    float default_value_ = 0;
};

class FloatBuilder : public SocketDeclarationBuilder<Float> {
   public:
    FloatBuilder& min(float val)
    {
        decl_->soft_min = val;
        return *this;
    }

    FloatBuilder& max(float val)
    {
        decl_->soft_max = val;
        return *this;
    }

    FloatBuilder& default_val(float val)
    {
        decl_->default_value_ = val;
        return *this;
    }
};

class StringBuilder;

class String : public SocketDeclaration {
   public:
    String()
    {
        type = SocketType::String;
    }

    NodeSocket* build(NodeTree* ntree, Node* node) const override;

    void update_default_value(NodeSocket* socket) const override;

    using Builder = StringBuilder;

    std::string default_value_ = {};
};

class StringBuilder : public SocketDeclarationBuilder<String> {
   public:
    StringBuilder& default_val(const std::string& val)
    {
        decl_->default_value_ = val;
        return *this;
    }
};

#define BUFFER_TYPES                                                                          \
    Int1Buffer, Int2Buffer, Int3Buffer, Int4Buffer, Float1Buffer, Float2Buffer, Float3Buffer, \
        Float4Buffer

MACRO_MAP(DECLARE_SOCKET_TYPE, BUFFER_TYPES)

#define GEO_SOCKET_TYPES Geometry, Int, String, Float, BUFFER_TYPES

}  // namespace decl

USTC_CG_NAMESPACE_CLOSE_SCOPE