#include <Eigen/Dense>

#include "comp_warping.h"

namespace USTC_CG
{
float Warping_RBF::phi(float x_1, float y_1, float x_2, float y_2, float r)
{
    return std::sqrt(
        (x_1 - x_2) * (x_1 - x_2) + (y_1 - y_2) * (y_1 - y_2) + r * r);
}

std::pair<int, int> Warping_RBF::warping_f(int x, int y, int width, int height)
{
    int n = start_points_.size();
    using namespace Eigen;
    MatrixXf P(n, n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            P(i, j) =
                phi(start_points_[i].x,
                    start_points_[i].y,
                    start_points_[j].x,
                    start_points_[j].y,
                    0);
        }
    }
    float r = P.minCoeff();
    MatrixXf A(2, 2);
    VectorXf b(2);
    VectorXf g(2);
    switch (n)
    {
        case 0: return { x, y }; break;
        case 1:
            return { x + end_points_[0].x - start_points_[0].x,
                     y + end_points_[0].y - start_points_[0].y };
            break;
        case 2:
        {
            MatrixXf T1(2, 2);
            T1 << start_points_[0].x, start_points_[0].y, start_points_[1].x,
                start_points_[1].y;
            MatrixXf T2(2, 2);
            T2 << end_points_[0].x, end_points_[0].y, end_points_[1].x,
                end_points_[1].y;
            A = T1.inverse() * T2;
            b = Vector2f::Zero();
            g = A * Vector2f(x, y) + b;
            return { static_cast<int>(g(0)), static_cast<int>(g(1)) };
            break;
        }
        case 3:
        {
            MatrixXf T1(2, 2);
            T1 << start_points_[0].x, start_points_[0].y, start_points_[1].x,
                start_points_[1].y;
            MatrixXf T2(2, 2);
            T2 << end_points_[0].x, end_points_[0].y, end_points_[1].x,
                end_points_[1].y;
            A = T1.inverse() * T2;
            b = T1.inverse() * Vector2f(start_points_[2].x, start_points_[2].y);
            g = A * Vector2f(x, y) + b;
            return { static_cast<int>(g(0)), static_cast<int>(g(1)) };
            break;
        }
        default:
        {
            MatrixXf T1{ MatrixXf::Zero(n + 3, n + 3) };
            VectorXf b1{ VectorXf::Zero(n + 3) };
            VectorXf b2{ VectorXf::Zero(n + 3) };
            VectorXf g1(n + 3);
            VectorXf g2(n + 3);
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    T1(i, j) =
                        phi(start_points_[i].x,
                            start_points_[i].y,
                            start_points_[j].x,
                            start_points_[j].y,
                            r);
                }
            }
            for (int i = 0; i < n; i++)
            {
                T1(i, n) = start_points_[i].x;
                T1(i, n + 1) = start_points_[i].y;
                T1(i, n + 2) = 1;

                T1(n, i) = start_points_[i].x;
                T1(n + 1, i) = start_points_[i].y;
                T1(n + 2, i) = 1;

                b1(i) = end_points_[i].x;
                b2(i) = end_points_[i].y;
            }

            g1 = T1.colPivHouseholderQr().solve(b1);
            g2 = T1.colPivHouseholderQr().solve(b2);
            float new_x = 0;
            float new_y = 0;
            for (int i = 0; i < n; i++)
            {
                new_x += g1(i) *
                         phi(x, y, start_points_[i].x, start_points_[i].y, r);
                new_y += g2(i) *
                         phi(x, y, start_points_[i].x, start_points_[i].y, r);
            }
            new_x += g1(n) * x + g1(n + 1) * y + g1(n + 2);
            new_y += g2(n) * x + g2(n + 1) * y + g2(n + 2);
            return { static_cast<int>(new_x), static_cast<int>(new_y) };
        }
    }
}
}  // namespace USTC_CG