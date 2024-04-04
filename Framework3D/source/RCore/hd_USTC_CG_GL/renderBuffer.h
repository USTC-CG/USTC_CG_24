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
class Hd_USTC_CG_RenderBufferGL : public HdRenderBuffer {
   public:
    Hd_USTC_CG_RenderBufferGL(const SdfPath& id);
    ~Hd_USTC_CG_RenderBufferGL() override;

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

    GLsizei GetbufSize();

    void* Map() override;

    void Unmap() override;

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

    // The feed memory size must match the type
    void Clear(const float* value);
    void Clear(const int* value);
    void Present(GLuint texture);
    GLuint fbo = 0;
    GLuint tex = 0;

   private:
    static GLenum _GetGLFormat(HdFormat hd_format);

    static GLenum _GetGLType(HdFormat hd_format);

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
