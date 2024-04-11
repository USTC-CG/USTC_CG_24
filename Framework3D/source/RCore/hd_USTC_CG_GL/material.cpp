// #define __GNUC__

#include "material.h"

#include "RCore/internal/gl/GLResources.hpp"
#include "Utils/Logging/Logging.h"
#include "pxr/imaging/hd/sceneDelegate.h"
#include "pxr/imaging/hio/image.h"
#include "pxr/usd/sdr/shaderNode.h"
#include "pxr/usd/usd/tokens.h"
#include "pxr/usdImaging/usdImaging/tokens.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
using namespace pxr;

// Here for the cource purpose, we support a very limited set of forms of the material.
// Specifically, we support only UsdPreviewSurface, and each input can be either value, or a texture
// connected to a primvar reader.

HdMaterialNode2 Hd_USTC_CG_Material::get_input_connection(
    HdMaterialNetwork2 surfaceNetwork,
    std::map<TfToken, std::vector<HdMaterialConnection2>>::value_type& input_connection)
{
    HdMaterialNode2 upstream;
    assert(input_connection.second.size() == 1);
    upstream = surfaceNetwork.nodes[input_connection.second[0].upstreamNode];
    return upstream;
}

void Hd_USTC_CG_Material::DestroyTexture(InputDescriptor& input_descriptor)
{
    if (input_descriptor.glTexture) {
        glDeleteTextures(1, &input_descriptor.glTexture);
        input_descriptor.glTexture = 0;
    }
}

void Hd_USTC_CG_Material::TryLoadTexture(
    const char* name,
    InputDescriptor& descriptor,
    HdMaterialNode2& usd_preview_surface)
{
    for (auto&& input_connection : usd_preview_surface.inputConnections) {
        if (input_connection.first == TfToken(name)) {
            logging("Loading texture: " + input_connection.first.GetString(), Info);
            auto texture_node = get_input_connection(surfaceNetwork, input_connection);
            assert(texture_node.nodeTypeId == UsdImagingTokens->UsdUVTexture);

            auto file_name =
                texture_node.parameters[TfToken("file")].Get<SdfAssetPath>().GetAssetPath();
            logging("Texture file name: " + file_name, Info);

            HioImage::SourceColorSpace colorSpace;

            if (texture_node.parameters[TfToken("sourceColorSpace")] == TfToken("sRGB")) {
                colorSpace = HioImage::SRGB;
            }
            else {
                colorSpace = HioImage::Raw;
            }

            descriptor.image = HioImage::OpenForReading(file_name, 0, 0, colorSpace);
            descriptor.wrapS = texture_node.parameters[TfToken("wrapS")].Get<TfToken>();
            descriptor.wrapT = texture_node.parameters[TfToken("wrapT")].Get<TfToken>();

            HdMaterialNode2 st_read_node;
            for (auto&& st_read_connection : texture_node.inputConnections) {
                st_read_node = get_input_connection(surfaceNetwork, st_read_connection);
            }

            assert(st_read_node.nodeTypeId == UsdImagingTokens->UsdPrimvarReader_float2);
            descriptor.uv_primvar_name = st_read_node.parameters[TfToken("varname")].Get<TfToken>();
        }
    }
}

void Hd_USTC_CG_Material::TryLoadParameter(
    const char* name,
    InputDescriptor& descriptor,
    HdMaterialNode2& usd_preview_surface)
{
    for (auto&& parameter : usd_preview_surface.parameters) {
        if (parameter.first == name) {
            descriptor.value = parameter.second;
            logging("Loading parameter: " + parameter.first.GetString(), Info);
        }
    }
}

GLuint Hd_USTC_CG_Material::createTextureFromHioImage(const InputDescriptor& descriptor)
{
    // Step 4: Create an OpenGL texture object
    GLuint texture;
    glGenTextures(1, &texture);

    // Step 5: Bind the texture object and specify its parameters
    glBindTexture(GL_TEXTURE_2D, texture);

    auto image = descriptor.image;
    if (image) {
        // Step 1: Get image information
        int width = image->GetWidth();
        int height = image->GetHeight();
        HioFormat format = image->GetFormat();

        HioImage::StorageSpec storageSpec;
        storageSpec.width = width;
        storageSpec.height = height;
        storageSpec.format = format;
        storageSpec.data = malloc(width * height * image->GetBytesPerPixel());
        if (!storageSpec.data) {
            return 0;
        }

        // Step 3: Read the image data
        if (!image->Read(storageSpec)) {
            free(storageSpec.data);
            return 0;
        }

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GetGLInternalFormat(format),
            width,
            height,
            0,
            GetGLFormat(format),
            GetGLType(format),
            storageSpec.data);
        free(storageSpec.data);
    }
    else {
        if (!descriptor.value.IsEmpty()) {
            if (descriptor.value.CanCast<GfVec3f>()) {
                auto val = descriptor.value.Get<GfVec3f>();
                float color[4] = { val[0], val[1], val[2], 1.0f };

                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GetGLInternalFormat(HioFormatFloat32Vec4),
                    1,
                    1,
                    0,
                    GetGLFormat(HioFormatFloat32Vec4),
                    GetGLType(HioFormatFloat32Vec4),
                    color);
            }
        }
    }
    // glGenerateMipmap(texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

void Hd_USTC_CG_Material::TryCreateGLTexture(InputDescriptor& descriptor)
{
    if (descriptor.glTexture == 0) {
        descriptor.glTexture = createTextureFromHioImage(descriptor);
    }
}

#define INPUT_LIST                                                                       \
    diffuseColor, specularColor, emissiveColor, displacement, opacity, opacityThreshold, \
        roughness, metallic, clearcoat, clearcoatRoughness, occlusion, normal, ior

#define TRY_LOAD(INPUT)                                 \
    TryLoadTexture(#INPUT, INPUT, usd_preview_surface); \
    TryLoadParameter(#INPUT, INPUT, usd_preview_surface);

Hd_USTC_CG_Material::Hd_USTC_CG_Material(const SdfPath& id) : HdMaterial(id)
{
    diffuseColor.value = VtValue(GfVec3f(0.8, 0.8, 0.8));
    roughness.value = VtValue(GfVec3f(0.0f, 0.0, 0.8));

    metallic.value = VtValue(GfVec3f(0.0f, 0.0, 0.8));
    normal.value = VtValue(GfVec3f(0.5, 0.5, 1.0));
}

void Hd_USTC_CG_Material::Sync(
    HdSceneDelegate* sceneDelegate,
    HdRenderParam* renderParam,
    HdDirtyBits* dirtyBits)
{
    VtValue vtMat = sceneDelegate->GetMaterialResource(GetId());
    if (vtMat.IsHolding<HdMaterialNetworkMap>()) {
        const HdMaterialNetworkMap& hdNetworkMap = vtMat.UncheckedGet<HdMaterialNetworkMap>();
        if (!hdNetworkMap.terminals.empty() && !hdNetworkMap.map.empty()) {
            logging("Loaded a material", Info);

            surfaceNetwork = HdConvertToHdMaterialNetwork2(hdNetworkMap);

            // Here we only support single output material.
            assert(surfaceNetwork.terminals.size() == 1);

            auto terminal = surfaceNetwork.terminals[HdMaterialTerminalTokens->surface];

            auto usd_preview_surface = surfaceNetwork.nodes[terminal.upstreamNode];
            assert(usd_preview_surface.nodeTypeId == UsdImagingTokens->UsdPreviewSurface);

            MACRO_MAP(TRY_LOAD, INPUT_LIST)
        }
    }
    else {
        logging("Not loaded a material", Info);
    }
    *dirtyBits = Clean;
}

void Hd_USTC_CG_Material::RefreshGLBuffer()
{
    ;
    TryCreateGLTexture(diffuseColor);
    TryCreateGLTexture(specularColor);
    TryCreateGLTexture(emissiveColor);
    TryCreateGLTexture(displacement);
    TryCreateGLTexture(opacity);
    TryCreateGLTexture(opacityThreshold);
    TryCreateGLTexture(roughness);
    TryCreateGLTexture(metallic);
    TryCreateGLTexture(clearcoat);
    TryCreateGLTexture(clearcoatRoughness);
    TryCreateGLTexture(occlusion);
    TryCreateGLTexture(normal);
    TryCreateGLTexture(ior);
}

void Hd_USTC_CG_Material::BindTextures(Shader& shader)
{
    assert(diffuseColor.glTexture);
    shader.setInt("diffuseColorSampler", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseColor.glTexture);

    shader.setInt("normalMapSampler", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normal.glTexture);

    shader.setInt("metallicRoughnessSampler", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, metallic.glTexture);
}

HdDirtyBits Hd_USTC_CG_Material::GetInitialDirtyBitsMask() const
{
    return AllDirty;
}

#define requireTexCoord(INPUT)              \
    if (!INPUT.uv_primvar_name.IsEmpty()) { \
        return INPUT.uv_primvar_name;       \
    }

TfToken Hd_USTC_CG_Material::requireTexcoordName()
{
    MACRO_MAP(requireTexCoord, INPUT_LIST)
    return {};
}

void Hd_USTC_CG_Material::Finalize(HdRenderParam* renderParam)
{
    MACRO_MAP(; DestroyTexture, INPUT_LIST);

    HdMaterial::Finalize(renderParam);
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
