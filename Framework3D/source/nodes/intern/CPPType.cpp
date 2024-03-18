#include "Utils/Functions/CPPType.hpp"

#include <sstream>

#include "GCore/GOP.h"
#include "USTC_CG.h"
USTC_CG_NAMESPACE_OPEN_SCOPE
/** Create a new #CPPType that can be accessed through `CPPType::get<T>()`. */
#define BLI_CPP_TYPE_MAKE(TYPE_NAME, FLAGS)                       \
    template<>                                                    \
    const CPPType &CPPType::get_impl<TYPE_NAME>()                 \
    {                                                             \
        static CPPType type{ TypeTag<TYPE_NAME>(),                \
                             TypeForValue<CPPTypeFlags, FLAGS>(), \
                             STRINGIFY(TYPE_NAME) };              \
        return type;                                              \
    }

/** Register a #CPPType created with #BLI_CPP_TYPE_MAKE. */
#define BLI_CPP_TYPE_REGISTER(TYPE_NAME) CPPType::get<TYPE_NAME>()

BLI_CPP_TYPE_MAKE(float, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(int32_t, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(std::string, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(GOperandBase, CPPTypeFlags::EqualityComparable)


// Buffers
BLI_CPP_TYPE_MAKE(pxr::VtArray<float>, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(pxr::VtArray<pxr::GfVec2f>, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(pxr::VtArray<pxr::GfVec3f>, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(pxr::VtArray<pxr::GfVec4f>, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(pxr::VtArray<int>, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(pxr::VtArray<pxr::GfVec2i>, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(pxr::VtArray<pxr::GfVec3i>, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(pxr::VtArray<pxr::GfVec4i>, CPPTypeFlags::BasicType)


void register_cpp_types()
{
    BLI_CPP_TYPE_REGISTER(float);
    BLI_CPP_TYPE_REGISTER(int32_t);
    BLI_CPP_TYPE_REGISTER(std::string);
    BLI_CPP_TYPE_REGISTER(GOperandBase);

    BLI_CPP_TYPE_REGISTER(pxr::VtArray<float>);
    BLI_CPP_TYPE_REGISTER(pxr::VtArray<pxr::GfVec2f>);
    BLI_CPP_TYPE_REGISTER(pxr::VtArray<pxr::GfVec3f>);
    BLI_CPP_TYPE_REGISTER(pxr::VtArray<pxr::GfVec4f>);
    BLI_CPP_TYPE_REGISTER(pxr::VtArray<int>);
    BLI_CPP_TYPE_REGISTER(pxr::VtArray<pxr::GfVec2i>);
    BLI_CPP_TYPE_REGISTER(pxr::VtArray<pxr::GfVec3i>);
    BLI_CPP_TYPE_REGISTER(pxr::VtArray<pxr::GfVec4i>);
}

USTC_CG_NAMESPACE_CLOSE_SCOPE