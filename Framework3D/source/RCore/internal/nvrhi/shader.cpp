#include <Windows.h>
#include <dxcapi.h>
#include <wrl.h>

#include <RCore/internal/nvrhi/Resources.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "RCore/Backend.hpp"
#include "shaderCompiler.h"
#include "slang-com-ptr.h"
#include "slang.h"

using namespace Microsoft::WRL;

USTC_CG_NAMESPACE_OPEN_SCOPE
void const* ShaderCompileResult::getBufferPointer() const
{
    return blob->getBufferPointer();
}

size_t ShaderCompileResult::getBufferSize() const
{
    return blob->getBufferSize();
}

void ShaderCompileDesc::set_path(const std::filesystem::path& path)
{
    this->path = path;
    update_last_write_time(path);
}

void ShaderCompileDesc::set_shader_type(nvrhi::ShaderType shaderType)
{
    this->shaderType = shaderType;
}

void ShaderCompileDesc::set_entry_name(const std::string& entry_name)
{
    this->entry_name = entry_name;
    update_last_write_time(path);
}
namespace fs = std::filesystem;

void ShaderCompileDesc::update_last_write_time(
    const std::filesystem::path& path)
{
    if (fs::exists(path)) {
        auto possibly_newer_lastWriteTime = fs::last_write_time(path);
        if (possibly_newer_lastWriteTime > lastWriteTime) {
            lastWriteTime = possibly_newer_lastWriteTime;
        }
    }
    else {
        lastWriteTime = {};
    }
}

std::string ShaderCompileDesc::get_profile() const
{
    switch (shaderType) {
        case nvrhi::ShaderType::None: break;
        case nvrhi::ShaderType::Compute: return "cs_6_5";
        case nvrhi::ShaderType::Vertex: return "vs_6_5";
        case nvrhi::ShaderType::Hull: return "hs_6_5";
        case nvrhi::ShaderType::Domain: return "ds_6_5";
        case nvrhi::ShaderType::Geometry: return "gs_6_5";
        case nvrhi::ShaderType::Pixel: return "ps_6_5";
        case nvrhi::ShaderType::Amplification: return "as_6_5";
        case nvrhi::ShaderType::Mesh: return "ms_6_5";
        case nvrhi::ShaderType::AllGraphics: return "lib_6_5";
        case nvrhi::ShaderType::RayGeneration: return "rg_6_5";
        case nvrhi::ShaderType::AnyHit: return "ah_6_5";
        case nvrhi::ShaderType::ClosestHit: return "ch_6_5";
        case nvrhi::ShaderType::Miss: return "ms_6_5";
        case nvrhi::ShaderType::Intersection: return "is_6_5";
        case nvrhi::ShaderType::Callable: return "cs_6_5";
        case nvrhi::ShaderType::AllRayTracing: return "lib_6_5";
        case nvrhi::ShaderType::All: return "lib_6_5";
    }

    // Default return value for cases not handled explicitly
    return "lib_6_5";
}

HRESULT DxcCompileHLSLToDXIL(
    const wchar_t* filename,
    const char* entryPoint,
    const char* profile,
    ComPtr<IDxcBlob>& ppResultBlob,
    std::string& error_string)
{
    ComPtr<IDxcCompiler3> pCompiler;
    ComPtr<IDxcUtils> pUtils;
    ComPtr<IDxcIncludeHandler> pIncludeHandler;
    HRESULT hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&pCompiler));
    if (FAILED(hr)) {
        std::wcerr << L"Failed to create DXC Compiler instance." << std::endl;
        return hr;
    }

    hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&pUtils));
    if (FAILED(hr)) {
        std::wcerr << L"Failed to create DXC Utils instance." << std::endl;
        return hr;
    }

    hr = pUtils->CreateDefaultIncludeHandler(&pIncludeHandler);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to create include handler." << std::endl;
        return hr;
    }

    std::ifstream shaderFile(filename, std::ios::binary | std::ios::ate);
    if (!shaderFile.is_open()) {
        std::wcerr << L"Failed to open shader file." << std::endl;
        return E_FAIL;
    }

    ComPtr<IDxcBlobEncoding> sourceBlob;
    hr = pUtils->LoadFile(filename, nullptr, &sourceBlob);

    DxcBuffer sourceBuffer = {};
    sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
    sourceBuffer.Size = sourceBlob->GetBufferSize();

    wchar_t w_entryPoint[100];
    mbstowcs(w_entryPoint, entryPoint, strlen(entryPoint) + 1);  // Plus null

    wchar_t w_profile[100];
    mbstowcs(w_profile, profile, strlen(profile) + 1);  // Plus null

    const wchar_t* args[] = {
        filename,
        L"-T",
        w_profile,
        L"-Zi",              // Enable debug information.
        L"-Qstrip_reflect",  // Strip reflection data for smaller binaries.
        L"-enable-16bit-types",
        DXC_ARG_OPTIMIZATION_LEVEL3,
    };

    ComPtr<IDxcResult> pResults;
    hr = pCompiler->Compile(
        &sourceBuffer,
        args,
        _countof(args),
        pIncludeHandler.Get(),
        IID_PPV_ARGS(&pResults));
    if (FAILED(hr)) {
        std::wcerr << L"Compilation failed." << std::endl;
        return hr;
    }

    ComPtr<IDxcBlobUtf8> pErrors;
    hr = pResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), nullptr);
    if (SUCCEEDED(hr) && pErrors != nullptr && pErrors->GetStringLength() > 0) {
        std::wcerr << L"Warnings and Errors:" << std::endl;
        std::wcerr << pErrors->GetStringPointer() << std::endl;
        error_string = pErrors->GetStringPointer();
    }

    hr = pResults->GetResult(&ppResultBlob);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to get compiled shader." << std::endl;
        return hr;
    }

    return S_OK;
}

Slang::ComPtr<slang::IGlobalSession> globalSession;

Slang::ComPtr<slang::IGlobalSession> createGlobal()
{
    Slang::ComPtr<slang::IGlobalSession> globalSession;
    slang::createGlobalSession(globalSession.writeRef());

    SlangShaderCompiler::addHLSLPrelude(globalSession);

    return globalSession;
}

static nvrhi::ResourceType convertBindingTypeToResourceType(
    slang::BindingType bindingType)
{
    using namespace nvrhi;
    using namespace slang;
    switch (bindingType) {
        case BindingType::Sampler: return ResourceType::Sampler;
        case BindingType::Texture:
        case BindingType::CombinedTextureSampler:
        case BindingType::InputRenderTarget: return ResourceType::Texture_SRV;
        case BindingType::MutableTexture: return ResourceType::Texture_UAV;
        case BindingType::TypedBuffer:
        case BindingType::MutableTypedBuffer:
            return ResourceType::TypedBuffer_SRV;
        case BindingType::RawBuffer: return ResourceType::RawBuffer_SRV;
        case BindingType::MutableRawBuffer: return ResourceType::RawBuffer_UAV;
        case BindingType::ConstantBuffer:
        case BindingType::ParameterBlock: return ResourceType::ConstantBuffer;
        case BindingType::RayTracingAccelerationStructure:
            return ResourceType::RayTracingAccelStruct;
        case BindingType::PushConstant: return ResourceType::PushConstants;
        case BindingType::InlineUniformData:
        case BindingType::VaryingInput:
        case BindingType::VaryingOutput:
        case BindingType::ExistentialValue:
        case BindingType::MutableFlag:
        case BindingType::BaseMask:
        case BindingType::ExtMask:
        case BindingType::Unknown:
        default: return ResourceType::None;
    }
}

nvrhi::BindingLayoutDescVector shader_reflect(
    SlangCompileRequest* request,
    nvrhi::ShaderType shader_type)
{
    slang::ShaderReflection* programReflection =
        slang::ShaderReflection::get(request);
    // slang::EntryPointReflection* entryPoint =
    //     programReflection->findEntryPointByName(entryPointName);
    auto parameterCount = programReflection->getParameterCount();
    // auto parameterCount = entryPoint->getParameterCount();
    nvrhi::BindingLayoutDescVector ret;

    for (int pp = 0; pp < parameterCount; ++pp) {
        auto var = programReflection->getParameterByIndex(pp);
        auto cat = var->getCategory();

        slang::TypeLayoutReflection* typeLayout = var->getTypeLayout();
        auto categoryCount = var->getCategoryCount();
        assert(categoryCount == 1);

        auto category = SlangParameterCategory(var->getCategoryByIndex(0));
        auto index = var->getOffset(category);
        auto space = var->getBindingSpace(category);

        auto bindingRangeCount = typeLayout->getBindingRangeCount();
        assert(bindingRangeCount == 1);
        slang::BindingType type = typeLayout->getBindingRangeType(0);

        nvrhi::BindingLayoutItem item;

        item.type = convertBindingTypeToResourceType(type);
        item.slot = index;

        if (ret.size() < space + 1) {
            ret.resize(space + 1);
        }

        ret[space].addItem(item);
        ret[space].visibility = shader_type;
    }

    return ret;
}

// Function to convert ShaderType to SlangStage
SlangStage ConvertShaderTypeToSlangStage(nvrhi::ShaderType shaderType)
{
    using namespace nvrhi;
    switch (shaderType) {
        case ShaderType::Vertex: return SLANG_STAGE_VERTEX;
        case ShaderType::Hull: return SLANG_STAGE_HULL;
        case ShaderType::Domain: return SLANG_STAGE_DOMAIN;
        case ShaderType::Geometry: return SLANG_STAGE_GEOMETRY;
        case ShaderType::Pixel:
            return SLANG_STAGE_FRAGMENT;  // alias for SLANG_STAGE_PIXEL
        case ShaderType::Amplification: return SLANG_STAGE_AMPLIFICATION;
        case ShaderType::Mesh: return SLANG_STAGE_MESH;
        case ShaderType::Compute: return SLANG_STAGE_COMPUTE;
        case ShaderType::RayGeneration: return SLANG_STAGE_RAY_GENERATION;
        case ShaderType::AnyHit: return SLANG_STAGE_ANY_HIT;
        case ShaderType::ClosestHit: return SLANG_STAGE_CLOSEST_HIT;
        case ShaderType::Miss: return SLANG_STAGE_MISS;
        case ShaderType::Intersection: return SLANG_STAGE_INTERSECTION;
        case ShaderType::Callable: return SLANG_STAGE_CALLABLE;
        default: return SLANG_STAGE_NONE;
    }
}

void SlangCompileHLSLToDXIL(
    const char* filename,
    const char* entryPoint,
    nvrhi::ShaderType shaderType,
    const char* profile,
    const std::vector<ShaderMacro>& defines,  // List of macro defines
    nvrhi::BindingLayoutDescVector& shader_reflection,
    Slang::ComPtr<ISlangBlob>& ppResultBlob,
    std::string& error_string)
{
    auto stage = ConvertShaderTypeToSlangStage(shaderType);
    // Ensure global session is created
    if (!globalSession) {
        globalSession = createGlobal();
    }

    // Create a compile request
    SlangCompileRequest* slangRequest = spCreateCompileRequest(globalSession);

    // Set the code generation target to DXIL
    int targetIndex = slangRequest->addCodeGenTarget(SLANG_DXIL);
    spSetTargetFlags(
        slangRequest, targetIndex, SLANG_TARGET_FLAG_GENERATE_WHOLE_PROGRAM);
    SlangShaderCompiler::addHLSLHeaderInclude(slangRequest);
    SlangShaderCompiler::addHLSLSupportPreDefine(slangRequest);

    // Add a translation unit to the compile request
    int translationUnitIndex = spAddTranslationUnit(
        slangRequest, SLANG_SOURCE_LANGUAGE_SLANG, nullptr);

    // Set the profile ID
    auto profile_id = globalSession->findProfile(profile);
    slangRequest->setTargetProfile(targetIndex, profile_id);

    // Add the source file to the translation unit
    spAddTranslationUnitSourceFile(
        slangRequest, translationUnitIndex, filename);

    // Add macro defines to the compile request
    for (const auto& define : defines) {
        spAddPreprocessorDefine(
            slangRequest, define.name.c_str(), define.definition.c_str());
    }

    // If an entry point is provided, set it
    if (entryPoint && *entryPoint) {
        slangRequest->addEntryPoint(translationUnitIndex, entryPoint, stage);
    }

    // Compile the request
    const SlangResult compileRes = slangRequest->compile();

    // Handle compile errors
    if (SLANG_FAILED(compileRes)) {
        if (auto diagnostics = spGetDiagnosticOutput(slangRequest)) {
            error_string = diagnostics;
        }
        // Cleanup and return early if compilation failed
        spDestroyCompileRequest(slangRequest);
        return;
    }

    shader_reflection = shader_reflect(slangRequest, shaderType);

    // Retrieve the compiled code blob
    slangRequest->getTargetCodeBlob(targetIndex, ppResultBlob.writeRef());

    // Destroy the compile request to clean up
    spDestroyCompileRequest(slangRequest);
}

ShaderCompileHandle createShaderCompile(const ShaderCompileDesc& desc)
{
    Slang::ComPtr<ISlangBlob> blob;
    ShaderCompileHandle ret =
        ShaderCompileHandle::Create(new ShaderCompileResult);

    SlangCompileHLSLToDXIL(
        desc.path.generic_string().c_str(),
        desc.entry_name.c_str(),
        desc.shaderType,
        desc.get_profile().c_str(),
        desc.macros,
        ret->binding_layout_,
        ret->blob,
        ret->error_string);
    return ret;
}

nvrhi::BindingLayoutDescVector mergeBindingLayoutDescVectors(
    const nvrhi::BindingLayoutDescVector& vec1,
    const nvrhi::BindingLayoutDescVector& vec2)
{
    nvrhi::BindingLayoutDescVector result;
    size_t maxSize = std::max(vec1.size(), vec2.size());

    for (size_t i = 0; i < maxSize; ++i) {
        BindingLayoutDesc mergedDesc;

        if (i < vec1.size()) {
            mergedDesc = vec1[i];
        }
        else {
            mergedDesc = BindingLayoutDesc();
        }

        if (i < vec2.size()) {
            const BindingLayoutDesc& desc2 = vec2[i];
            mergedDesc.visibility = mergedDesc.visibility | desc2.visibility;
            for (int j = 0; j < desc2.bindings.size(); ++j) {
                mergedDesc.bindings.push_back(desc2.bindings[j]);
            }
        }

        result.push_back(mergedDesc);
    }

    return result;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE