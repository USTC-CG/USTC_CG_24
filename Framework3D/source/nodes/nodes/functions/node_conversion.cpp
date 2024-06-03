#include "../render/resource_allocator_instance.hpp"
#include "NODES_FILES_DIR.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "Nodes/socket_types/render_socket_types.hpp"
#include "Nodes/socket_types/stage_socket_types.hpp"
#include "RCore/Backend.hpp"
#include "boost/python/numpy.hpp"
#include "func_node_base.h"
#include "nvrhi/utils.h"
#include "pxr/imaging/hd/types.h"

namespace USTC_CG::node_conversion {
static void node_declare_Int_to_Float(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Int>("int");
    b.add_output<decl::Float>("float");
}

namespace bpn = boost::python::numpy;

static void node_exec_Int_to_Float(ExeParams params)
{
    auto i = params.get_input<int>("int");
    params.set_output("float", float(i));
    std::cout << "The invisible node is being executed!" << std::endl;
}

void node_exec_Texture_to_NumpyArray(ExeParams exe_params)
{
    auto handle = exe_params.get_input<TextureHandle>("Texture");

    std::vector shape{ handle->getDesc().width, handle->getDesc().height };
    std::vector<unsigned> stride{ sizeof(float), sizeof(float) };

    auto m_CommandList = resource_allocator.create(CommandListDesc{});

    auto staging = resource_allocator.create(
        StagingTextureDesc{ handle->getDesc() }, nvrhi::CpuAccessMode::Read);

    auto nvrhi_device = resource_allocator.device;
    auto width = handle->getDesc().width;
    auto height = handle->getDesc().height;
    auto s = boost::python::make_tuple(width, height, 4u);
    auto arr = bpn::empty(s, bpn::dtype::get_builtin<float>());

    m_CommandList->open();

    m_CommandList->copyTexture(staging, {}, handle, {});
    m_CommandList->close();

    nvrhi_device->executeCommandList(m_CommandList.Get());

    size_t pitch;
    auto mapped = nvrhi_device->mapStagingTexture(staging, {}, nvrhi::CpuAccessMode::Read, &pitch);

    auto hd_format = HdFormat_from_nvrhi_format(handle->getDesc().format);
    for (int i = 0; i < handle->getDesc().height; ++i) {
        auto copy_to_loc = arr.get_data() + i * width * HdDataSizeOfFormat(hd_format);
        memcpy(
            copy_to_loc, (uint8_t*)mapped + i * pitch, width * pxr::HdDataSizeOfFormat(hd_format));
    }

    nvrhi_device->unmapStagingTexture(staging);

    resource_allocator.destroy(m_CommandList);
    resource_allocator.destroy(staging);
    exe_params.set_output("NumpyArray", arr);
}

void node_declare_Texture_to_NumpyArray(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Texture>("Texture");
    b.add_output<decl::NumpyArray>("NumpyArray");
}

void node_declare_NumpyArray_to_Texture(NodeDeclarationBuilder& b)
{
    b.add_input<decl::NumpyArray>("NumpyArray");
    b.add_output<decl::Texture>("Texture");
}

void node_exec_NumpyArray_to_Texture(ExeParams exe_params)
{
    auto np_array = exe_params.get_input<bpn::ndarray>("NumpyArray");

    if (np_array.get_dtype() != bpn::dtype::get_builtin<float>()) {
        throw std::runtime_error("Numpy array must have dtype float.");
    }

    auto shape = np_array.get_shape();
    if (np_array.get_nd() != 3 || shape[2] != 4) {
        throw std::runtime_error("Numpy array must have shape (height, width, 4).");
    }

    auto width = shape[0];
    auto height = shape[1];

    auto tex_desc = nvrhi::TextureDesc().setWidth(width).setHeight(height).setFormat(
        nvrhi::Format::RGBA32_FLOAT);

    tex_desc.initialState = nvrhi::ResourceStates::CopyDest;
    tex_desc.keepInitialState = true;
    tex_desc.isUAV = true;

    auto texture = resource_allocator.create(tex_desc);
    auto staging =
        resource_allocator.create(StagingTextureDesc{ tex_desc }, nvrhi::CpuAccessMode::Write);

    auto m_CommandList = resource_allocator.create(CommandListDesc{});
    auto nvrhi_device = resource_allocator.device;

    size_t pitch;
    auto mapped = nvrhi_device->mapStagingTexture(staging, {}, nvrhi::CpuAccessMode::Write, &pitch);

    auto hd_format = HdFormat_from_nvrhi_format(tex_desc.format);

    for (int i = 0; i < height; ++i) {
        auto copy_from_loc = np_array.get_data() + i * width * HdDataSizeOfFormat(hd_format);
        memcpy(
            (uint8_t*)mapped + i * pitch,
            copy_from_loc,
            width * pxr::HdDataSizeOfFormat(hd_format));
    }

    nvrhi_device->unmapStagingTexture(staging);

    m_CommandList->open();
    m_CommandList->copyTexture(texture, {}, staging, {});
    m_CommandList->close();

    nvrhi_device->executeCommandList(m_CommandList.Get());

    resource_allocator.destroy(m_CommandList);
    resource_allocator.destroy(staging);

    exe_params.set_output("Texture", texture);
}

static void node_register()
{
#define CONVERSION(FROM, TO)                                                 \
    static NodeTypeInfo ntype_##FROM##_to_##TO;                              \
    strcpy(ntype_##FROM##_to_##TO.ui_name, "invisible");                     \
    strcpy(ntype_##FROM##_to_##TO.id_name, "conv_" #FROM "_to_" #TO);        \
    func_node_type_base(&ntype_##FROM##_to_##TO);                            \
    ntype_##FROM##_to_##TO.node_execute = node_exec_##FROM##_to_##TO;        \
    ntype_##FROM##_to_##TO.declare = node_declare_##FROM##_to_##TO;          \
    ntype_##FROM##_to_##TO.node_type_of_grpah = NodeTypeOfGrpah::Conversion; \
    ntype_##FROM##_to_##TO.INVISIBLE = true;                                 \
    ntype_##FROM##_to_##TO.conversion_from = SocketType::FROM;               \
    ntype_##FROM##_to_##TO.conversion_to = SocketType::TO;                   \
    nodeRegisterType(&ntype_##FROM##_to_##TO);

#define CONVERSION_TYPES Int_to_Float, Texture_to_NumpyArray, NumpyArray_to_Texture

    CONVERSION(Int, Float)
    CONVERSION(Texture, NumpyArray)
    CONVERSION(NumpyArray, Texture)
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_conversion
