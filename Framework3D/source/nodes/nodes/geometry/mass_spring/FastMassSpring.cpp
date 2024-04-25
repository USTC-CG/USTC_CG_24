#include "FastMassSpring.h"
#include <iostream>


namespace USTC_CG::node_mass_spring {
FastMassSpring::FastMassSpring(const Eigen::MatrixXd& X, const EdgeSet& E, const float stiffness): 
MassSpring(X, E){
    // construct L and J at initialization
    std::cout << "init fast mass spring" << std::endl;

    unsigned n_vertices = X.rows();
    this->stiffness = stiffness; 

    Eigen::SparseMatrix<double> A(n_vertices * 3, n_vertices * 3);
    A.setZero();

    // (HW Optional) precompute A and prefactorize
    // Note: one thing to take care of: A is related with stiffness, if stiffness changes, A need to be recomputed
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
