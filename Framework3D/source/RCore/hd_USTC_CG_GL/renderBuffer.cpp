//
// Copyright 2018 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#include "renderBuffer.h"

#include <iostream>

#include "Windows.h"
#include "pxr/base/gf/half.h"
#include "renderParam.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;

Hd_USTC_CG_RenderBufferGL::Hd_USTC_CG_RenderBufferGL(SdfPath const &id)
    : HdRenderBuffer(id),
      _width(0),
      _height(0),
      _format(HdFormatInvalid),
      _multiSampled(false),
      _buffer(),
      _sampleBuffer(),
      _sampleCount(),
      _mappers(0),
      _converged(false)
{
}

Hd_USTC_CG_RenderBufferGL::~Hd_USTC_CG_RenderBufferGL() = default;

/*virtual*/
void Hd_USTC_CG_RenderBufferGL::Sync(
    HdSceneDelegate *sceneDelegate,
    HdRenderParam *renderParam,
    HdDirtyBits *dirtyBits)
{
    HdRenderBuffer::Sync(sceneDelegate, renderParam, dirtyBits);
}

/*virtual*/
void Hd_USTC_CG_RenderBufferGL::Finalize(HdRenderParam *renderParam)
{
    HdRenderBuffer::Finalize(renderParam);
}

/*virtual*/
void Hd_USTC_CG_RenderBufferGL::_Deallocate()
{
    // If the buffer is mapped while we're doing this, there's not a great
    // recovery path...
    TF_VERIFY(!IsMapped());

    if (fbo) {
        glDeleteFramebuffers(1, &fbo);
    }
    if (tex) {
        glDeleteTextures(1, &tex);
    }
    fbo = 0;
    tex = 0;

    _width = 0;
    _height = 0;
    _format = HdFormatInvalid;
    _multiSampled = false;
    _buffer.resize(0);
    _sampleBuffer.resize(0);
    _sampleCount.resize(0);

    _mappers.store(0);
    _converged.store(false);
}

/*static*/
HdFormat Hd_USTC_CG_RenderBufferGL::_GetSampleFormat(HdFormat format)
{
    HdFormat component = HdGetComponentFormat(format);
    size_t arity = HdGetComponentCount(format);

    if (component == HdFormatUNorm8 || component == HdFormatSNorm8 ||
        component == HdFormatFloat16 || component == HdFormatFloat32) {
        if (arity == 1) {
            return HdFormatFloat32;
        }
        else if (arity == 2) {
            return HdFormatFloat32Vec2;
        }
        else if (arity == 3) {
            return HdFormatFloat32Vec3;
        }
        else if (arity == 4) {
            return HdFormatFloat32Vec4;
        }
    }
    else if (component == HdFormatInt32) {
        if (arity == 1) {
            return HdFormatInt32;
        }
        else if (arity == 2) {
            return HdFormatInt32Vec2;
        }
        else if (arity == 3) {
            return HdFormatInt32Vec3;
        }
        else if (arity == 4) {
            return HdFormatInt32Vec4;
        }
    }
    return HdFormatInvalid;
}

/*virtual*/
bool Hd_USTC_CG_RenderBufferGL::Allocate(
    GfVec3i const &dimensions,
    HdFormat format,
    bool multiSampled)
{
    _Deallocate();

    if (dimensions[2] != 1) {
        TF_WARN(
            "Render buffer allocated with dims <%d, %d, %d> and"
            " format %s; depth must be 1!",
            dimensions[0],
            dimensions[1],
            dimensions[2],
            TfEnum::GetName(format).c_str());
        return false;
    }

    _width = dimensions[0];
    _height = dimensions[1];
    _format = format;

    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &tex);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        _GetGLFormat(_format),
        _width,
        _height,
        0,
        _GetGLFormat(_format),
        _GetGLType(_format),
        NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _buffer.resize(GetbufSize(), 255);

    _multiSampled = multiSampled;

    _sampleCount.resize(_width * _height);

    return true;
}

template<typename T>
static void _WriteOutput(HdFormat format, uint8_t *dst, T const *value)
{
    HdFormat componentFormat = HdGetComponentFormat(format);
    size_t componentCount = HdGetComponentCount(format);

    for (size_t c = 0; c < componentCount; ++c) {
        if (componentFormat == HdFormatInt32) {
            ((int32_t *)dst)[c] = (int32_t)value[c];
        }
        else if (componentFormat == HdFormatFloat16) {
            ((uint16_t *)dst)[c] = GfHalf(value[c]).bits();
        }
        else if (componentFormat == HdFormatFloat32) {
            ((float *)dst)[c] = (float)value[c];
        }
        else if (componentFormat == HdFormatUNorm8) {
            ((uint8_t *)dst)[c] = (uint8_t)(value[c] * 255.0f);
        }
        else if (componentFormat == HdFormatSNorm8) {
            ((int8_t *)dst)[c] = (int8_t)(value[c] * 127.0f);
        }
    }
}

void Hd_USTC_CG_RenderBufferGL::Clear(const float *value)
{
    uint8_t buffer[16];
    _WriteOutput(_format, buffer, value);

    glBindTexture(GL_TEXTURE_2D, tex);
    glClearTexImage(tex, 0, _GetGLFormat(_format), _GetGLType(_format), buffer);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void Hd_USTC_CG_RenderBufferGL::Clear(const int *value)
{
    uint8_t buffer[16];
    _WriteOutput(_format, buffer, value);

    glBindTexture(GL_TEXTURE_2D, tex);
    glClearTexImage(tex, 0, _GetGLFormat(_format), _GetGLType(_format), buffer);
    glBindTexture(GL_TEXTURE_2D, 0);

    assert(glGetError() == 0);
}

void Hd_USTC_CG_RenderBufferGL::Present(GLuint texture)
{
    GLuint temp;
    glCreateFramebuffers(1,&temp);
    // 绑定传入的纹理到帧缓冲
    glBindFramebuffer(GL_READ_FRAMEBUFFER, temp);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // 绑定自己的纹理到读取帧缓冲
    glBindFramebuffer(GL_READ_FRAMEBUFFER, temp);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    // 拷贝纹理数据到成员变量
    glBlitFramebuffer(
        0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // 解绑定帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1,&temp);
}

GLenum Hd_USTC_CG_RenderBufferGL::_GetGLFormat(HdFormat hd_format)
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

GLenum Hd_USTC_CG_RenderBufferGL::_GetGLType(HdFormat hd_format)
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
        case HdFormatFloat32UInt8: return GL_DEPTH32F_STENCIL8;
        default: throw std::runtime_error("Unsupported format");
    }
}

GLsizei Hd_USTC_CG_RenderBufferGL::GetbufSize()
{
    return _width * _height * HdDataSizeOfFormat(_format);
}

void *Hd_USTC_CG_RenderBufferGL::Map()
{
    glGetTextureImage(
        tex, 0, _GetGLFormat(_format), _GetGLType(_format), GetbufSize(), _buffer.data());
    _mappers++;
    return _buffer.data();
}

void Hd_USTC_CG_RenderBufferGL::Unmap()
{
    _mappers--;
}

/*virtual*/
void Hd_USTC_CG_RenderBufferGL::Resolve()
{
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
