#include "slang.h"

#include <gtest/gtest.h>

#include <fstream>

#include "RCore/Backend.hpp"
#include "RCore/internal/nvrhi/shaderCompiler.h"
#include "slang-com-ptr.h"

using namespace USTC_CG;

TEST(SLANG, createGlobal)
{
    Slang::ComPtr<slang::IGlobalSession> globalSession;
    slang::createGlobalSession(globalSession.writeRef());
    ASSERT_NE(globalSession.get(), nullptr);
}

Slang::ComPtr<slang::IGlobalSession> createGlobal()
{
    Slang::ComPtr<slang::IGlobalSession> globalSession;
    slang::createGlobalSession(globalSession.writeRef());
    return globalSession;
}

TEST(SLANG, createSession)
{
    auto globalSession = createGlobal();

    slang::SessionDesc sessionDesc;
    Slang::ComPtr<slang::ISession> session;

    slang::TargetDesc targetDesc;
    targetDesc.format = SlangCompileTarget::SLANG_CUDA_SOURCE;

    sessionDesc.targets = &targetDesc;
    sessionDesc.targetCount = 1;

    globalSession->createSession(sessionDesc, session.writeRef());
    ASSERT_NE(session.get(), nullptr);

    auto shaderPath = SlangShaderCompiler::find_root(".") /
                      "usd/hd_USTC_CG_GL/resources/shaders/shader.slang";

    Slang::ComPtr<slang::IBlob> outDiagnostic;
    Slang::ComPtr<slang::IModule> module_;

    module_ = session->loadModule(
        shaderPath.generic_string().c_str(), outDiagnostic.writeRef());
}
TEST(SLANG, findRoot)
{
    ASSERT_NE(SlangShaderCompiler::find_root("."), std::filesystem::path(""));
}
#if USTC_CG_WITH_CUDA

TEST(SLANG, setPrelude)
{
    auto globalSession = createGlobal();

    ASSERT_EQ(SlangShaderCompiler::addCUDAPrelude(globalSession), SLANG_OK);
}

TEST(SLANG, compileFromGlobal)
{
    auto globalSession = createGlobal();

    ASSERT_EQ(SlangShaderCompiler::addCUDAPrelude(globalSession), SLANG_OK);
    SlangCompileRequest* slangRequest = spCreateCompileRequest(globalSession);

    int targetIndex = spAddCodeGenTarget(slangRequest, SLANG_CUDA_SOURCE);
    spSetTargetFlags(
        slangRequest, targetIndex, SLANG_TARGET_FLAG_GENERATE_WHOLE_PROGRAM);
    int translationUnitIndex = spAddTranslationUnit(
        slangRequest, SLANG_SOURCE_LANGUAGE_SLANG, nullptr);

    auto shaderPath = SlangShaderCompiler::find_root(".") /
                      "usd/hd_USTC_CG_GL/resources/shaders/shader.slang";
    spAddTranslationUnitSourceFile(
        slangRequest,
        translationUnitIndex,
        shaderPath.generic_string().c_str());
    const SlangResult compileRes = spCompile(slangRequest);

    if (SLANG_FAILED(compileRes)) {
        if (auto diagnostics = spGetDiagnosticOutput(slangRequest)) {
            std::cerr << "Error diagnostics: " << diagnostics;
        }
    }
    ASSERT_EQ(compileRes, SLANG_OK);

    if (SLANG_FAILED(compileRes)) {
        spDestroyCompileRequest(slangRequest);
    }
}

TEST(SLANG, codeGen)
{
    auto globalSession = createGlobal();

    ASSERT_EQ(SlangShaderCompiler::addCUDAPrelude(globalSession), SLANG_OK);
    SlangCompileRequest* slangRequest = spCreateCompileRequest(globalSession);

    int targetIndex = spAddCodeGenTarget(slangRequest, SLANG_CUDA_SOURCE);
    spSetTargetFlags(
        slangRequest, targetIndex, SLANG_TARGET_FLAG_GENERATE_WHOLE_PROGRAM);
    int translationUnitIndex = spAddTranslationUnit(
        slangRequest, SLANG_SOURCE_LANGUAGE_SLANG, nullptr);
    spTranslationUnit_addPreprocessorDefine(
        slangRequest, translationUnitIndex, "SLANG_CUDA_ENABLE_OPTIX", "1");

    auto shaderPath = SlangShaderCompiler::find_root(".") /
                      "usd/hd_USTC_CG_GL/resources/shaders/shader.slang";
    spAddTranslationUnitSourceFile(
        slangRequest,
        translationUnitIndex,
        shaderPath.generic_string().c_str());

    const SlangResult compileRes = spCompile(slangRequest);
    ASSERT_EQ(compileRes, SLANG_OK);

    Slang::ComPtr<ISlangBlob> outBlob;
    auto Result =
        slangRequest->getTargetCodeBlob(targetIndex, outBlob.writeRef());
    ASSERT_EQ(Result, SLANG_OK);

    std::string code((char*)(outBlob->getBufferPointer()));
    ASSERT_EQ(code.size(), outBlob->getBufferSize());
    ASSERT_NE(code.size(), 0);

    SlangShaderCompiler::save_file(
        "dump.cu", (char*)outBlob->getBufferPointer());
}

TEST(SLANG, genPTX)
{
    auto globalSession = createGlobal();

    ASSERT_EQ(SlangShaderCompiler::addCUDAPrelude(globalSession), SLANG_OK);
    SlangCompileRequest* slangRequest = spCreateCompileRequest(globalSession);

    int targetIndex = spAddCodeGenTarget(slangRequest, SLANG_PTX);
    spSetTargetFlags(
        slangRequest, targetIndex, SLANG_TARGET_FLAG_GENERATE_WHOLE_PROGRAM);
    int translationUnitIndex = spAddTranslationUnit(
        slangRequest, SLANG_SOURCE_LANGUAGE_SLANG, nullptr);

    auto shaderPath = SlangShaderCompiler::find_root(".") /
                      "usd/hd_USTC_CG_GL/resources/shaders/shader.slang";

    SlangShaderCompiler::addOptiXSupport(slangRequest);

    spAddTranslationUnitSourceFile(
        slangRequest,
        translationUnitIndex,
        shaderPath.generic_string().c_str());

    const SlangResult compileRes = spCompile(slangRequest);

    if (SLANG_FAILED(compileRes)) {
        if (auto diagnostics = spGetDiagnosticOutput(slangRequest)) {
            std::cerr << "Error diagnostics: " << diagnostics;
        }
    }

    ASSERT_EQ(compileRes, SLANG_OK);

    Slang::ComPtr<ISlangBlob> outBlob;
    auto Result =
        slangRequest->getTargetCodeBlob(targetIndex, outBlob.writeRef());
    ASSERT_EQ(Result, SLANG_OK);

    std::string code((char*)(outBlob->getBufferPointer()));
    ASSERT_NE(code.size(), 0);

    SlangShaderCompiler::save_file(
        "dump.ptx", (char*)outBlob->getBufferPointer());
}

static std::string indent(int id)
{
    return std::string(id, ' ');
}

TEST(SLANG, reflection)
{
    auto globalSession = createGlobal();

    ASSERT_EQ(SlangShaderCompiler::addCUDAPrelude(globalSession), SLANG_OK);
    SlangCompileRequest* slangRequest = spCreateCompileRequest(globalSession);

    int targetIndex = spAddCodeGenTarget(slangRequest, SLANG_CUDA_SOURCE);
    spSetTargetFlags(
        slangRequest, targetIndex, SLANG_TARGET_FLAG_GENERATE_WHOLE_PROGRAM);
    int translationUnitIndex = spAddTranslationUnit(
        slangRequest, SLANG_SOURCE_LANGUAGE_SLANG, nullptr);

    auto shaderPath = SlangShaderCompiler::find_root(".") /
                      "usd/hd_USTC_CG_GL/resources/shaders/shader.slang";
    spAddTranslationUnitSourceFile(
        slangRequest,
        translationUnitIndex,
        shaderPath.generic_string().c_str());
    const SlangResult compileRes = spCompile(slangRequest);
    ASSERT_EQ(compileRes, SLANG_OK);

    Slang::ComPtr<ISlangBlob> outBlob;
    auto Result =
        slangRequest->getTargetCodeBlob(targetIndex, outBlob.writeRef());

    ASSERT_EQ(Result, SLANG_OK);

    SlangReflection* reflection = slangRequest->getReflection();

    slang::ShaderReflection* shaderReflection =
        slang::ShaderReflection::get(slangRequest);

    std::ofstream reflection_log("reflection.log");

    reflection_log << "Parameter" << std::endl;
    unsigned ParameterCount = shaderReflection->getParameterCount();
    for (unsigned pp = 0; pp < ParameterCount; pp++) {
        auto parameter = shaderReflection->getParameterByIndex(pp);
        reflection_log << indent(2) << parameter->getName() << std::endl;
#define output_style(name) indent(4) << #name << ' ' << name << std::endl

        auto category = (SlangParameterCategory)parameter->getCategory();
        unsigned index = parameter->getBindingIndex();
        reflection_log << output_style(index);
        unsigned space = parameter->getBindingSpace();
        reflection_log << output_style(space);

        size_t offsetForCategory = parameter->getOffset(category);
        reflection_log << output_style(offsetForCategory);

        size_t spaceForCategory = parameter->getBindingSpace(category);
        reflection_log << output_style(spaceForCategory);
        slang::TypeLayoutReflection* typeLayout = parameter->getTypeLayout();
        slang::TypeReflection::Kind kind = typeLayout->getKind();
        reflection_log << output_style(int(kind));
        reflection_log << output_style(typeLayout->getName());
        reflection_log << output_style(typeLayout->getDescriptorSetCount());
        reflection_log << output_style(typeLayout->getBindingRangeCount());

        // query the number of bytes of constant-buffer storage used by a type
        // layout
        size_t sizeInBytes = typeLayout->getSize();
        reflection_log << output_style(sizeInBytes);

        size_t tRegCount =
            typeLayout->getSize(SLANG_PARAMETER_CATEGORY_SHADER_RESOURCE);
        reflection_log << output_style(tRegCount);

        size_t arrayElementCount = typeLayout->getElementCount();
        reflection_log << output_style(arrayElementCount);

        slang::TypeLayoutReflection* elementTypeLayout =
            typeLayout->getElementTypeLayout();
        size_t arrayElementStride = typeLayout->getElementStride(category);
        reflection_log << output_style(arrayElementStride);

        reflection_log << std::endl;
    };

#define print(name)                                                 \
    reflection_log << #name << std::endl;                           \
    unsigned name##Count = shaderReflection->get##name##Count();    \
    for (unsigned pp = 0; pp < name##Count; pp++) {                 \
        auto name## = shaderReflection->get##name##ByIndex(pp);     \
        reflection_log << "    " << name##->getName() << std::endl; \
    }

    print(EntryPoint);
    print(TypeParameter);

    reflection_log.close();
}

#endif

TEST(SLANG, compileFromGlobalDXC)
{
    Slang::ComPtr<slang::IGlobalSession> globalSession = createGlobal();

    SlangCompileRequest* slangRequest = spCreateCompileRequest(globalSession);

    int targetIndex = slangRequest->addCodeGenTarget(SLANG_DXIL);
    spSetTargetFlags(
        slangRequest, targetIndex, SLANG_TARGET_FLAG_GENERATE_WHOLE_PROGRAM);
    int translationUnitIndex = spAddTranslationUnit(
        slangRequest, SLANG_SOURCE_LANGUAGE_SLANG, nullptr);

    auto profile_id = globalSession->findProfile("lib_6_5");

    auto shaderPath = SlangShaderCompiler::find_root(".") /
                      "usd/hd_USTC_CG_GL/resources/shaders/shader.slang";
    spAddTranslationUnitSourceFile(
        slangRequest,
        translationUnitIndex,
        shaderPath.generic_string().c_str());

    slangRequest->setTargetProfile(targetIndex, profile_id);

    const SlangResult compileRes = slangRequest->compile();

    if (SLANG_FAILED(compileRes)) {
        if (auto diagnostics = spGetDiagnosticOutput(slangRequest)) {
            std::cerr << "Error diagnostics: " << diagnostics;
        }
    }
    ASSERT_EQ(compileRes, SLANG_OK);

    if (SLANG_FAILED(compileRes)) {
        spDestroyCompileRequest(slangRequest);
    }
}
