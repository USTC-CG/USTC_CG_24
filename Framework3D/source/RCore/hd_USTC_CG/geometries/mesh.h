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

#include "context.h"
#include "meshSamplers.h"
#include "embree4/rtcore.h"
#include "pxr/pxr.h"
#include "pxr/base/gf/matrix4f.h"
#include "pxr/imaging/hd/mesh.h"
#include "pxr/imaging/hd/vertexAdjacency.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;
/// \class Hd_USTC_CG_Mesh
///
/// This class is an example of a Hydra Rprim, or renderable object, and it
/// gets created on a call to HdRenderIndex::InsertRprim() with a type of
/// HdPrimTypeTokens->mesh.
///
/// The prim object's main function is to bridge the scene description and the
/// renderable representation. The Hydra image generation algorithm will call
/// HdRenderIndex::SyncAll() before any drawing; this, in turn, will call
/// Sync() for each mesh with new data.
///
/// Sync() is passed a set of dirtyBits, indicating which scene buffers are
/// dirty. It uses these to pull all of the new scene data and constructs
/// updated geometry objects.
///
/// An rprim's state is lazily populated in Sync(); matching this, Finalize()
/// can do the heavy work of releasing state (such as handles into the top-level
/// scene), so that object population and existence aren't tied to each other.
///
class Hd_USTC_CG_Mesh final : public HdMesh
{
public:
    HF_MALLOC_TAG_NEW("new Hd_USTC_CG_Mesh");

    /// Hd_USTC_CG_Mesh constructor.
    ///   \param id The scene-graph path to this mesh.
    Hd_USTC_CG_Mesh(const SdfPath& id);

    /// Hd_USTC_CG_Mesh destructor.
    ~Hd_USTC_CG_Mesh() override;

    /// Inform the scene graph which state needs to be downloaded in the
    /// first Sync() call: in this case, topology and points data to build
    /// the geometry object in the scene graph.
    ///   \return The initial dirty state this mesh wants to query.
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
    // Initialize the given representation of this Rprim.
    // This is called prior to syncing the prim, the first time the repr
    // is used.
    //
    // reprToken is the name of the repr to initalize.
    //
    // dirtyBits is an in/out value.  It is initialized to the dirty bits
    // from the change tracker.  InitRepr can then set additional dirty bits
    // if additional data is required from the scene delegate when this
    // repr is synced.  InitRepr occurs before dirty bit propagation.
    //
    // See HdRprim::InitRepr()
    void _InitRepr(
        const TfToken& reprToken,
        HdDirtyBits* dirtyBits) override;

    // This callback from Rprim gives the prim an opportunity to set
    // additional dirty bits based on those already set.  This is done
    // before the dirty bits are passed to the scene delegate, so can be
    // used to communicate that extra information is needed by the prim to
    // process the changes.
    //
    // The return value is the new set of dirty bits, which replaces the bits
    // passed in.
    //
    // See HdRprim::PropagateRprimDirtyBits()
    HdDirtyBits _PropagateDirtyBits(HdDirtyBits bits) const override;
    TfTokenVector _UpdateComputedPrimvarSources(
        HdSceneDelegate* sceneDelegate,
        HdDirtyBits dirtyBits);
    void _UpdatePrimvarSources(
        HdSceneDelegate* sceneDelegate,
        HdDirtyBits dirtyBits);
    RTCGeometry _CreateEmbreeSubdivMesh(RTCScene scene, RTCDevice device);
    RTCGeometry _CreateEmbreeTriangleMesh(RTCScene scene, RTCDevice device);

    // This class does not support copying.
    Hd_USTC_CG_Mesh(const Hd_USTC_CG_Mesh&) = delete;
    Hd_USTC_CG_Mesh& operator =(const Hd_USTC_CG_Mesh&) = delete;

private:
    void _PopulateRtMesh(
        HdSceneDelegate* sceneDelegate,
        RTCScene scene,
        RTCDevice device,
        HdDirtyBits* dirtyBits,
        const HdMeshReprDesc& desc);
    HdEmbreePrototypeContext* _GetPrototypeContext();
    HdEmbreeInstanceContext* _GetInstanceContext(RTCScene scene, size_t i);

    // Cached scene data. VtArrays are reference counted, so as long as we
    // only call const accessors keeping them around doesn't incur a buffer
    // copy.

    RTCScene _rtcMeshScene;

    HdMeshTopology _topology;
    GfMatrix4f _transform;
    VtVec3fArray _points;
    HdCullStyle _cullStyle;
    bool _doubleSided;
    bool _smoothNormals;
    unsigned _rtcMeshId;

    // Each instance of the mesh in the top-level scene is stored in
    // _rtcInstanceIds.
    std::vector<unsigned> _rtcInstanceIds;

    std::vector<RTCGeometry> _rtcInstanceGeometries;

    // Derived scene data:
    // - _triangulatedIndices holds a triangulation of the source topology,
    //   which can have faces of arbitrary arity.
    // - _trianglePrimitiveParams holds a mapping from triangle index (in
    //   the triangulated topology) to authored face index.
    // - _computedNormals holds per-vertex normals computed as an average of
    //   adjacent face normals.
    VtVec3iArray _triangulatedIndices;
    VtIntArray _trianglePrimitiveParams;

    // Embree recommends after creating one should hold onto the geometry
    //
    //      "However, it is generally recommended to store the geometry handle
    //       inside the application's geometry representation and look up the
    //       geometry handle from that representation directly.""
    //
    // found this to be necessary in the case where multiple threads were
    // commiting to the scene at the same time, and a geometry needed to be
    // referenced again while other threads were committing
    RTCGeometry _geometry;
    bool _normalsValid;
    Hd_VertexAdjacency _adjacency;
    VtVec3fArray computedNormals;

    bool _adjacencyValid;
    bool _refined;

    // An embree intersection filter callback, for doing backface culling.
    static void _EmbreeCullFaces(const RTCFilterFunctionNArguments* args);

    HdEmbreeRTCBufferAllocator _embreeBufferAllocator;


    // A local cache of primvar scene data. "data" is a copy-on-write handle to
    // the actual primvar buffer, and "interpolation" is the interpolation mode
    // to be used. This cache is used in _PopulateRtMesh to populate the
    // primvar sampler map in the prototype context, which is used for shading.
    struct PrimvarSource
    {
        VtValue data;
        HdInterpolation interpolation;
    };

    TfHashMap<TfToken, PrimvarSource, TfToken::HashFunctor> _primvarSourceMap;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE

#endif // EXTRAS_IMAGING_EXAMPLES_HD_TINY_MESH_H
