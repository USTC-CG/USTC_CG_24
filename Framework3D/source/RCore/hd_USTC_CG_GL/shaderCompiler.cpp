#include "shaderCompiler.h"
#ifdef _WIN32
#include <codecvt>
#endif
#include <filesystem>
#include <fstream>

#include "USTC_CG.h"
#include "Utils/Logging/Logging.h"
#include "slang.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

std::filesystem::path SlangShaderCompiler::find_root(const std::filesystem::path& p)
{
    auto mark_prelude_name = "/slang/prelude/slang-cuda-prelude.h";
    auto current = absolute(p);

    while (!exists(current / mark_prelude_name)) {
        if (current.has_parent_path()) {
            current = current.parent_path();
        }
        else {
            logging("CUDA Prelude not found.", Error);
            return "";
        }
    }
    // std::cerr << current.generic_string() << std::endl;

    return current;
}

SlangResult SlangShaderCompiler::addCUDAPrelude(slang::IGlobalSession* session)
{
    std::filesystem::path includePath = ".";

    auto root = find_root(includePath);

    auto prelude_name = "/slang/prelude/slang-cuda-prelude.h";
    std::ostringstream prelude;
    prelude << "#include \"" << root.generic_string() + prelude_name << "\"\n\n";

    // std::cerr << prelude.str() << std::endl;
    session->setLanguagePrelude(SLANG_SOURCE_LANGUAGE_CUDA, prelude.str().c_str());
    return SLANG_OK;
}

SlangResult SlangShaderCompiler::addOptiXHeaderInclude(SlangCompileRequest* slangRequest)
{
    auto optix_path = find_root(".") / "usd/HD_USTC_CG_HWRT/resources/optix/";

    auto optix_path_name = "-I" + optix_path.generic_string();

    // Inclusion in prelude should be passed to down stream compilers.....
    const char* args[] = { "-Xnvrtc...", optix_path_name.c_str(), "-X." };
    return slangRequest->processCommandLineArguments(args, sizeof(args) / sizeof(const char*));
}

SlangResult SlangShaderCompiler::addOptiXSupportPreDefine(SlangCompileRequest* slangRequest)
{
    // However, this predefine remains to nvrtc...
    slangRequest->addPreprocessorDefine("SLANG_CUDA_ENABLE_OPTIX", "1");
    return SLANG_OK;
}

SlangResult SlangShaderCompiler::addOptiXSupport(SlangCompileRequest* slangRequest)
{
    addOptiXSupportPreDefine(slangRequest);
    return addOptiXHeaderInclude(slangRequest);
}

void SlangShaderCompiler::save_file(const std::string& filename, const char* data)
{
    std::ofstream file(filename);

    file << std::string(data);
    file.close();
}

USTC_CG_NAMESPACE_CLOSE_SCOPE