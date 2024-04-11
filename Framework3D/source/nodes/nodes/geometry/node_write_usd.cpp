// #define __GNUC__
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/primvarsAPI.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>

#include "GCore/Components/MaterialComponent.h"
#include "GCore/Components/MeshOperand.h"
#include "GCore/Components/XformComponent.h"
#include "Nodes/GlobalUsdStage.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"
#include "pxr/base/gf/matrix4d.h"
#include "pxr/base/gf/rotation.h"

namespace USTC_CG::node_write_usd {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Geometry>("Geometry");
    b.add_input<decl::String>("File Name").default_val("Default");
    b.add_input<decl::String>("Prim Path").default_val("geometry");
    b.add_input<decl::Int>("Time Code").default_val(0).min(0).max(240);
}

bool legal(const std::string& string)
{
    if (string.empty()) {
        return false;
    }
    if (std::find_if(string.begin(), string.end(), [](char val) {
            return val == '(' || val == ')' || val == '-';
        }) == string.end()) {
        return true;
    }
    return false;
}

static void node_exec(ExeParams params)
{
    auto file_name = params.get_input<std::string>("File Name");
    auto prim_path = params.get_input<std::string>("Prim Path");

    auto geometry = params.get_input<GOperandBase>("Geometry");

    auto mesh = geometry.get_component<MeshComponent>();

    auto t = params.get_input<int>("Time Code");
    pxr::UsdTimeCode time = pxr::UsdTimeCode(t);
    if (t == 0) {
        time = pxr::UsdTimeCode::Default();
    }

    auto& stage = GlobalUsdStage::global_usd_stage;
    if (!legal(prim_path.c_str())) {
        return;
    }
    // Here 'c_str' call is necessary since prim_path
    auto sdf_path = pxr::SdfPath(prim_path.c_str());

    if (mesh) {
        if (stage->GetPrimAtPath(sdf_path)) {
            stage->RemovePrim(sdf_path);
        }

        pxr::UsdGeomMesh usdgeom =
            pxr::UsdGeomMesh::Define(stage, pxr::SdfPath("/geom").AppendPath(sdf_path));
        if (usdgeom) {
            // Fill in the vertices and faces here
            usdgeom.CreatePointsAttr().Set(mesh->vertices, time);
            usdgeom.CreateFaceVertexCountsAttr().Set(mesh->faceVertexCounts, time);
            usdgeom.CreateFaceVertexIndicesAttr().Set(mesh->faceVertexIndices, time);

            usdgeom.CreateDoubleSidedAttr(pxr::VtValue(true));

            if (mesh->normals.size() > 0) {
                usdgeom.CreateNormalsAttr().Set(mesh->normals, time);
            }

            auto PrimVarAPI = pxr::UsdGeomPrimvarsAPI(usdgeom);

            if (mesh->texcoordsArray.size() > 0) {
                pxr::UsdGeomPrimvar primvar = PrimVarAPI.CreatePrimvar(
                    pxr::TfToken("st"), pxr::SdfValueTypeNames->TexCoord2fArray);
                primvar.Set(mesh->texcoordsArray, time);

                // Here only consider two modes
                if (mesh->texcoordsArray.size() == mesh->vertices.size()) {
                    primvar.SetInterpolation(pxr::UsdGeomTokens->vertex);
                }
                else {
                    primvar.SetInterpolation(pxr::UsdGeomTokens->faceVarying);
                }
            }

            if (mesh->displayColor.size()) {
                pxr::UsdGeomPrimvar colorPrimvar = PrimVarAPI.CreatePrimvar(
                    pxr::TfToken("displayColor"), pxr::SdfValueTypeNames->Color3fArray);
                colorPrimvar.SetInterpolation(pxr::UsdGeomTokens->vertex);
                colorPrimvar.Set(mesh->displayColor);
            }
        }

        auto xform_component = geometry.get_component<XformComponent>();
        if (xform_component) {
            // Transform
            assert(xform_component->translation.size() == xform_component->rotation.size());

            pxr::GfMatrix4d final_transform;
            final_transform.SetIdentity();

            for (int i = 0; i < xform_component->translation.size(); ++i) {
                pxr::GfMatrix4d t;
                t.SetTranslate(xform_component->translation[i]);
                pxr::GfMatrix4d s;
                s.SetScale(xform_component->scale[i]);

                pxr::GfMatrix4d r_x;
                r_x.SetRotate(pxr::GfRotation{ { 1, 0, 0 }, xform_component->rotation[i][0] });
                pxr::GfMatrix4d r_y;
                r_y.SetRotate(pxr::GfRotation{ { 0, 1, 0 }, xform_component->rotation[i][1] });
                pxr::GfMatrix4d r_z;
                r_z.SetRotate(pxr::GfRotation{ { 0, 0, 1 }, xform_component->rotation[i][2] });

                auto transform = r_x * r_y * r_z * s * t;
                final_transform = final_transform * transform;
            }

            auto xform_op = usdgeom.AddTransformOp();
            xform_op.Set(final_transform);
        }

        // Material and Texture
        auto material_component = geometry.get_component<MaterialComponent>();
        if (material_component) {
            if (legal(std::string(material_component->textures[0].c_str()))) {
                auto texture_name = std::string(material_component->textures[0].c_str());
                std::filesystem::path p = std::filesystem::path(texture_name).replace_extension();
                auto file_name = "texture" + p.filename().string();

                auto material_path_root = pxr::SdfPath("/TexModel");
                auto material_path = material_path_root.AppendPath(pxr::SdfPath(file_name + "Mat"));
                auto material_shader_path = material_path.AppendPath(pxr::SdfPath("PBRShader"));
                auto material_stReader_path = material_path.AppendPath(pxr::SdfPath("stReader"));
                auto material_texture_path =
                    material_path.AppendPath(pxr::SdfPath("diffuseTexture"));

                auto material = pxr::UsdShadeMaterial::Define(stage, material_path);
                auto pbrShader = pxr::UsdShadeShader::Define(stage, material_shader_path);

                pbrShader.CreateIdAttr(pxr::VtValue(pxr::TfToken("UsdPreviewSurface")));
                material.CreateSurfaceOutput().ConnectToSource(
                    pbrShader.ConnectableAPI(), pxr::TfToken("surface"));

                auto stReader = pxr::UsdShadeShader::Define(stage, material_stReader_path);
                stReader.CreateIdAttr(pxr::VtValue(pxr::TfToken("UsdPrimvarReader_float2")));

                auto diffuseTextureSampler =
                    pxr::UsdShadeShader::Define(stage, material_texture_path);

                diffuseTextureSampler.CreateIdAttr(pxr::VtValue(pxr::TfToken("UsdUVTexture")));
                diffuseTextureSampler
                    .CreateInput(pxr::TfToken("file"), pxr::SdfValueTypeNames->Asset)
                    .Set(pxr::SdfAssetPath(texture_name));
                diffuseTextureSampler
                    .CreateInput(pxr::TfToken("st"), pxr::SdfValueTypeNames->Float2)
                    .ConnectToSource(stReader.ConnectableAPI(), pxr::TfToken("result"));
                diffuseTextureSampler.CreateOutput(
                    pxr::TfToken("rgb"), pxr::SdfValueTypeNames->Float3);

                diffuseTextureSampler
                    .CreateInput(pxr::TfToken("wrapS"), pxr::SdfValueTypeNames->Token)
                    .Set(pxr::TfToken("mirror"));

                diffuseTextureSampler
                    .CreateInput(pxr::TfToken("wrapT"), pxr::SdfValueTypeNames->Token)
                    .Set(pxr::TfToken("mirror"));

                pbrShader.CreateInput(pxr::TfToken("diffuseColor"), pxr::SdfValueTypeNames->Color3f)
                    .ConnectToSource(diffuseTextureSampler.ConnectableAPI(), pxr::TfToken("rgb"));

                auto stInput = material.CreateInput(
                    pxr::TfToken("frame:stPrimvarName"), pxr::SdfValueTypeNames->Token);
                stInput.Set(pxr::TfToken("st"));

                stReader.CreateInput(pxr::TfToken("varname"), pxr::SdfValueTypeNames->Token)
                    .ConnectToSource(stInput);

                usdgeom.GetPrim().ApplyAPI(pxr::UsdShadeTokens->MaterialBindingAPI);
                pxr::UsdShadeMaterialBindingAPI(usdgeom).Bind(material);
            }
            else {
                // TODO: Throw something
            }
        }
    }
    else {
        if (stage->GetPrimAtPath(sdf_path)) {
            stage->RemovePrim(sdf_path);
        }
    }
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Write USD");
    strcpy_s(ntype.id_name, "geom_write_usd");
    ntype.ALWAYS_REQUIRED = true;

    geo_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_write_usd
