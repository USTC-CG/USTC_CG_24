#include "iisph.h"
#include <iostream>

namespace USTC_CG::node_sph_fluid {

using namespace Eigen;

IISPH::IISPH(const MatrixXd& X, const Vector3d& box_min, const Vector3d& box_max)
    : SPHBase(X, box_min, box_max)
{
    // (HW TODO) Feel free to modify this part to remove or add necessary member variables
    predict_density_ = VectorXd::Zero(ps_.particles().size());
    aii_ = VectorXd::Zero(ps_.particles().size());
    Api_ = VectorXd::Zero(ps_.particles().size());
    last_pressure_ = VectorXd::Zero(ps_.particles().size());
}

void IISPH::step()
{
    // (HW Optional)
}

void IISPH::compute_pressure()
{
    // (HW Optional) solve pressure using relaxed Jacobi iteration 
    // Something like this: 

    //double threshold = 0.001;
    //for (unsigned iter = 0; iter < max_iter_; iter++) {
    //    double avg_density_error = pressure_solve_iteration();
    //    if (avg_density_error < threshold)
    //        break;
    //}
}

void IISPH::predict_advection()
{
    // (HW Optional)
    // predict new density based on non-pressure forces,
    // compute necessary variables for pressure solve, etc. 

    // Note: feel free to remove or add functions based on your need,
    // you can also rename this function. 
}

double IISPH::pressure_solve_iteration()
{
    // (HW Optional)   
    // One step iteration to solve the pressure poisson equation of IISPH

    return 1.0; 
}

// ------------------ helper function, no need to modify ---------------------
void IISPH::reset()
{
    SPHBase::reset();

    predict_density_ = VectorXd::Zero(ps_.particles().size());
    aii_ = VectorXd::Zero(ps_.particles().size());
    Api_ = VectorXd::Zero(ps_.particles().size());
    last_pressure_ = VectorXd::Zero(ps_.particles().size());
}
}  // namespace USTC_CG::node_sph_fluid