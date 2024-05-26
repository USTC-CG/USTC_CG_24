#pragma once
#include "USTC_CG.h"
#include "Utils/Macro/map.h"
#include <RCore/internal/resources.hpp>
#include "nvrhi/nvrhi.h"
USTC_CG_NAMESPACE_OPEN_SCOPE
#define USING_NVRHI_SYMBOL(RESOURCE) \
    using nvrhi::RESOURCE##Desc;     \
    using nvrhi::RESOURCE##Handle;
#define RESOURCE_LIST Texture, Shader

MACRO_MAP(USING_NVRHI_SYMBOL, RESOURCE_LIST);


USTC_CG_NAMESPACE_CLOSE_SCOPE

#include "nvrhi_equality.hpp"
