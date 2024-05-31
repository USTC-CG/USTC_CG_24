#pragma once
#include <filesystem>
#include "USTC_CG.h"
#include "slang.h"
#include "api.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
class HD_USTC_CG_GL_API SlangShaderCompiler {
   public:
    static std::filesystem::path find_root(const std::filesystem::path& p);
    static SlangResult addCUDAPrelude(slang::IGlobalSession* session);

    static SlangResult addOptiXHeaderInclude(SlangCompileRequest* slangRequest); 
    static SlangResult addOptiXSupportPreDefine(SlangCompileRequest* slangRequest);
    static SlangResult addOptiXSupport(SlangCompileRequest* slangRequest);

    static void save_file(const std::string& filename, const char* data);
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
