#pragma once

#include "USTC_CG.h"
#include "pxr/imaging/hd/types.h"
USTC_CG_NAMESPACE_OPEN_SCOPE
inline pxr::HdFormat HdFormat_from_nvrhi_format(nvrhi::Format format)
{
    using namespace pxr;
    switch (format) {
        case nvrhi::Format::UNKNOWN: return HdFormatInvalid;

        case nvrhi::Format::R8_UINT: return HdFormatUNorm8;
        case nvrhi::Format::R8_SINT: return HdFormatSNorm8;
        case nvrhi::Format::R8_UNORM: return HdFormatUNorm8;
        case nvrhi::Format::R8_SNORM: return HdFormatSNorm8;

        case nvrhi::Format::RG8_UINT: return HdFormatUNorm8Vec2;
        case nvrhi::Format::RG8_SINT: return HdFormatSNorm8Vec2;
        case nvrhi::Format::RG8_UNORM: return HdFormatUNorm8Vec2;
        case nvrhi::Format::RG8_SNORM: return HdFormatSNorm8Vec2;

        case nvrhi::Format::R16_UINT: return HdFormatUInt16;
        case nvrhi::Format::R16_SINT: return HdFormatInt16;
        case nvrhi::Format::R16_UNORM: return HdFormatUInt16;
        case nvrhi::Format::R16_SNORM: return HdFormatInt16;
        case nvrhi::Format::R16_FLOAT: return HdFormatFloat16;

        case nvrhi::Format::BGRA4_UNORM: return HdFormatInvalid;
        case nvrhi::Format::B5G6R5_UNORM: return HdFormatInvalid;
        case nvrhi::Format::B5G5R5A1_UNORM: return HdFormatInvalid;

        case nvrhi::Format::RGBA8_UINT: return HdFormatUNorm8Vec4;
        case nvrhi::Format::RGBA8_SINT: return HdFormatSNorm8Vec4;
        case nvrhi::Format::RGBA8_UNORM: return HdFormatUNorm8Vec4;
        case nvrhi::Format::RGBA8_SNORM: return HdFormatSNorm8Vec4;

        case nvrhi::Format::BGRA8_UNORM: return HdFormatUNorm8Vec4;
        case nvrhi::Format::SRGBA8_UNORM: return HdFormatInvalid;
        case nvrhi::Format::SBGRA8_UNORM: return HdFormatInvalid;

        case nvrhi::Format::R10G10B10A2_UNORM: return HdFormatInvalid;
        case nvrhi::Format::R11G11B10_FLOAT: return HdFormatInvalid;

        case nvrhi::Format::RG16_UINT: return HdFormatUInt16Vec2;
        case nvrhi::Format::RG16_SINT: return HdFormatInt16Vec2;
        case nvrhi::Format::RG16_UNORM: return HdFormatUInt16Vec2;
        case nvrhi::Format::RG16_SNORM: return HdFormatInt16Vec2;
        case nvrhi::Format::RG16_FLOAT: return HdFormatFloat16Vec2;

        case nvrhi::Format::R32_UINT: return HdFormatInt32;
        case nvrhi::Format::R32_SINT: return HdFormatInt32;
        case nvrhi::Format::R32_FLOAT: return HdFormatFloat32;

        case nvrhi::Format::RGBA16_UINT: return HdFormatUInt16Vec4;
        case nvrhi::Format::RGBA16_SINT: return HdFormatInt16Vec4;
        case nvrhi::Format::RGBA16_FLOAT: return HdFormatFloat16Vec4;
        case nvrhi::Format::RGBA16_UNORM: return HdFormatUInt16Vec4;
        case nvrhi::Format::RGBA16_SNORM: return HdFormatInt16Vec4;

        case nvrhi::Format::RG32_UINT: return HdFormatInt32Vec2;
        case nvrhi::Format::RG32_SINT: return HdFormatInt32Vec2;
        case nvrhi::Format::RG32_FLOAT: return HdFormatFloat32Vec2;

        case nvrhi::Format::RGB32_UINT: return HdFormatInt32Vec3;
        case nvrhi::Format::RGB32_SINT: return HdFormatInt32Vec3;
        case nvrhi::Format::RGB32_FLOAT: return HdFormatFloat32Vec3;

        case nvrhi::Format::RGBA32_UINT: return HdFormatInt32Vec4;
        case nvrhi::Format::RGBA32_SINT: return HdFormatInt32Vec4;
        case nvrhi::Format::RGBA32_FLOAT: return HdFormatFloat32Vec4;

        case nvrhi::Format::D16: return HdFormatInvalid;
        case nvrhi::Format::D24S8: return HdFormatFloat32UInt8;
        case nvrhi::Format::X24G8_UINT: return HdFormatInvalid;
        case nvrhi::Format::D32: return HdFormatInvalid;
        case nvrhi::Format::D32S8: return HdFormatFloat32UInt8;
        case nvrhi::Format::X32G8_UINT: return HdFormatInvalid;

        case nvrhi::Format::BC1_UNORM: return HdFormatInvalid;
        case nvrhi::Format::BC1_UNORM_SRGB: return HdFormatInvalid;
        case nvrhi::Format::BC2_UNORM: return HdFormatInvalid;
        case nvrhi::Format::BC2_UNORM_SRGB: return HdFormatInvalid;
        case nvrhi::Format::BC3_UNORM: return HdFormatInvalid;
        case nvrhi::Format::BC3_UNORM_SRGB: return HdFormatInvalid;
        case nvrhi::Format::BC4_UNORM: return HdFormatInvalid;
        case nvrhi::Format::BC4_SNORM: return HdFormatInvalid;
        case nvrhi::Format::BC5_UNORM: return HdFormatInvalid;
        case nvrhi::Format::BC5_SNORM: return HdFormatInvalid;
        case nvrhi::Format::BC6H_UFLOAT: return HdFormatInvalid;
        case nvrhi::Format::BC6H_SFLOAT: return HdFormatInvalid;
        case nvrhi::Format::BC7_UNORM: return HdFormatInvalid;
        case nvrhi::Format::BC7_UNORM_SRGB: return HdFormatInvalid;

        case nvrhi::Format::COUNT: break;
    }
    return HdFormatInvalid;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE