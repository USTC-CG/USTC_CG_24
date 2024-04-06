#include "Utils/Functions/CPPType.hpp"

#include <sstream>

#include "GCore/GOP.h"
#include "RCore/Backend.hpp"
#include "USTC_CG.h"
#include "Utils/Macro/map.h"
#include "rich_type_buffer.hpp"
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
BLI_CPP_TYPE_MAKE(pxr::UsdStageRefPtr, CPPTypeFlags::EqualityComparable)

BLI_CPP_TYPE_MAKE(MeshArray, CPPTypeFlags::EqualityComparable)
BLI_CPP_TYPE_MAKE(LightArray, CPPTypeFlags::EqualityComparable)
BLI_CPP_TYPE_MAKE(CameraArray, CPPTypeFlags::EqualityComparable)
BLI_CPP_TYPE_MAKE(MaterialMap, CPPTypeFlags::EqualityComparable)

// RESOURCES
#define WRAP_MAKE(TYPE) BLI_CPP_TYPE_MAKE(TYPE##Handle, CPPTypeFlags::None)
MACRO_MAP(WRAP_MAKE, RESOURCE_LIST)

// Little Vectors

BLI_CPP_TYPE_MAKE(pxr::GfVec2f, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(pxr::GfVec3f, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(pxr::GfVec4f, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(pxr::GfVec2i, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(pxr::GfVec3i, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(pxr::GfVec4i, CPPTypeFlags::BasicType)

// Buffers
BLI_CPP_TYPE_MAKE(pxr::VtArray<float>, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(pxr::VtArray<int>, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(pxr::VtArray<pxr::GfVec2f>, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(pxr::VtArray<pxr::GfVec3f>, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(pxr::VtArray<pxr::GfVec4f>, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(pxr::VtArray<pxr::GfVec2i>, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(pxr::VtArray<pxr::GfVec3i>, CPPTypeFlags::BasicType)
BLI_CPP_TYPE_MAKE(pxr::VtArray<pxr::GfVec4i>, CPPTypeFlags::BasicType)

void register_cpp_types()
{
    BLI_CPP_TYPE_REGISTER(float);
    BLI_CPP_TYPE_REGISTER(int32_t);
    BLI_CPP_TYPE_REGISTER(std::string);
    BLI_CPP_TYPE_REGISTER(GOperandBase);
    BLI_CPP_TYPE_REGISTER(LightArray);
    BLI_CPP_TYPE_REGISTER(pxr::UsdStageRefPtr);
    BLI_CPP_TYPE_REGISTER(MeshArray);
    BLI_CPP_TYPE_REGISTER(CameraArray);
    BLI_CPP_TYPE_REGISTER(MaterialMap);

#define WRAP_REGISTER(TYPE) BLI_CPP_TYPE_REGISTER(TYPE##Handle);

    MACRO_MAP(WRAP_REGISTER, RESOURCE_LIST);

    BLI_CPP_TYPE_REGISTER(pxr::GfVec2f);
    BLI_CPP_TYPE_REGISTER(pxr::GfVec3f);
    BLI_CPP_TYPE_REGISTER(pxr::GfVec4f);
    BLI_CPP_TYPE_REGISTER(pxr::GfVec2i);
    BLI_CPP_TYPE_REGISTER(pxr::GfVec3i);
    BLI_CPP_TYPE_REGISTER(pxr::GfVec4i);

    BLI_CPP_TYPE_REGISTER(pxr::VtArray<float>);
    BLI_CPP_TYPE_REGISTER(pxr::VtArray<int>);
    BLI_CPP_TYPE_REGISTER(pxr::VtArray<pxr::GfVec2f>);
    BLI_CPP_TYPE_REGISTER(pxr::VtArray<pxr::GfVec3f>);
    BLI_CPP_TYPE_REGISTER(pxr::VtArray<pxr::GfVec4f>);
    BLI_CPP_TYPE_REGISTER(pxr::VtArray<pxr::GfVec2i>);
    BLI_CPP_TYPE_REGISTER(pxr::VtArray<pxr::GfVec3i>);
    BLI_CPP_TYPE_REGISTER(pxr::VtArray<pxr::GfVec4i>);
}

USTC_CG_NAMESPACE_CLOSE_SCOPE