#pragma once

#include <filesystem>

#include "USTC_CG.h"
#include "Utils/Logging/Logging.h"
#include "pxr/base/gf/vec2i.h"
#include "pxr/imaging/garch/glApi.h"
#include "pxr/imaging/hd/types.h"
#include "pxr/imaging/hio/types.h"
#include "shader.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
#define RESOURCE_LIST Texture, Shader

////////////////////////////////Shader/////////////////////////////////////////

struct ShaderResource;
using ShaderHandle = std::shared_ptr<ShaderResource>;

struct ShaderDesc {
    friend bool operator==(const ShaderDesc& lhs, const ShaderDesc& rhs)
    {
        return lhs.vertexPath == rhs.vertexPath && lhs.fragmentPath == rhs.fragmentPath &&
               lhs.lastWriteTime == rhs.lastWriteTime;
    }

    friend bool operator!=(const ShaderDesc& lhs, const ShaderDesc& rhs)
    {
        return !(lhs == rhs);
    }

    void set_vertex_path(const std::filesystem::path& vertex_path);

    void set_fragment_path(const std::filesystem::path& fragment_path);

   private:
    void update_last_write_time(const std::filesystem::path& path);

    friend ShaderHandle createShader(const ShaderDesc& desc);
    std::filesystem::path vertexPath;
    std::filesystem::path fragmentPath;
    std::filesystem::file_time_type lastWriteTime;
};

struct ShaderResource {
    ShaderDesc desc;
    Shader shader;

    ShaderResource(const char* vertexPath, const char* fragmentPath)
        : shader(vertexPath, fragmentPath)
    {
    }

    ~ShaderResource()
    {
    }
};

using ShaderHandle = std::shared_ptr<ShaderResource>;
ShaderHandle createShader(const ShaderDesc& desc);

////////////////////////////////Texture///////////////////////////////////////

struct TextureDesc {
    pxr::GfVec2i size;
    pxr::HdFormat format;

    unsigned array_size = 1;

    friend bool operator==(const TextureDesc& lhs, const TextureDesc& rhs)
    {
        return lhs.size == rhs.size && lhs.format == rhs.format && lhs.array_size == rhs.array_size;
    }

    friend bool operator!=(const TextureDesc& lhs, const TextureDesc& rhs)
    {
        return !(lhs == rhs);
    }
};

struct TextureResource {
    TextureDesc desc;
    GLuint texture_id;

    ~TextureResource()
    {
        glDeleteTextures(1, &texture_id);
    }
};

using TextureHandle = std::shared_ptr<TextureResource>;
TextureHandle createTexture(const TextureDesc& desc);

#define DESC_HANDLE_TRAIT(RESOURCE)        \
    template<>                             \
    struct ResouceDesc<RESOURCE##Handle> { \
        using Desc = RESOURCE##Desc;       \
    };

#define HANDLE_DESC_TRAIT(RESOURCE)        \
    template<>                             \
    struct DescResouce<RESOURCE##Desc> {   \
        using Resource = RESOURCE##Handle; \
    };

template<typename RESOURCE>
struct ResouceDesc {
    using Desc = void;
};

template<typename DESC>
struct DescResouce {
    using Resource = void;
};

GLenum GetGLFormat(pxr::HdFormat hd_format);
GLenum GetGLType(pxr::HdFormat hd_format);
GLenum GetGLInternalFormat(pxr::HdFormat hd_format);

GLenum GetGLFormat(pxr::HioFormat hd_format);
GLenum GetGLType(pxr::HioFormat hd_format);
GLenum GetGLInternalFormat(pxr::HioFormat hd_format);
USTC_CG_NAMESPACE_CLOSE_SCOPE