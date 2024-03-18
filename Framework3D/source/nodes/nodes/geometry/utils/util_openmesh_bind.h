#pragma once

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <memory>

#include "GCore/GOP.h"
#include "USTC_CG.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
class GOperandBase;
using PolyMesh = OpenMesh::PolyMesh_ArrayKernelT<>;

std::shared_ptr<PolyMesh> operand_to_openmesh(GOperandBase* mesh_oeprand);

std::shared_ptr<GOperandBase> openmesh_to_operand(PolyMesh* openmesh);

USTC_CG_NAMESPACE_CLOSE_SCOPE
