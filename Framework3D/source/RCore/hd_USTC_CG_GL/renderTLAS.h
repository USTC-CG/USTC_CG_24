#pragma once
#include "RCore/Backend.hpp"
#include "USTC_CG.h"
#include "geometries/mesh.h"
#include "nvrhi/d3d12.h"
#include "pxr/imaging/garch/glApi.h"
#include "pxr/imaging/hd/renderBuffer.h"
#include "pxr/pxr.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

class Hd_USTC_CG_GL_RenderTLAS {
   public:
    explicit Hd_USTC_CG_GL_RenderTLAS(nvrhi::IDevice *nvrhi_device);

    AccelStructHandle get_tlas();

    std::vector<nvrhi::rt::InstanceDesc> &acquire_instances_to_edit(
        HdRprim *mesh);
    void removeInstance(HdRprim *hd_ustc_cg_mesh);

    std::mutex edit_instances_mutex;

   private:
    void rebuild_tlas();

    AccelStructHandle TLAS;
    std::map<HdRprim *, std::vector<nvrhi::rt::InstanceDesc>> instances;
    bool require_rebuild_tlas = true;

    nvrhi::IDevice *nvrhi_device;
    nvrhi::CommandListHandle m_command_list;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE