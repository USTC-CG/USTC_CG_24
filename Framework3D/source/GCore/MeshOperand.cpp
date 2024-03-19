// #undef _MSC_VER

#include "GCore/Components/MeshOperand.h"

#include "GCore/GOP.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
std::string MeshComponent::to_string() const
{
    std::ostringstream out;
    // Loop over the faces and vertices and print the data
    out << "Topology component. "
        << "Vertices count " << vertices.size() << ". Face vertices count "
        << faceVertexCounts.size() << ". Face vertex indices " << faceVertexIndices.size() << ".";
    return out.str();
}

GOperandComponentHandle MeshComponent::copy(GOperandBase* operand) const
{
    auto ret = std::make_shared<MeshComponent>(operand);

    // This is fast because the VtArray has the copy on write mechanism
    ret->vertices = this->vertices;
    ret->faceVertexCounts = this->faceVertexCounts;
    ret->faceVertexIndices = this->faceVertexIndices;
    ret->texcoordsArray = this->texcoordsArray;
    ret->normals = this->normals;
    ret->displayColor = this->displayColor;
    return ret;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
