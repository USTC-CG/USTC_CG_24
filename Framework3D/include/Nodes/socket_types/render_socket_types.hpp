#pragma once

#include "USTC_CG.h"
#include "Utils/Macro/map.h"
#include "Nodes/all_socket_types.hpp"
#include "Nodes/make_standard_type.hpp"
#include "Nodes/pin.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
namespace decl {

MACRO_MAP(DECLARE_SOCKET_TYPE, RENDER_SOCKET_TYPES)

}  // namespace decl

USTC_CG_NAMESPACE_CLOSE_SCOPE