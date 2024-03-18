#pragma once
#include <cassert>
#include <iosfwd>
#include <string>
#include <type_traits>

#include "USTC_CG.h"
#include "memory_utils.hh"
#include "mixins.hpp"
#include "utildefines.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

/**
 * Different types support different features. Features like copy
 * constructability can be detected automatically easily. For some features this
 * is harder as of C++17. Those have flags in this enum and need to be
 * determined by the programmer.
 */
enum class CPPTypeFlags {
    None = 0,
    Printable = 1 << 0,
    EqualityComparable = 1 << 1,

    BasicType = Printable | EqualityComparable,
};
ENUM_OPERATORS(CPPTypeFlags, CPPTypeFlags::EqualityComparable)

/**
 * A struct that allows passing in a type as a function parameter.
 */
template<typename T>
struct TypeTag {
    using type = T;
};

/**
 * A type that encodes a specific value.
 */
template<typename T, T Element>
struct TypeForValue {
    static constexpr T value = Element;
};

class CPPType : NonCopyable, NonMovable {
   private:
    int64_t size_ = 0;
    int64_t alignment_ = 0;
    uintptr_t alignment_mask_ = 0;
    bool is_trivial_ = false;
    bool is_trivially_destructible_ = false;
    bool has_special_member_functions_ = false;

    void (*default_construct_)(void *ptr) = nullptr;

    void (*value_initialize_)(void *ptr) = nullptr;

    void (*destruct_)(void *ptr) = nullptr;

    void (*copy_assign_)(const void *src, void *dst) = nullptr;

    void (*copy_construct_)(const void *src, void *dst) = nullptr;

    void (*move_assign_)(void *src, void *dst) = nullptr;

    void (*move_construct_)(void *src, void *dst) = nullptr;

    void (*relocate_assign_)(void *src, void *dst) = nullptr;

    void (*relocate_construct_)(void *src, void *dst) = nullptr;

    void (*print_)(const void *value, std::stringstream &ss) = nullptr;
    bool (*is_equal_)(const void *a, const void *b) = nullptr;

    const void *default_value_ = nullptr;
    std::string debug_name_;

   public:
    template<typename T, CPPTypeFlags Flags>
    CPPType(
        TypeTag<T> /*type*/,
        TypeForValue<CPPTypeFlags, Flags> /*flags*/,
        const char *debug_name);
    virtual ~CPPType() = default;

    /**
     * Two types only compare equal when their pointer is equal. No two
     * instances of CPPType for the same C++ type should be created.
     */
    friend bool operator==(const CPPType &a, const CPPType &b)
    {
        return &a == &b;
    }

    friend bool operator!=(const CPPType &a, const CPPType &b)
    {
        return !(&a == &b);
    }

    /**
     * Get the `CPPType` that corresponds to a specific static type.
     * This only works for types that actually implement the template
     * specialization using `BLI_CPP_TYPE_MAKE`.
     */
    template<typename T>
    static const CPPType &get()
    {
        /* Store the #CPPType locally to avoid making the function call in most
         * cases. */
        static const CPPType &type = CPPType::get_impl<std::decay_t<T>>();
        return type;
    }
    template<typename T>
    static const CPPType &get_impl();

    /**
     * Required memory in bytes for an instance of this type.
     *
     * C++ equivalent:
     *   `sizeof(T);`
     */
    int64_t size() const
    {
        return size_;
    }

    /**
     * Required memory alignment for an instance of this type.
     *
     * C++ equivalent:
     *   alignof(T);
     */
    int64_t alignment() const
    {
        return alignment_;
    }

    /**
     * When true, the destructor does not have to be called on this type. This
     * can sometimes be used for optimization purposes.
     *
     * C++ equivalent:
     *   std::is_trivially_destructible_v<T>;
     */
    bool is_trivially_destructible() const
    {
        return is_trivially_destructible_;
    }

    /**
     * When true, the value is like a normal C type, it can be copied around
     * with #memcpy and does not have to be destructed.
     *
     * C++ equivalent:
     *   std::is_trivial_v<T>;
     */
    bool is_trivial() const
    {
        return is_trivial_;
    }

    bool is_default_constructible() const
    {
        return default_construct_ != nullptr;
    }

    bool is_copy_constructible() const
    {
        return copy_assign_ != nullptr;
    }

    bool is_move_constructible() const
    {
        return move_assign_ != nullptr;
    }

    bool is_destructible() const
    {
        return destruct_ != nullptr;
    }

    bool is_copy_assignable() const
    {
        return copy_assign_ != nullptr;
    }

    bool is_move_assignable() const
    {
        return copy_construct_ != nullptr;
    }

    bool is_printable() const
    {
        return print_ != nullptr;
    }

    bool is_equality_comparable() const
    {
        return is_equal_ != nullptr;
    }

    /**
     * Returns true, when the type has the following functions:
     * - Default constructor.
     * - Copy constructor.
     * - Move constructor.
     * - Copy assignment operator.
     * - Move assignment operator.
     * - Destructor.
     */
    bool has_special_member_functions() const
    {
        return has_special_member_functions_;
    }

    /**
     * Returns true, when the given pointer fulfills the alignment requirement
     * of this type.
     */
    bool pointer_has_valid_alignment(const void *ptr) const
    {
        return (uintptr_t(ptr) & alignment_mask_) == 0;
    }

    bool pointer_can_point_to_instance(const void *ptr) const
    {
        return ptr != nullptr && pointer_has_valid_alignment(ptr);
    }

    /**
     * Call the default constructor at the given memory location.
     * The memory should be uninitialized before this method is called.
     * For some trivial types (like int), this method does nothing.
     *
     * C++ equivalent:
     *   new (ptr) T;
     */
    void default_construct(void *ptr) const
    {
        assert(this->pointer_can_point_to_instance(ptr));

        default_construct_(ptr);
    }

    /**
     * Same as #default_construct, but does zero initialization for trivial
     * types.
     *
     * C++ equivalent:
     *   new (ptr) T();
     */
    void value_initialize(void *ptr) const
    {
        assert(this->pointer_can_point_to_instance(ptr));

        value_initialize_(ptr);
    }

    /**
     * Call the destructor on the given instance of this type. The pointer must
     * not be nullptr.
     *
     * For some trivial types, this does nothing.
     *
     * C++ equivalent:
     *   ptr->~T();
     */
    void destruct(void *ptr) const
    {
        assert(this->pointer_can_point_to_instance(ptr));

        destruct_(ptr);
    }

    /**
     * Copy an instance of this type from src to dst.
     *
     * C++ equivalent:
     *   dst = src;
     */
    void copy_assign(const void *src, void *dst) const
    {
        assert(this->pointer_can_point_to_instance(src));
        assert(this->pointer_can_point_to_instance(dst));

        copy_assign_(src, dst);
    }

    /**
     * Copy an instance of this type from src to dst.
     *
     * The memory pointed to by dst should be uninitialized.
     *
     * C++ equivalent:
     *   new (dst) T(src);
     */
    void copy_construct(const void *src, void *dst) const
    {
        assert(src != dst || is_trivial_);
        assert(this->pointer_can_point_to_instance(src));
        assert(this->pointer_can_point_to_instance(dst));

        copy_construct_(src, dst);
    }

    /**
     * Move an instance of this type from src to dst.
     *
     * The memory pointed to by dst should be initialized.
     *
     * C++ equivalent:
     *   dst = std::move(src);
     */
    void move_assign(void *src, void *dst) const
    {
        assert(this->pointer_can_point_to_instance(src));
        assert(this->pointer_can_point_to_instance(dst));

        move_assign_(src, dst);
    }

    /**
     * Move an instance of this type from src to dst.
     *
     * The memory pointed to by dst should be uninitialized.
     *
     * C++ equivalent:
     *   new (dst) T(std::move(src));
     */
    void move_construct(void *src, void *dst) const
    {
        assert(src != dst || is_trivial_);
        assert(this->pointer_can_point_to_instance(src));
        assert(this->pointer_can_point_to_instance(dst));

        move_construct_(src, dst);
    }

    /**
     * Relocates an instance of this type from src to dst. src will point to
     * uninitialized memory afterwards.
     *
     * C++ equivalent:
     *   dst = std::move(src);
     *   src->~T();
     */
    void relocate_assign(void *src, void *dst) const
    {
        assert(src != dst || is_trivial_);
        assert(this->pointer_can_point_to_instance(src));
        assert(this->pointer_can_point_to_instance(dst));

        relocate_assign_(src, dst);
    }

    /**
     * Relocates an instance of this type from src to dst. src will point to
     * uninitialized memory afterwards.
     *
     * C++ equivalent:
     *   new (dst) T(std::move(src))
     *   src->~T();
     */
    void relocate_construct(void *src, void *dst) const
    {
        assert(src != dst || is_trivial_);
        assert(this->pointer_can_point_to_instance(src));
        assert(this->pointer_can_point_to_instance(dst));

        relocate_construct_(src, dst);
    }

    bool can_exist_in_buffer(
        const int64_t buffer_size,
        const int64_t buffer_alignment) const
    {
        return size_ <= buffer_size && alignment_ <= buffer_alignment;
    }

    void print(const void *value, std::stringstream &ss) const
    {
        assert(this->pointer_can_point_to_instance(value));
        print_(value, ss);
    }

    std::string to_string(const void *value) const;

    bool is_equal(const void *a, const void *b) const
    {
        assert(this->pointer_can_point_to_instance(a));
        assert(this->pointer_can_point_to_instance(b));
        return is_equal_(a, b);
    }

    bool is_equal_or_false(const void *a, const void *b) const
    {
        if (this->is_equality_comparable()) {
            return this->is_equal(a, b);
        }
        return false;
    }

    /**
     * Get a pointer to a constant value of this type. The specific value
     * depends on the type. It is usually a zero-initialized or default
     * constructed value.
     */
    const void *default_value() const
    {
        return default_value_;
    }

    void (*destruct_fn() const)(void *)
    {
        return destruct_;
    }

    template<typename T>
    bool is() const
    {
        return this == &CPPType::get<std::decay_t<T>>();
    }

    template<typename... T>
    bool is_any() const
    {
        return (this->is<T>() || ...);
    }
};

namespace cpp_type_util {

template<typename T>
void default_construct_cb(void *ptr)
{
    new (ptr) T;
}
template<typename T>
void value_initialize_cb(void *ptr)
{
    new (ptr) T();
}

template<typename T>
void destruct_cb(void *ptr)
{
    (static_cast<T *>(ptr))->~T();
}

template<typename T>
void copy_assign_cb(const void *src, void *dst)
{
    *static_cast<T *>(dst) = *static_cast<const T *>(src);
}

template<typename T>
void copy_construct_cb(const void *src, void *dst)
{
    uninitialized_copy_n(static_cast<const T *>(src), 1, static_cast<T *>(dst));
}

template<typename T>
void move_assign_cb(void *src, void *dst)
{
    initialized_move_n(static_cast<T *>(src), 1, static_cast<T *>(dst));
}

template<typename T>
void move_construct_cb(void *src, void *dst)
{
    uninitialized_move_n(static_cast<T *>(src), 1, static_cast<T *>(dst));
}

template<typename T>
void relocate_assign_cb(void *src, void *dst)
{
    T *src_ = static_cast<T *>(src);
    T *dst_ = static_cast<T *>(dst);

    *dst_ = std::move(*src_);
    src_->~T();
}

template<typename T>
void relocate_construct_cb(void *src, void *dst)
{
    T *src_ = static_cast<T *>(src);
    T *dst_ = static_cast<T *>(dst);

    new (dst_) T(std::move(*src_));
    src_->~T();
}

template<typename T>
void fill_assign_cb(const void *value, void *dst, int64_t n)
{
    const T &value_ = *static_cast<const T *>(value);
    T *dst_ = static_cast<T *>(dst);

    for (int64_t i = 0; i < n; i++) {
        dst_[i] = value_;
    }
}

template<typename T>
void fill_construct_cb(const void *value, void *dst, int64_t n)
{
    const T &value_ = *static_cast<const T *>(value);
    T *dst_ = static_cast<T *>(dst);

    for (int64_t i = 0; i < n; i++) {
        new (dst_ + i) T(value_);
    }
}

template<typename T>
void print_cb(const void *value, std::stringstream &ss)
{
    const T &value_ = *static_cast<const T *>(value);
    ss << value_;
}

template<typename T>
bool is_equal_cb(const void *a, const void *b)
{
    const T &a_ = *static_cast<const T *>(a);
    const T &b_ = *static_cast<const T *>(b);
    return a_ == b_;
}


}  // namespace cpp_type_util

template<typename T, CPPTypeFlags Flags>
CPPType::CPPType(
    TypeTag<T> /*type*/,
    TypeForValue<CPPTypeFlags, Flags> /*flags*/,
    const char *debug_name)
{
    using namespace cpp_type_util;

    debug_name_ = debug_name;
    size_ = int64_t(sizeof(T));
    alignment_ = int64_t(alignof(T));
    is_trivial_ = std::is_trivial_v<T>;
    is_trivially_destructible_ = std::is_trivially_destructible_v<T>;
    if constexpr (std::is_default_constructible_v<T>) {
        default_construct_ = default_construct_cb<T>;
        value_initialize_ = value_initialize_cb<T>;
        static T default_value;
        default_value_ = &default_value;
    }
    if constexpr (std::is_destructible_v<T>) {
        destruct_ = destruct_cb<T>;
    }
    if constexpr (std::is_copy_assignable_v<T>) {
        copy_assign_ = copy_assign_cb<T>;
    }
    if constexpr (std::is_copy_constructible_v<T>) {
        copy_construct_ = copy_construct_cb<T>;
    }
    if constexpr (std::is_move_assignable_v<T>) {
        move_assign_ = move_assign_cb<T>;
    }
    if constexpr (std::is_move_constructible_v<T>) {
        move_construct_ = move_construct_cb<T>;
    }
    if constexpr (std::is_destructible_v<T>) {
        if constexpr (std::is_move_assignable_v<T>) {
            relocate_assign_ = relocate_assign_cb<T>;
        }
        if constexpr (std::is_move_constructible_v<T>) {
            relocate_construct_ = relocate_construct_cb<T>;
        }
    }
    if constexpr ((bool)(Flags & CPPTypeFlags::Printable)) {
        print_ = print_cb<T>;
    }
    if constexpr ((bool)(Flags & CPPTypeFlags::EqualityComparable)) {
        is_equal_ = is_equal_cb<T>;
    }

    alignment_mask_ = uintptr_t(alignment_) - uintptr_t(1);
    has_special_member_functions_ =
        (default_construct_ && copy_construct_ && copy_assign_ &&
         move_construct_ && move_assign_ && destruct_);
}

/**
 * Initialize and register basic cpp types.
 */
void register_cpp_types();

USTC_CG_NAMESPACE_CLOSE_SCOPE
