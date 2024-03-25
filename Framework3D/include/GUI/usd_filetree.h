#pragma once
#include <memory>

#include "USTC_CG.h"
#include "pxr/usd/usd/stage.h"


USTC_CG_NAMESPACE_OPEN_SCOPE
class UsdFileViewerImpl;

class USTC_CG_API UsdFileViewer {
   public:
    explicit UsdFileViewer();

    void set_stage(pxr::UsdStageRefPtr root_stage);
    void ShowFileTree();

    ~UsdFileViewer();
    void render();
    void ShowPrimInfo();

protected:
    std::unique_ptr<UsdFileViewerImpl> impl_;
};
USTC_CG_NAMESPACE_CLOSE_SCOPE