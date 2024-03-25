//#define __GNUC__

#include "GUI/usd_filetree.h"

#include "imgui.h"
#include "pxr/usd/usd/primRange.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
class UsdFileViewerImpl {
   public:
    void BuildUI();
    void set_stage(const pxr::UsdStageRefPtr& ref);

   private:
    void DrawChild(const pxr::UsdPrim& prim)
    {
        if (prim.GetChildren().empty()) {
            ImGui::Text(prim.GetName().GetText());
        }
        else {
            if (ImGui::TreeNodeEx(prim.GetName().GetText(), ImGuiTreeNodeFlags_DefaultOpen)) {
                for (const pxr::UsdPrim& child : prim.GetChildren()) {
                    DrawChild(child);
                }
                ImGui::TreePop();
            }
        }
    }
    pxr::UsdStageRefPtr stage;
};

void UsdFileViewerImpl::BuildUI()
{
    auto root = stage->GetPseudoRoot();
    DrawChild(root);
}

void UsdFileViewerImpl::set_stage(const pxr::UsdStageRefPtr& ref)
{
    stage = ref;
}

UsdFileViewer::UsdFileViewer()
{
    impl_ = std::make_unique<UsdFileViewerImpl>();
}

void UsdFileViewer::set_stage(pxr::UsdStageRefPtr root_stage)
{
    impl_->set_stage(root_stage);
}

void UsdFileViewer::BuildUI()
{
    ImGui::Begin("Stage Viewer", nullptr, ImGuiWindowFlags_None);
    impl_->BuildUI();
    ImGui::End();
}

UsdFileViewer::~UsdFileViewer()
{
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
