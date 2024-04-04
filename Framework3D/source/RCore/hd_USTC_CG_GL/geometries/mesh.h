//
// Copyright 2020 Pixar
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
#ifndef EXTRAS_IMAGING_EXAMPLES_HD_TINY_MESH_H
#define EXTRAS_IMAGING_EXAMPLES_HD_TINY_MESH_H

#include "USTC_CG.h"
#include "pxr/base/gf/matrix4f.h"
#include "pxr/imaging/hd/mesh.h"
#include "pxr/imaging/hd/vertexAdjacency.h"
#include "pxr/pxr.h"
#include "pxr/imaging/garch/glApi.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;

class Hd_USTC_CG_Mesh final : public HdMesh {
   public:
    HF_MALLOC_TAG_NEW("new Hd_USTC_CG_Mesh");

    Hd_USTC_CG_Mesh(const SdfPath& id);

    ~Hd_USTC_CG_Mesh() override;

    HdDirtyBits GetInitialDirtyBitsMask() const override;
    void _CreatePrimvarSampler(
        const TfToken& name,
        const VtValue& data,
        HdInterpolation interpolation,
        bool refined);

    void Sync(
        HdSceneDelegate* sceneDelegate,
        HdRenderParam* renderParam,
        HdDirtyBits* dirtyBits,
        const TfToken& reprToken) override;

    void Finalize(HdRenderParam* renderParam) override;

   protected:
    void _InitRepr(const TfToken& reprToken, HdDirtyBits* dirtyBits) override;

    HdDirtyBits _PropagateDirtyBits(HdDirtyBits bits) const override;
    TfTokenVector _UpdateComputedPrimvarSources(
        HdSceneDelegate* sceneDelegate,
        HdDirtyBits dirtyBits);
    void _UpdatePrimvarSources(HdSceneDelegate* sceneDelegate, HdDirtyBits dirtyBits);

    // This class does not support copying.
    Hd_USTC_CG_Mesh(const Hd_USTC_CG_Mesh&) = delete;
    Hd_USTC_CG_Mesh& operator=(const Hd_USTC_CG_Mesh&) = delete;

    GLuint VAO;
    GLuint VBO;

   private:
    HdMeshTopology _topology;
    GfMatrix4f _transform;
    HdCullStyle _cullStyle;
    bool _doubleSided;
    bool _smoothNormals;

    // Derived scene data:
    // - _triangulatedIndices holds a triangulation of the source topology,
    //   which can have faces of arbitrary arity.
    // - _trianglePrimitiveParams holds a mapping from triangle index (in
    //   the triangulated topology) to authored face index.
    // - _computedNormals holds per-vertex normals computed as an average of
    //   adjacent face normals.
    VtVec3iArray _triangulatedIndices;
    VtIntArray _trianglePrimitiveParams;

    bool _normalsValid;
    Hd_VertexAdjacency _adjacency;
    VtVec3fArray _computedNormals;

    bool _adjacencyValid;
    bool _refined;

    // A local cache of primvar scene data. "data" is a copy-on-write handle to
    // the actual primvar buffer, and "interpolation" is the interpolation mode
    // to be used. This cache is used in _PopulateRtMesh to populate the
    // primvar sampler map in the prototype context, which is used for shading.
    struct PrimvarSource {
        VtValue data;
        HdInterpolation interpolation;
    };

    TfHashMap<TfToken, PrimvarSource, TfToken::HashFunctor> _primvarSourceMap;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE

#endif  // EXTRAS_IMAGING_EXAMPLES_HD_TINY_MESH_H
