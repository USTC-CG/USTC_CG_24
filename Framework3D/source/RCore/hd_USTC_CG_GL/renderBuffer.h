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
#ifndef PXR_IMAGING_PLUGIN_HD_EMBREE_RENDER_BUFFER_H
#define PXR_IMAGING_PLUGIN_HD_EMBREE_RENDER_BUFFER_H
#include "USTC_CG.h"
#include "pxr/imaging/garch/glApi.h"
#include "pxr/imaging/hd/renderBuffer.h"
#include "pxr/pxr.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
class Hd_USTC_CG_RenderBuffer : public HdRenderBuffer {
   public:
    Hd_USTC_CG_RenderBuffer(const SdfPath& id);
    ~Hd_USTC_CG_RenderBuffer() override;

    void Sync(HdSceneDelegate* sceneDelegate, HdRenderParam* renderParam, HdDirtyBits* dirtyBits)
        override;

    void Finalize(HdRenderParam* renderParam) override;

    bool Allocate(const GfVec3i& dimensions, HdFormat format, bool multiSampled) override;

    unsigned int GetWidth() const override
    {
        return _width;
    }

    unsigned int GetHeight() const override
    {
        return _height;
    }

    unsigned int GetDepth() const override
    {
        return 1;
    }

    HdFormat GetFormat() const override
    {
        return _format;
    }

    bool IsMultiSampled() const override
    {
        return _multiSampled;
    }

    static GLenum GetGLFormat(HdFormat hd_format)
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
            case HdFormatFloat32UInt8:
            case HdFormatCount:
            default:;
        }
        return GL_RGBA;
    }

    static GLenum GetGLType(HdFormat hd_format)
    {
        switch (hd_format) {
            case HdFormatInvalid:
            case HdFormatUNorm8:
            case HdFormatUNorm8Vec2:
            case HdFormatUNorm8Vec3:
            case HdFormatUNorm8Vec4: return GL_UNSIGNED_NORMALIZED;
            case HdFormatSNorm8:
            case HdFormatSNorm8Vec2:
            case HdFormatSNorm8Vec3:
            case HdFormatSNorm8Vec4: return GL_SIGNED_NORMALIZED;
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
            case HdFormatFloat32UInt8:
            case HdFormatCount:
            default:;
        }
        return GL_UNSIGNED_BYTE;
    }

    GLsizei GetbufSize()
    {
        return _width * _height * HdDataSizeOfFormat(_format);
    }

    void* Map() override
    {
        if (!_mappers) {
            _buffer.resize(GetbufSize());
            glBindTexture(GL_TEXTURE_2D, tex);

            glGetTextureImage(
                tex, 0, GetGLFormat(_format), GetGLType(_format), GetbufSize(), _buffer.data());
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        _mappers++;

        return _buffer.data();
    }

    void Unmap() override
    {
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
            _buffer.data());
        glBindTexture(GL_TEXTURE_2D, 0);

        _mappers--;
    }

    bool IsMapped() const override
    {
        return _mappers.load() != 0;
    }

    bool IsConverged() const override
    {
        return _converged.load();
    }

    void SetConverged(bool cv)
    {
        _converged.store(cv);
    }

    void Resolve() override;

    void Clear(size_t numComponents, const float* value);
    void Clear(size_t numComponents, const int* value);

   private:
    // Calculate the needed _buffer size, given the allocation parameters.
    static size_t _GetBufferSize(const GfVec2i& dims, HdFormat format);

    static HdFormat _GetSampleFormat(HdFormat format);

    // Release any allocated resources.
    void _Deallocate() override;

    // Buffer width.
    unsigned int _width;
    // Buffer height.
    unsigned int _height;
    // Buffer format.
    HdFormat _format;
    // Whether the _buffer is operating in multisample mode.
    bool _multiSampled;

    GLuint fbo;
    GLuint tex;

    std::vector<uint8_t> _buffer;

    // For multisampled buffers: the input write _buffer.
    std::vector<uint8_t> _sampleBuffer;
    // For multisampled buffers: the sample count _buffer.
    std::vector<uint8_t> _sampleCount;

    // The number of callers mapping this _buffer.
    std::atomic<int> _mappers;
    // Whether the _buffer has been marked as converged.
    std::atomic<bool> _converged;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE

#endif  // PXR_IMAGING_PLUGIN_HD_EMBREE_RENDER_BUFFER_H
