#pragma once

#include "view/comp_image.h"

namespace USTC_CG
{
class CompWarping : public ImageEditor
{
   public:
    explicit CompWarping(const std::string& label, const std::string& filename);
    virtual ~CompWarping() noexcept = default;

    // Simple edit functions
    void invert();
    void mirror(bool is_horizontal, bool is_vertical);
    void gray_scale();
    void restore();

   private:
    std::shared_ptr<Image> back_up_;
};

}  // namespace USTC_CG