#pragma once

#include "Nodes/all_socket_types.hpp"
#include "Nodes/make_standard_type.hpp"
#include "Nodes/pin.hpp"
#include "USTC_CG.h"
#include "Utils/Macro/map.h"
#include "basic_socket_types.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
namespace decl {

MACRO_MAP(DECLARE_SOCKET_TYPE, STAGE_SOCKET_TYPES)

}  // namespace decl

USTC_CG_NAMESPACE_CLOSE_SCOPE