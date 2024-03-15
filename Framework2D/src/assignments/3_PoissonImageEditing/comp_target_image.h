#pragma once

#include "comp_source_image.h"
#include "view/comp_image.h"

namespace USTC_CG
{
class CompTargetImage : public ImageEditor
{
   public:
    // HW3_TODO: Add more types of cloning
    enum CloneType
    {
        kDefault = 0,
        kPaste = 1,
        kSeamless = 2
    };

    explicit CompTargetImage(
        const std::string& label,
        const std::string& filename);
    virtual ~CompTargetImage() noexcept = default;

    void draw() override;
    // Bind the source image component
    void set_source(std::shared_ptr<CompSourceImage> source);
    // Enable realtime updating
    void set_realtime(bool flag);
    void restore();

    // HW3_TODO: Add more types of cloning
    void set_paste();
    void set_seamless();
    // The clone function
    void clone();

   private:
    // Store the original image data
    std::shared_ptr<Image> back_up_;
    // Source image
    std::shared_ptr<CompSourceImage> source_image_;
    CloneType clone_type_ = kDefault;

    ImVec2 mouse_position_;
    bool edit_status_ = false;
    bool flag_realtime_updating = false;
};

}  // namespace USTC_CG