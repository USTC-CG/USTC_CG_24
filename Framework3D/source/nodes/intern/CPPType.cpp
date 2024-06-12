#include "Utils/Functions/CPPType.hpp"

#include <any>
#include <entt/core/hashed_string.hpp>
#include <entt/core/utility.hpp>
#include <entt/entity/registry.hpp>
#include <entt/locator/locator.hpp>
#include <entt/meta/context.hpp>
#include <entt/meta/factory.hpp>
#include <sstream>

#include "GCore/GOP.h"
#include "RCore/Backend.hpp"
#include "USTC_CG.h"
#include "Utils/Macro/map.h"
#include "rich_type_buffer.hpp"
#include "Nodes/node_socket.hpp"
#include "boost/python/numpy.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
namespace node_mass_spring {
class MassSpring;
}
namespace node_sph_fluid {
class SPHBase;
}
namespace node_character_animation {
class Animator;
}

/** Register a #CPPType created with #BLI_CPP_TYPE_MAKE. */
#define BLI_CPP_TYPE_REGISTER(TYPE_NAME) entt::meta<TYPE_NAME>().type(#TYPE_NAME##_hs)

void register_cpp_types()
{
    using namespace entt::literals;
    BLI_CPP_TYPE_REGISTER(float);
    BLI_CPP_TYPE_REGISTER(int32_t);
    BLI_CPP_TYPE_REGISTER(std::string);
    BLI_CPP_TYPE_REGISTER(entt::any);
    BLI_CPP_TYPE_REGISTER(boost::python::object);
    BLI_CPP_TYPE_REGISTER(boost::python::numpy::ndarray);
    BLI_CPP_TYPE_REGISTER(GOperandBase);
    BLI_CPP_TYPE_REGISTER(LightArray);
    BLI_CPP_TYPE_REGISTER(pxr::UsdStageRefPtr);
    BLI_CPP_TYPE_REGISTER(MeshArray);
    BLI_CPP_TYPE_REGISTER(CameraArray);
    BLI_CPP_TYPE_REGISTER(MaterialMap);
    BLI_CPP_TYPE_REGISTER(SocketGroup);
    BLI_CPP_TYPE_REGISTER(std::shared_ptr<node_mass_spring::MassSpring>);
    BLI_CPP_TYPE_REGISTER(std::shared_ptr<node_sph_fluid::SPHBase>);
    BLI_CPP_TYPE_REGISTER(std::shared_ptr<node_character_animation::Animator>);

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