#pragma once

#include "USTC_CG.h"
#include "pin.hpp"
#include "make_standard_type.hpp"
#include "Utils/Macro/map.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
namespace decl {

#define STAGE_SOCKET_TYPES Layer
MACRO_MAP(DECLARE_SOCKET_TYPE, STAGE_SOCKET_TYPES)

}  // namespace decl

USTC_CG_NAMESPACE_CLOSE_SCOPE