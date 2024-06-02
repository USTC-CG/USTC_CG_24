param(
    [string]$filename
)

# ���� CPP �ļ�·��
$cppFilePath = "node_${filename}.cpp"

# ����ļ��Ƿ��Ѵ���
if (Test-Path $cppFilePath) {
    Write-Host "�ļ��Ѵ���: $cppFilePath"
    exit
}

# Ҫд�������
$content = @"
#include "NODES_FILES_DIR.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "nvrhi/utils.h"
#include "render_node_base.h"
#include "resource_allocator_instance.hpp"

namespace USTC_CG::node_$filename {
static void node_declare(NodeDeclarationBuilder& b)
{
}

static void node_exec(ExeParams params)
{
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "$filename");
    strcpy(ntype.id_name, "node_$filename");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_$filename
"@

# ������д���ļ�
$content | Out-File -FilePath $cppFilePath -Encoding utf8

Write-Host "�Ѵ����ļ�: $cppFilePath"
