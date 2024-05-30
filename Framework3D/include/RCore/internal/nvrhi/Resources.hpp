#pragma once

#include <wrl.h>

#include <filesystem>

#include "USTC_CG.h"
#include "Utils/Macro/map.h"
#include "nvrhi/nvrhi.h"

namespace nvrhi {
using CommandListDesc = nvrhi::CommandListParameters;
}
struct IDxcBlob;
USTC_CG_NAMESPACE_OPEN_SCOPE
struct ShaderCompileResult;
struct ShaderCompileDesc;
#define USING_NVRHI_SYMBOL(RESOURCE) \
    using nvrhi::RESOURCE##Desc;     \
    using nvrhi::RESOURCE##Handle;

#define USING_NVRHI_RT_SYMBOL(RESOURCE) \
    using nvrhi::rt::RESOURCE##Desc;    \
    using nvrhi::rt::RESOURCE##Handle;

#define NVRHI_RESOURCE_LIST    Texture, Shader, Buffer, BindingLayout, BindingSet, CommandList
#define NVRHI_RT_RESOURCE_LIST Pipeline, AccelStruct
#define RESOURCE_LIST          NVRHI_RESOURCE_LIST, NVRHI_RT_RESOURCE_LIST, ShaderCompile

using nvrhi::BindingLayoutDesc;
using nvrhi::BindingLayoutHandle;
using nvrhi::BindingSetDesc;
using nvrhi::BindingSetHandle;
using nvrhi::BufferDesc;
using nvrhi::BufferHandle;
using nvrhi::CommandListDesc;
using nvrhi::CommandListHandle;
using nvrhi::ShaderDesc;
using nvrhi::ShaderHandle;
using nvrhi::TextureDesc;
using nvrhi::TextureHandle;
;
MACRO_MAP(USING_NVRHI_RT_SYMBOL, NVRHI_RT_RESOURCE_LIST);

using ShaderCompileHandle = std::shared_ptr<ShaderCompileResult>;

struct ShaderCompileResult {
    void* getBufferPointer() const;
    size_t getBufferSize() const;

    [[nodiscard]] const std::string& get_error_string() const
    {
        return error_string;
    }

   private:
    friend ShaderCompileHandle createShaderCompile(const ShaderCompileDesc& desc);

    Microsoft::WRL::ComPtr<IDxcBlob> blob;
    std::string error_string;
};

struct ShaderCompileDesc {
    friend bool operator==(const ShaderCompileDesc& lhs, const ShaderCompileDesc& rhs)
    {
        return lhs.path == rhs.path && lhs.entry_name == rhs.entry_name &&
               lhs.lastWriteTime == rhs.lastWriteTime;
    }

    friend bool operator!=(const ShaderCompileDesc& lhs, const ShaderCompileDesc& rhs)
    {
        return !(lhs == rhs);
    }

    void set_path(const std::filesystem::path& path);
    void set_shader_type(nvrhi::ShaderType shaderType);
    void set_entry_name(const std::string& entry_name);

    nvrhi::ShaderType shaderType;

   private:
    void update_last_write_time(const std::filesystem::path& path);

    std::string get_profile() const;
    friend ShaderCompileHandle createShaderCompile(const ShaderCompileDesc& desc);
    std::filesystem::path path;
    std::filesystem::file_time_type lastWriteTime;
    std::string entry_name;
};

ShaderCompileHandle createShaderCompile(const ShaderCompileDesc& desc);

USTC_CG_NAMESPACE_CLOSE_SCOPE

#include "nvrhi_equality.hpp"