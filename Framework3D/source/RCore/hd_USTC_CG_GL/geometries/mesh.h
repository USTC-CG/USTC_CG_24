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
#include "pxr/pxr.h"
#include "pxr/base/gf/matrix4f.h"
#include "pxr/imaging/hd/mesh.h"
#include "pxr/imaging/hd/vertexAdjacency.h"

PXR_NAMESPACE_OPEN_SCOPE
/// \class Hd_USTC_CG_GL_Mesh
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
class Hd_USTC_CG_GL_Mesh final : public HdMesh
{
public:
    HF_MALLOC_TAG_NEW("new Hd_USTC_CG_GL_Mesh");

    /// Hd_USTC_CG_GL_Mesh constructor.
    ///   \param id The scene-graph path to this mesh.
    Hd_USTC_CG_GL_Mesh(const SdfPath& id);

    /// Hd_USTC_CG_GL_Mesh destructor.
    ~Hd_USTC_CG_GL_Mesh() override = default;

    /// Inform the scene graph which state needs to be downloaded in the
    /// first Sync() call: in this case, topology and points data to build
    /// the geometry object in the scene graph.
    ///   \return The initial dirty state this mesh wants to query.
    HdDirtyBits GetInitialDirtyBitsMask() const override;
    
    /// Pull invalidated scene data and prepare/update the renderable
    /// representation.
    ///
    /// This function is told which scene data to pull through the
    /// dirtyBits parameter. The first time it's called, dirtyBits comes
    /// from _GetInitialDirtyBits(), which provides initial dirty state,
    /// but after that it's driven by invalidation tracking in the scene
    /// delegate.
    ///
    /// The contract for this function is that the prim can only pull on scene
    /// delegate buffers that are marked dirty. Scene delegates can and do
    /// implement just-in-time data schemes that mean that pulling on clean
    /// data will be at best incorrect, and at worst a crash.
    ///
    /// This function is called in parallel from worker threads, so it needs
    /// to be threadsafe; calls into HdSceneDelegate are ok.
    ///
    /// Reprs are used by hydra for controlling per-item draw settings like
    /// flat/smooth shaded, wireframe, refined, etc.
    ///   \param sceneDelegate The data source for this geometry item.
    ///   \param renderParam State.
    ///   \param dirtyBits A specifier for which scene data has changed.
    ///   \param reprToken A specifier for which representation to draw with.
    ///
    void Sync(
        HdSceneDelegate* sceneDelegate,
        HdRenderParam* renderParam,
        HdDirtyBits* dirtyBits,
        const TfToken& reprToken) override;

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

    // This class does not support copying.
    Hd_USTC_CG_GL_Mesh(const Hd_USTC_CG_GL_Mesh&) = delete;
    Hd_USTC_CG_GL_Mesh& operator =(const Hd_USTC_CG_GL_Mesh&) = delete;

private:
    void _PopulateRtMesh(
        HdSceneDelegate* sceneDelegate,
        HdDirtyBits* dirtyBits,
        const HdMeshReprDesc& desc);


    HdMeshTopology _topology;
    GfMatrix4f _transform;
    VtVec3fArray _points;
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
    struct PrimvarSource
    {
        VtValue data;
        HdInterpolation interpolation;
    };

    TfHashMap<TfToken, PrimvarSource, TfToken::HashFunctor> _primvarSourceMap;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // EXTRAS_IMAGING_EXAMPLES_HD_TINY_MESH_H
