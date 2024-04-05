#include "RCore/internal/gl/GLResources.hpp"

#include <filesystem>

#include "pxr/imaging/hd/types.h"
USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
GLenum GetGLInternalFormat(HdFormat hd_format)
{
    switch (hd_format) {
        case HdFormatInvalid:
        case HdFormatUNorm8: return GL_RED;
        case HdFormatUNorm8Vec2: return GL_RG;
        case HdFormatUNorm8Vec3: return GL_RGB;
        case HdFormatUNorm8Vec4: return GL_RGBA;
        case HdFormatSNorm8: return GL_RED;
        case HdFormatSNorm8Vec2: return GL_RG;
        case HdFormatSNorm8Vec3: return GL_RGB;
        case HdFormatSNorm8Vec4: return GL_RGBA;
        case HdFormatFloat16: return GL_RED;
        case HdFormatFloat16Vec2: return GL_RG;
        case HdFormatFloat16Vec3: return GL_RGB;
        case HdFormatFloat16Vec4: return GL_RGBA;
        case HdFormatFloat32: return GL_RED;
        case HdFormatFloat32Vec2: return GL_RG;
        case HdFormatFloat32Vec3: return GL_RGB;
        case HdFormatFloat32Vec4: return GL_RGBA;
        case HdFormatInt16: return GL_RED;
        case HdFormatInt16Vec2: return GL_RG;
        case HdFormatInt16Vec3: return GL_RGB;
        case HdFormatInt16Vec4: return GL_RGBA;
        case HdFormatUInt16: return GL_RED;
        case HdFormatUInt16Vec2: return GL_RG;
        case HdFormatUInt16Vec3: return GL_RGB;
        case HdFormatUInt16Vec4: return GL_RGBA;
        case HdFormatInt32: return GL_RED;
        case HdFormatInt32Vec2: return GL_RG;
        case HdFormatInt32Vec3: return GL_RGB;
        case HdFormatInt32Vec4: return GL_RGBA;
        case HdFormatFloat32UInt8: return GL_DEPTH24_STENCIL8;
        default: throw std::runtime_error("Unsupported format");
    }
}

GLenum GetGLFormat(HdFormat hd_format)
{
    switch (hd_format) {
        case HdFormatInvalid:
        case HdFormatUNorm8: return GL_RED;
        case HdFormatUNorm8Vec2: return GL_RG;
        case HdFormatUNorm8Vec3: return GL_RGB;
        case HdFormatUNorm8Vec4: return GL_RGBA;
        case HdFormatSNorm8: return GL_RED;
        case HdFormatSNorm8Vec2: return GL_RG;
        case HdFormatSNorm8Vec3: return GL_RGB;
        case HdFormatSNorm8Vec4: return GL_RGBA;
        case HdFormatFloat16: return GL_RED;
        case HdFormatFloat16Vec2: return GL_RG;
        case HdFormatFloat16Vec3: return GL_RGB;
        case HdFormatFloat16Vec4: return GL_RGBA;
        case HdFormatFloat32: return GL_RED;
        case HdFormatFloat32Vec2: return GL_RG;
        case HdFormatFloat32Vec3: return GL_RGB;
        case HdFormatFloat32Vec4: return GL_RGBA;
        case HdFormatInt16: return GL_RED;
        case HdFormatInt16Vec2: return GL_RG;
        case HdFormatInt16Vec3: return GL_RGB;
        case HdFormatInt16Vec4: return GL_RGBA;
        case HdFormatUInt16: return GL_RED;
        case HdFormatUInt16Vec2: return GL_RG;
        case HdFormatUInt16Vec3: return GL_RGB;
        case HdFormatUInt16Vec4: return GL_RGBA;
        case HdFormatInt32: return GL_RED;
        case HdFormatInt32Vec2: return GL_RG;
        case HdFormatInt32Vec3: return GL_RGB;
        case HdFormatInt32Vec4: return GL_RGBA;
        case HdFormatFloat32UInt8: return GL_DEPTH_STENCIL;
        default: throw std::runtime_error("Unsupported format");
    }
}

GLenum GetGLType(HdFormat hd_format)
{
    switch (hd_format) {
        case HdFormatInvalid:
        case HdFormatUNorm8:
        case HdFormatUNorm8Vec2:
        case HdFormatUNorm8Vec3:
        case HdFormatUNorm8Vec4: return GL_UNSIGNED_BYTE;
        case HdFormatSNorm8:
        case HdFormatSNorm8Vec2:
        case HdFormatSNorm8Vec3:
        case HdFormatSNorm8Vec4: return GL_BYTE;
        case HdFormatFloat16:
        case HdFormatFloat16Vec2:
        case HdFormatFloat16Vec3:
        case HdFormatFloat16Vec4: return GL_HALF_FLOAT;
        case HdFormatFloat32:
        case HdFormatFloat32Vec2:
        case HdFormatFloat32Vec3:
        case HdFormatFloat32Vec4: return GL_FLOAT;
        case HdFormatInt16:
        case HdFormatInt16Vec2:
        case HdFormatInt16Vec3:
        case HdFormatInt16Vec4: return GL_INT16_NV;  // Danger
        case HdFormatUInt16:
        case HdFormatUInt16Vec2:
        case HdFormatUInt16Vec3:
        case HdFormatUInt16Vec4: return GL_INT16_NV;  // Danger
        case HdFormatInt32:
        case HdFormatInt32Vec2:
        case HdFormatInt32Vec3:
        case HdFormatInt32Vec4: return GL_INT;
        case HdFormatFloat32UInt8: return GL_UNSIGNED_INT_24_8;
        default: throw std::runtime_error("Unsupported format");
    }
}

namespace fs = std::filesystem;

void ShaderDesc::set_vertex_path(const std::filesystem::path& vertex_path)
{
    vertexPath = vertex_path;
    update_last_write_time(vertexPath);
}

void ShaderDesc::set_fragment_path(const std::filesystem::path& fragment_path)
{
    fragmentPath = fragment_path;
    update_last_write_time(fragmentPath);
}

void ShaderDesc::update_last_write_time(const std::filesystem::path& path)
{
    try {
        auto possibly_newer_lastWriteTime = fs::last_write_time(path);
        if (possibly_newer_lastWriteTime > lastWriteTime) {
            lastWriteTime = possibly_newer_lastWriteTime;
        }
    }
    catch (const fs::filesystem_error& e) {
        lastWriteTime = {};
    }
}

ShaderHandle createShader(const ShaderDesc& desc)
{
    ShaderHandle ret = std::make_shared<ShaderResource>(
        desc.vertexPath.string().c_str(), desc.fragmentPath.string().c_str());
    ret->desc = desc;
    return ret;
}

TextureHandle createTexture(const TextureDesc& desc)
{
    TextureHandle ret = std::make_shared<TextureResource>();
    ret->desc = desc;
    auto _format = desc.format;
    glCreateTextures(GL_TEXTURE_2D, 1, &ret->texture_id);
    glBindTexture(GL_TEXTURE_2D, ret->texture_id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GetGLInternalFormat(_format),
        desc.size[0],
        desc.size[1],
        0,
        GetGLFormat(_format),
        GetGLType(_format),
        NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return ret;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
