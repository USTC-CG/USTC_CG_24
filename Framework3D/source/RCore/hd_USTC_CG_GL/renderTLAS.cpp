#include "renderTLAS.h"

USTC_CG::Hd_USTC_CG_GL_RenderTLAS::Hd_USTC_CG_GL_RenderTLAS(
    nvrhi::IDevice* nvrhi_device)
    : nvrhi_device(nvrhi_device)
{
    m_command_list = nvrhi_device->createCommandList();
    AccelStructDesc tlasDesc;
    tlasDesc.isTopLevel = true;
    tlasDesc.topLevelMaxInstances = 114514;
    TLAS = nvrhi_device->createAccelStruct(tlasDesc);
}

nvrhi::rt::AccelStructHandle USTC_CG::Hd_USTC_CG_GL_RenderTLAS::get_tlas()
{
    if (require_rebuild_tlas) {
        rebuild_tlas();
        require_rebuild_tlas = false;
    }

    return TLAS;
}

void USTC_CG::Hd_USTC_CG_GL_RenderTLAS::removeInstance(HdRprim* rPrim)
{
    std::lock_guard lock(edit_instances_mutex);
    instances.erase(rPrim);
}

std::vector<nvrhi::rt::InstanceDesc>&
USTC_CG::Hd_USTC_CG_GL_RenderTLAS::acquire_instances_to_edit(HdRprim* mesh)
{
    require_rebuild_tlas = true;
    return instances[mesh];
}

void USTC_CG::Hd_USTC_CG_GL_RenderTLAS::rebuild_tlas()
{
    std::vector<nvrhi::rt::InstanceDesc> instances_vec;

    // Iterate through the map and concatenate all vectors
    for (const auto& pair : instances) {
        const std::vector<nvrhi::rt::InstanceDesc>& vec = pair.second;
        instances_vec.insert(instances_vec.end(), vec.begin(), vec.end());
    }
    nvrhi_device->waitForIdle();

    m_command_list->open();
    m_command_list->beginMarker("TLAS Update");
    m_command_list->buildTopLevelAccelStruct(
        TLAS, instances_vec.data(), instances_vec.size());
    m_command_list->endMarker();

    m_command_list->close();

    nvrhi_device->executeCommandList(m_command_list);

    nvrhi_device->waitForIdle();
}