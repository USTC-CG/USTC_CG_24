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

#include "pxr/base/gf/half.h"
#include "renderParam.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;

Hd_USTC_CG_RenderBuffer::Hd_USTC_CG_RenderBuffer(SdfPath const &id)
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

Hd_USTC_CG_RenderBuffer::~Hd_USTC_CG_RenderBuffer() = default;

/*virtual*/
void Hd_USTC_CG_RenderBuffer::Sync(
    HdSceneDelegate *sceneDelegate,
    HdRenderParam *renderParam,
    HdDirtyBits *dirtyBits)
{
    HdRenderBuffer::Sync(sceneDelegate, renderParam, dirtyBits);
}

/*virtual*/
void Hd_USTC_CG_RenderBuffer::Finalize(HdRenderParam *renderParam)
{
    HdRenderBuffer::Finalize(renderParam);
}

/*virtual*/
void Hd_USTC_CG_RenderBuffer::_Deallocate()
{
    // If the buffer is mapped while we're doing this, there's not a great
    // recovery path...
    TF_VERIFY(!IsMapped());

    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &tex);
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
size_t Hd_USTC_CG_RenderBuffer::_GetBufferSize(GfVec2i const &dims, HdFormat format)
{
    return dims[0] * dims[1] * HdDataSizeOfFormat(format);
}

/*static*/
HdFormat Hd_USTC_CG_RenderBuffer::_GetSampleFormat(HdFormat format)
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
bool Hd_USTC_CG_RenderBuffer::Allocate(
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
        GetGLFormat(_format),
        _width,
        _height,
        0,
        GetGLFormat(_format),
        GetGLType(_format),
        NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _multiSampled = multiSampled;
    if (_multiSampled) {
        _sampleBuffer.resize(_GetBufferSize(GfVec2i(_width, _height), _GetSampleFormat(format)));
        _sampleCount.resize(_width * _height);
    }

    return true;
}

void Hd_USTC_CG_RenderBuffer::Clear(size_t numComponents, float const *value)
{
    glBindTexture(GL_TEXTURE_2D, tex);
    glClearTexImage(tex, 0, GetGLFormat(_format), GetGLFormat(_format), value);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Hd_USTC_CG_RenderBuffer::Clear(size_t numComponents, int const *value)
{
    glBindTexture(GL_TEXTURE_2D, tex);
    glClearTexImage(tex, 0, GetGLFormat(_format), GetGLFormat(_format), value);
    glBindTexture(GL_TEXTURE_2D, 0);
}

/*virtual*/
void Hd_USTC_CG_RenderBuffer::Resolve()
{
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
