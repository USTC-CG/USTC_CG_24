// #include <torch/torch.h>

#include <Eigen/Dense>

#include "comp_warping.h"

namespace USTC_CG
{
std::pair<int, int> Warping_NN::warping_f(int x, int y, int width, int height)
{
    int n = start_points_.size();
    if (n < 5)
    {
        printf("Please select at least 5 points\n");
    }
    return { 0, 0 };
    // using pytorch building neural network
}
}  // namespace USTC_CG