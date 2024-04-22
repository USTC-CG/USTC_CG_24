#pragma once 
#include <Eigen/Dense>
#include <Eigen/Sparse>


namespace USTC_CG::node_mass_spring {
inline auto flatten = [&](const Eigen::MatrixXd& A) {
    Eigen::MatrixXd A_flatten = A.transpose();
    A_flatten.resize(A.rows() * A.cols(), 1);
    return A_flatten;
};
inline auto unflatten = [&](const Eigen::MatrixXd& A_flatten) {
    Eigen::MatrixXd A = A_flatten;
    A.resize(3, A_flatten.rows() / 3);
    A.transposeInPlace();
    return A;
};
}  // namespace USTC_CG::node_mass_spring