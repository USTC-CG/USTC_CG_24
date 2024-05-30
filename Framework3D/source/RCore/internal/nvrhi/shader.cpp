#include <Windows.h>
#include <dxcapi.h>
#include <wrl.h>

#include <RCore/internal/nvrhi/Resources.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "RCore/Backend.hpp"

using namespace Microsoft::WRL;

USTC_CG_NAMESPACE_OPEN_SCOPE
void* ShaderCompileResult::getBufferPointer() const
{
    return blob->GetBufferPointer();
}

size_t ShaderCompileResult::getBufferSize() const
{
    return blob->GetBufferSize();
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

void ShaderCompileDesc::update_last_write_time(const std::filesystem::path& path)
{
    try {
        auto possibly_newer_lastWriteTime = fs::last_write_time(path);
        if (possibly_newer_lastWriteTime > lastWriteTime) {
            lastWriteTime = possibly_newer_lastWriteTime;
        }
    }
    catch (const fs::filesystem_error& e) {
        lastWriteTime = {};
    }
}

std::string ShaderCompileDesc::get_profile() const
{
    switch (shaderType) {
        case nvrhi::ShaderType::None: break;
        case nvrhi::ShaderType::Compute: break;
        case nvrhi::ShaderType::Vertex: break;
        case nvrhi::ShaderType::Hull: break;
        case nvrhi::ShaderType::Domain: break;
        case nvrhi::ShaderType::Geometry: break;
        case nvrhi::ShaderType::Pixel: break;
        case nvrhi::ShaderType::Amplification: break;
        case nvrhi::ShaderType::Mesh: break;
        case nvrhi::ShaderType::AllGraphics: break;
        case nvrhi::ShaderType::RayGeneration:
        case nvrhi::ShaderType::AnyHit:
        case nvrhi::ShaderType::ClosestHit:
        case nvrhi::ShaderType::Miss:
        case nvrhi::ShaderType::Intersection:
        case nvrhi::ShaderType::Callable:
        case nvrhi::ShaderType::AllRayTracing:
        case nvrhi::ShaderType::All: return "lib_6_5";
    }
    return "lib_6_5";
}

HRESULT CompileHLSLToDXIL(
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
        &sourceBuffer, args, _countof(args), pIncludeHandler.Get(), IID_PPV_ARGS(&pResults));
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

ShaderCompileHandle createShaderCompile(const ShaderCompileDesc& desc)
{
    ComPtr<IDxcBlob> blob;
    std::string error_string;
    CompileHLSLToDXIL(
        desc.path.c_str(), desc.entry_name.c_str(), desc.get_profile().c_str(), blob, error_string);
    ShaderCompileHandle ret = std::make_shared<ShaderCompileResult>();
    ret->blob = blob;
    ret->error_string = error_string;
    return ret;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE