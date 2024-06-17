#pragma once

#include <wrl.h>

#include <filesystem>

#include "USTC_CG.h"
#include "Utils/Macro/map.h"
#include "nvrhi/nvrhi.h"
#include "nvrhi_format.hpp"
#include "slang-com-ptr.h"

namespace nvrhi {
using CommandListDesc = nvrhi::CommandListParameters;
typedef static_vector<BindingLayoutDesc, c_MaxBindingLayouts>
    BindingLayoutDescVector;

struct StagingTextureDesc : public nvrhi::TextureDesc { };

struct CPUBuffer {
    void* data;

    ~CPUBuffer()
    {
        delete[] data;
    }
};

struct CPUBufferDesc {
    size_t size;
};

using CPUBufferHandle = std::shared_ptr<CPUBuffer>;
}  // namespace nvrhi
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

#define NVRHI_RESOURCE_LIST                                                   \
    Texture, Sampler, Framebuffer, Shader, Buffer, BindingLayout, BindingSet, \
        CommandList, StagingTexture, ComputePipeline, GraphicsPipeline
#define NVRHI_RT_RESOURCE_LIST Pipeline, AccelStruct
#define RESOURCE_LIST          NVRHI_RESOURCE_LIST, NVRHI_RT_RESOURCE_LIST, ShaderCompile

MACRO_MAP(USING_NVRHI_SYMBOL, NVRHI_RESOURCE_LIST);
MACRO_MAP(USING_NVRHI_RT_SYMBOL, NVRHI_RT_RESOURCE_LIST);

using ShaderCompileHandle = nvrhi::RefCountPtr<ShaderCompileResult>;

class IShaderCompileResult : public nvrhi::IResource {
   public:
    virtual void const* getBufferPointer() const = 0;
    virtual size_t getBufferSize() const = 0;
    virtual [[nodiscard]] const std::string& get_error_string() const = 0;
    virtual [[nodiscard]] const nvrhi::BindingLayoutDescVector&
    get_binding_layout() const = 0;
};

struct ShaderCompileResult : nvrhi::RefCounter<IShaderCompileResult> {
    void const* getBufferPointer() const override;
    size_t getBufferSize() const override;

    [[nodiscard]] const std::string& get_error_string() const override
    {
        return error_string;
    }

    [[nodiscard]] const nvrhi::BindingLayoutDescVector& get_binding_layout()
        const override
    {
        return binding_layout_;
    }

   private:
    friend ShaderCompileHandle createShaderCompile(
        const ShaderCompileDesc& desc);

    nvrhi::BindingLayoutDescVector binding_layout_;
    Slang::ComPtr<ISlangBlob> blob;
    std::string error_string;
};

struct ShaderMacro {
    std::string name;
    std::string definition;

    ShaderMacro(const std::string& _name, const std::string& _definition)
        : name(_name),
          definition(_definition)
    {
    }
};

struct ShaderCompileDesc {
    friend bool operator==(
        const ShaderCompileDesc& lhs,
        const ShaderCompileDesc& rhs)
    {
        return lhs.path == rhs.path && lhs.entry_name == rhs.entry_name &&
               lhs.lastWriteTime == rhs.lastWriteTime;
    }

    friend bool operator!=(
        const ShaderCompileDesc& lhs,
        const ShaderCompileDesc& rhs)
    {
        return !(lhs == rhs);
    }

    void define(std::string macro, std::string value)
    {
        macros.push_back(ShaderMacro(macro, value));
    }
    void set_path(const std::filesystem::path& path);
    void set_shader_type(nvrhi::ShaderType shaderType);
    void set_entry_name(const std::string& entry_name);

    nvrhi::ShaderType shaderType;

   private:
    void update_last_write_time(const std::filesystem::path& path);
    std::vector<ShaderMacro> macros;
    std::string get_profile() const;
    friend ShaderCompileHandle createShaderCompile(
        const ShaderCompileDesc& desc);
    std::filesystem::path path;
    std::filesystem::file_time_type lastWriteTime;
    std::string entry_name;
};

ShaderCompileHandle createShaderCompile(const ShaderCompileDesc& desc);

// Function to merge two BindingLayoutDescVector objects
nvrhi::BindingLayoutDescVector mergeBindingLayoutDescVectors(
    const nvrhi::BindingLayoutDescVector& vec1,
    const nvrhi::BindingLayoutDescVector& vec2);

USTC_CG_NAMESPACE_CLOSE_SCOPE

#include "nvrhi_equality.hpp"