#pragma once
#include <RCore/internal/resources.hpp>

#include "USTC_CG.h"
#include "Utils/Macro/map.h"
#include "nvrhi/nvrhi.h"
USTC_CG_NAMESPACE_OPEN_SCOPE
#define USING_NVRHI_SYMBOL(RESOURCE) \
    using nvrhi::RESOURCE##Desc;     \
    using nvrhi::RESOURCE##Handle;

#define USING_NVRHI_RT_SYMBOL(RESOURCE) \
    using nvrhi::rt::RESOURCE##Desc;    \
    using nvrhi::rt::RESOURCE##Handle;

#define NVRHI_RESOURCE_LIST    Texture, Shader, Buffer, BindingLayout, BindingSet
#define NVRHI_RT_RESOURCE_LIST Pipeline, AccelStruct
#define RESOURCE_LIST          NVRHI_RESOURCE_LIST, NVRHI_RT_RESOURCE_LIST

MACRO_MAP(USING_NVRHI_SYMBOL, NVRHI_RESOURCE_LIST);
MACRO_MAP(USING_NVRHI_RT_SYMBOL, NVRHI_RT_RESOURCE_LIST);

USTC_CG_NAMESPACE_CLOSE_SCOPE

#include "nvrhi_equality.hpp"
