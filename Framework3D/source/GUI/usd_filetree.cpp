// #define __GNUC__
#define IMGUI_DEFINE_MATH_OPERATORS

#include "GUI/usd_filetree.h"

#include <iostream>

#include "imgui.h"
#include "imgui_internal.h"
#include "pxr/base/gf/matrix4f.h"
#include "pxr/base/tf/ostreamMethods.h"
#include "pxr/base/vt/typeHeaders.h"
#include "pxr/base/vt/visitValue.h"
#include "pxr/usd/usd/attribute.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usd/property.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
class UsdFileViewerImpl {
   public:
    void BuildUI();
    void set_stage(const pxr::UsdStageRefPtr& ref);
    void ShowPrimInfo();

   private:
    pxr::SdfPath selected;

    void DrawChild(const pxr::UsdPrim& prim);
    pxr::UsdStageRefPtr stage;
};

void UsdFileViewerImpl::BuildUI()
{
    auto root = stage->GetPseudoRoot();
    ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg |
                            ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;
    if (ImGui::BeginTable("stage_table", 2, flags)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
        DrawChild(root);
        ImGui::EndTable();
    }
}

void UsdFileViewerImpl::set_stage(const pxr::UsdStageRefPtr& ref)
{
    stage = ref;
}

void UsdFileViewerImpl::ShowPrimInfo()
{
    using namespace pxr;
    ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg |
                            ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable;
    if (ImGui::BeginTable("table", 3, flags)) {
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Property Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableHeadersRow();
        UsdPrim prim = stage->GetPrimAtPath(selected);
        if (prim) {
            auto properties = prim.GetAttributes();

            for (auto&& attr : properties) {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("A");
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(attr.GetName().GetText());

                ImGui::TableSetColumnIndex(2);
                VtValue v;
                attr.Get(&v);
                auto s = VtVisitValue(v, [](auto&& v) { return TfStringify(v); });
                ImGui::TextUnformatted(s.c_str());
            }
        }
        ImGui::EndTable();
    }
}

void UsdFileViewerImpl::DrawChild(const pxr::UsdPrim& prim)
{
    auto flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth |
                 ImGuiTreeNodeFlags_OpenOnArrow;

    bool is_leaf = prim.GetChildren().empty();
    if (is_leaf) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet |
                 ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    if (prim.GetPath() == selected) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    bool open = ImGui::TreeNodeEx(prim.GetName().GetText(), flags);

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        selected = prim.GetPath();
    }

    ImGui::TableNextColumn();
    ImGui::TextUnformatted(prim.GetTypeName().GetText());

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        selected = prim.GetPath();
    }

    if (!is_leaf) {
        if (open) {


            for (const pxr::UsdPrim& child : prim.GetChildren()) {
                DrawChild(child);
            }
            ImGui::TreePop();
        }
    }
}

UsdFileViewer::UsdFileViewer()
{
    impl_ = std::make_unique<UsdFileViewerImpl>();
}

void UsdFileViewer::set_stage(pxr::UsdStageRefPtr root_stage)
{
    impl_->set_stage(root_stage);
}

void UsdFileViewer::ShowFileTree()
{
    ImGui::Begin("Stage Viewer", nullptr, ImGuiWindowFlags_None);
    impl_->BuildUI();
    ImGui::End();
}

UsdFileViewer::~UsdFileViewer()
{
}

void UsdFileViewer::ShowPrimInfo()
{
    ImGui::Begin("PrimInfo", nullptr, ImGuiWindowFlags_None);
    impl_->ShowPrimInfo();
    ImGui::End();
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
