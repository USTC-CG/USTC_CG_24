#pragma once

#include <Eigen/Dense>

#include "view/comp_image.h"

namespace USTC_CG
{
class Warping
{
   protected:
    std::vector<ImVec2> start_points_, end_points_;

    ImVec2 start_, end_;
    bool flag_enable_selecting_points_ = false;
    bool draw_status_ = false;

   public:
    virtual std::pair<int, int> warping_f(int x, int y, int width, int height);

    Warping(
        std::vector<ImVec2> start_points_,
        std::vector<ImVec2> end_points_,

        ImVec2 start_,
        ImVec2 end_,
        bool flag_enable_selecting_points_ = false,
        bool draw_status_ = false)
        : start_points_(start_points_),
          end_points_(end_points_),
          start_(start_),
          end_(end_),
          flag_enable_selecting_points_(flag_enable_selecting_points_),
          draw_status_(draw_status_)
    {
    }
};

class Warping_IDW : public Warping
{
   public:
    Warping_IDW(
        std::vector<ImVec2> start_points_,
        std::vector<ImVec2> end_points_,

        ImVec2 start_,
        ImVec2 end_,
        bool flag_enable_selecting_points_ = false,
        bool draw_status_ = false)
        : Warping(
              start_points_,
              end_points_,
              start_,
              end_,
              flag_enable_selecting_points_,
              draw_status_)
    {
    }

    std::pair<int, int> warping_f(int x, int y, int width, int height) override;
    // void restore() override;
};

class Warping_RBF : public Warping
{
   public:
    Warping_RBF(
        std::vector<ImVec2> start_points_,
        std::vector<ImVec2> end_points_,

        ImVec2 start_,
        ImVec2 end_,
        bool flag_enable_selecting_points_ = false,
        bool draw_status_ = false)
        : Warping(
              start_points_,
              end_points_,
              start_,
              end_,
              flag_enable_selecting_points_,
              draw_status_)
    {
    }

    float phi(float x_0, float y_0, float x_1, float y_1, float r_1);

    std::pair<int, int> warping_f(int x, int y, int width, int height) override;
    // void restore() override;

   private:
};

class Warping_Fisheye : public Warping
{
   public:
    Warping_Fisheye(
        std::vector<ImVec2> start_points_,
        std::vector<ImVec2> end_points_,

        ImVec2 start_,
        ImVec2 end_,
        bool flag_enable_selecting_points_ = false,
        bool draw_status_ = false)
        : Warping(
              start_points_,
              end_points_,
              start_,
              end_,
              flag_enable_selecting_points_,
              draw_status_)
    {
    }

    std::pair<int, int> warping_f(int x, int y, int width, int height) override;
    // void restore() override;
};

class Warping_NN : public Warping
{
   public:
    Warping_NN(
        std::vector<ImVec2> start_points_,
        std::vector<ImVec2> end_points_,

        ImVec2 start_,
        ImVec2 end_,
        bool flag_enable_selecting_points_ = false,
        bool draw_status_ = false)
        : Warping(
              start_points_,
              end_points_,
              start_,
              end_,
              flag_enable_selecting_points_,
              draw_status_)
    {
    }

    std::pair<int, int> warping_f(int x, int y, int width, int height) override;
    // void restore() override;
};

// Image component for warping and other functions
class CompWarping : public ImageEditor
{
   public:
    std::shared_ptr<Warping> my_warping = nullptr;
    explicit CompWarping(const std::string& label, const std::string& filename);
    virtual ~CompWarping() noexcept = default;

    void draw() override;

    // Simple edit functions
    void invert();
    void mirror(bool is_horizontal, bool is_vertical);
    void gray_scale();
    void warping();
    void restore();

    // Point selecting interaction
    void enable_selecting(bool flag);
    void select_points();
    void init_selections();

    // enum class WarpingType
    enum class WarpingType
    {
        FISHEYE,
        IDW,
        RBF
    };

    // creat parent class of warping.

    // std::pair<int, int> warping_f(int x, int y, int width, int height);

    // Store the original image data
    std::shared_ptr<Image> back_up_;
    // The selected point couples for image warping
    std::vector<ImVec2> start_points_, end_points_;

    ImVec2 start_, end_;
    bool flag_enable_selecting_points_ = false;
    bool draw_status_ = false;

   private:
};

}  // namespace USTC_CG
   // namespace USTC_CG
