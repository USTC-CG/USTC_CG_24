#pragma once
#include "CPPType.hpp"
#include "USTC_CG.h"
USTC_CG_NAMESPACE_OPEN_SCOPE

/**
 * A generic non-const pointer whose type is only known at runtime.
 */
class GMutablePointer {
   private:
    const CPPType *type_ = nullptr;
    void *data_ = nullptr;

   public:
    GMutablePointer() = default;

    GMutablePointer(const CPPType *type, void *data = nullptr) : type_(type), data_(data)
    {
        /* If there is data, there has to be a type. */
        assert(data_ == nullptr || type_ != nullptr);
    }

    GMutablePointer(const CPPType &type, void *data = nullptr) : GMutablePointer(&type, data)
    {
    }

    template<typename T>
    GMutablePointer(T *data) : GMutablePointer(&CPPType::get<T>(), data)
    {
    }

    void *get() const
    {
        return data_;
    }

    // this is dangerous. call it cautiously
    void set_type(const CPPType *type)
    {
        type_ = type;
    }

    const CPPType *type() const
    {
        return type_;
    }

    template<typename T>
    T *get() const
    {
        assert(this->is_type<T>());
        return static_cast<T *>(data_);
    }

    template<typename T>
    bool is_type() const
    {
        return type_ != nullptr && type_->is<T>();
    }

    template<typename T>
    T relocate_out()
    {
        assert(this->is_type<T>());
        T value;
        type_->relocate_assign(data_, &value);
        data_ = nullptr;
        type_ = nullptr;
        return value;
    }

    void destruct()
    {
        assert(data_ != nullptr);
        type_->destruct(data_);
    }

    void default_construct()
    {
        assert(data_ != nullptr);
        type_->default_construct(data_);
    }
};

/**
 * A generic const pointer whose type is only known at runtime.
 */
class GPointer {
   private:
    const CPPType *type_ = nullptr;
    const void *data_ = nullptr;

   public:
    GPointer() = default;

    GPointer(GMutablePointer ptr) : type_(ptr.type()), data_(ptr.get())
    {
    }

    GPointer(const CPPType *type, const void *data = nullptr) : type_(type), data_(data)
    {
        /* If there is data, there has to be a type. */
        assert(data_ == nullptr || type_ != nullptr);
    }

    GPointer(const CPPType &type, const void *data = nullptr) : type_(&type), data_(data)
    {
    }

    template<typename T>
    GPointer(T *data) : GPointer(&CPPType::get<T>(), data)
    {
    }

    const void *get() const
    {
        return data_;
    }

    const CPPType *type() const
    {
        return type_;
    }

    template<typename T>
    const T *get() const
    {
        assert(this->is_type<T>());
        return static_cast<const T *>(data_);
    }

    template<typename T>
    bool is_type() const
    {
        return type_ != nullptr && type_->is<T>();
    }
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
