#pragma once
#include <iostream>
#include <sstream>
#include <string>

#include "USTC_CG.h"
#include "pxr/base/gf/matrix2f.h"
#include "pxr/base/gf/matrix3f.h"
#include "pxr/base/gf/matrix4f.h"
#include "pxr/base/gf/vec2f.h"
#include "pxr/base/gf/vec3f.h"
#include "pxr/base/gf/vec4f.h"
#include "pxr/imaging/garch/glApi.h"
USTC_CG_NAMESPACE_OPEN_SCOPE

class Shader {
   public:
    unsigned int ID;
    // constructor generates the shader on the fly

    Shader(const char *vertexPath, const char *fragmentPath);
    // activate the shader

    void use() const;
    // utility uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, const pxr::GfVec2f &value) const;
    void setVec2(const std::string &name, float x, float y) const;
    void setVec3(const std::string &name, const pxr::GfVec3f &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec4(const std::string &name, const pxr::GfVec4f &value) const;
    void setVec4(const std::string &name, float x, float y, float z, float w) const;
    void setMat2(const std::string &name, const pxr::GfMatrix2f &mat) const;
    void setMat3(const std::string &name, const pxr::GfMatrix3f &mat) const;
    void setMat4(const std::string &name, const pxr::GfMatrix4f &mat) const;

    std::string get_error()
    {
        return error_string;
    }

private:
    std::string error_string;
    std::ostringstream error_stream;

    // utility function for checking shader compilation/linking errors.

    void checkCompileErrors(GLuint shader, std::string type);
};
USTC_CG_NAMESPACE_CLOSE_SCOPE
