#pragma once
#include <string>

#include "RCore/internal/nvrhi/Resources.hpp"
#include "USTC_CG.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

ShaderHandle compile_shader(
    const std::string& entryName,
    nvrhi::ShaderType shader_type,
    std::filesystem::path shader_path,
    nvrhi::BindingLayoutDescVector& binding_layout_desc,
    std::string& error_string,
    const std::vector<ShaderMacro>& macro_defines = {},
    bool nvapi_support = false,
    bool absolute = false);

USTC_CG_NAMESPACE_CLOSE_SCOPE