#include "FastMassSpring.h"
#include <iostream>


namespace USTC_CG::node_mass_spring {
FastMassSpring::FastMassSpring(const Eigen::MatrixXd& X, const EdgeSet& E) : MassSpring(X, E)
{
    // construct L and J at initialization
    std::cout << "init fast mass spring" << std::endl;

    unsigned n_vertices = X.rows();

    Eigen::SparseMatrix<double> A(n_vertices * 3, n_vertices * 3);
    A.setZero();

    // (HW Optional) precompute A and prefactorize
}

void FastMassSpring::step()
{
    // (HW Optional) Necessary preparation
    // ...
    for (unsigned iter = 0; iter < max_iter; iter++) {
        // (HW Optional)
        // local_step and global_step alternating solving
    }
}

}  // namespace USTC_CG::node_mass_spring
