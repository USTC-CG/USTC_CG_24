#include "sph_base.h"
#include <cmath>
#define M_PI 3.14159265358979323846
#include <omp.h>
#include <iostream>
#include "colormap_jet.h"

namespace USTC_CG::node_sph_fluid {
using namespace Eigen;
using Real = double;

SPHBase::SPHBase(const Eigen::MatrixXd& X, const Vector3d& box_min, const Vector3d& box_max)
    : init_X_(X),
      X_(X),
      vel_(MatrixXd::Zero(X.rows(), X.cols())),
      box_max_(box_max),
      box_min_(box_min),
      ps_(X, box_min, box_max)
{
}

// ----------------- SPH kernal function and its spatial derivatives, no need to modify -----------------
double SPHBase::W(const Eigen::Vector3d& r, double h)
{
    double h3 = h * h * h;
    double m_k = 8.0 / (M_PI * h3);
    double m_l = 48.0 / (M_PI * h3); 
    const double q = r.norm() / h;
    double result = 0.;

    if (q <= 1.0) {
        if (q <= 0.5) {
            const Real q2 = q * q;
            const Real q3 = q2 * q;
            result = m_k * (6.0 * q3 - 6.0 * q2 + 1.0);
        }
        else {
            result = m_k * (2.0 * pow(1.0 - q, 3.0));
        }
    }
    return result;
}

double SPHBase::W_zero(double h)
{
    double h3 = h * h * h;
    double m_k = 8.0 / (M_PI * h3);
    return m_k;
}

Vector3d SPHBase::grad_W(const Vector3d& r, double h)
{
    double h3 = h * h * h;
    double m_k = 8.0 / (M_PI * h3);
    double m_l = 48.0 / (M_PI * h3);

    const double rl = r.norm();
    const double q = rl / h;
    Vector3d result = Vector3d::Zero();

    if (q <= 1.0 && rl > 1e-9) {
        Vector3d grad_q = r / rl;
        if (q <= 0.5) {
            result = m_l * q * (3.0 * q - 2.0) * grad_q;
        }
        else {
            const Real factor = 1.0 - q;
            result = -m_l * factor * factor * grad_q;
        }
    }
    return result;
}
// ---------------------------------------------------------------------------------------


void SPHBase::compute_density()
{
    // (HW TODO) Traverse all particles to compute each particle's density
    // (Optional) This operation can be done in parallel using OpenMP 
    for (auto& p : ps_.particles()) {

        // ... necessary initialization of particle p's density here  

        // Then traverse all neighbor fluid particles of p
        for (auto& q : p->neighbors()) {

            // ... compute the density contribution from q to p

        }
    }
}

void SPHBase::compute_pressure()
{
    // Not implemented, should be implemented in children classes WCSPH, IISPH, etc. 
}

void SPHBase::compute_non_pressure_acceleration()
{
    // (HW TODO) Traverse all particles to compute each particle's non-pressure acceleration 
    for (auto& p : ps_.particles()) {

        // necessary code here to compute particle p's acceleration include gravity and viscosity
        // We do not consider surface tension in this assignment, but you can add it if you like

        //for (auto& q : p->neighbors()) {
        // 
        // Prompt: use the "compute_viscosity_acceleration" function to compute the viscosity acceleration between p and q"
        // 
        //}


    }
}

// compute viscosity acceleration between two particles
Vector3d SPHBase::compute_viscosity_acceleration(
    const std::shared_ptr<Particle>& p,
    const std::shared_ptr<Particle>& q)
{
    auto v_ij = p->vel() - q->vel();
    auto x_ij = p->x() - q->x();
    Vector3d grad = grad_W(p->x() - q->x(), ps_.h());

    // Vector3d laplace_v = ... 

    //return this->viscosity_ * laplace_v;

    return Vector3d::Zero();
}

// Traverse all particles and compute pressure gradient acceleration
void SPHBase::compute_pressure_gradient_acceleration()
{
    for (auto& p : ps_.particles()) {
        // (HW TODO) Traverse all particles and compute each particle's acceleration from pressure gradient force
    }
}

void SPHBase::step()
{
    // Not implemented, should be implemented in children classes WCSPH, IISPH, etc. 
}


void SPHBase::advect()
{
    for (auto& p : ps_.particles())  
    {

        // ---------------------------------------------------------
        // (HW TODO) Implement the advection step of each particle
        // Remember to check collision after advection

        // Your code here 

        // ---------------------------------------------------------
        vel_.row(p->idx()) = p->vel().transpose();
        X_.row(p->idx()) = p->x().transpose();
    }
}

// ------------------------------- helper functions -----------------------
// Basic collision detection and process
void SPHBase::check_collision(const std::shared_ptr<Particle>& p)
{
    // coefficient of restitution, you can make this parameter adjustable in the UI 
    double restitution = 0.2; 

    // add epsilon offset to avoid particles sticking to the boundary
    Vector3d eps_ = 0.0001 * (box_max_ - box_min_);

    for (int i = 0; i < 3; i++) {
        if (p->x()[i] < box_min_[i]) {
            p->x()[i] = box_min_[i] + eps_[i];
            p->vel()[i] = -restitution * p->vel()[i];
        }
        if (p->x()[i] > box_max_[i]) {
            p->x()[i] = box_max_[i] - eps_[i];
            p->vel()[i] = -restitution * p->vel()[i];
        }
    }
}

// For display
MatrixXd SPHBase::get_vel_color_jet()
{
    MatrixXd vel_color = MatrixXd::Zero(vel_.rows(), 3);
    double max_vel_norm = vel_.rowwise().norm().maxCoeff();
    double min_vel_norm = vel_.rowwise().norm().minCoeff();

    auto c = colormap_jet;

    for (int i = 0; i < vel_.rows(); i++) {
        double vel_norm = vel_.row(i).norm();
        int idx = 0;
        if (fabs(max_vel_norm - min_vel_norm) > 1e-6) {
            idx = static_cast<int>(
                floor((vel_norm - min_vel_norm) / (max_vel_norm - min_vel_norm) * 255));
        }
        vel_color.row(i) << c[idx][0], c[idx][1], c[idx][2];
    }
    return vel_color;
}

void SPHBase::reset()
{
    X_ = init_X_;
    vel_ = MatrixXd::Zero(X_.rows(), X_.cols());

    for (auto& p : ps_.particles()) {
        p->vel() = Vector3d::Zero();
        p->x() = init_X_.row(p->idx()).transpose();
    }
}

// ---------------------------------------------------------------------------------------
}  // namespace USTC_CG::node_sph_fluid