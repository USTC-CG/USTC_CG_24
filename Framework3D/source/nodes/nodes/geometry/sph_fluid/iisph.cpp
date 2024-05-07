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
    ps_.assign_particles_to_cells(); 
    ps_.searchNeighbors(); 

    compute_density();
    compute_non_pressure_acceleration();

    for(auto& p : ps_.particles())
    {
        if (p->type() == Particle::BOUNDARY) {
			continue;
		}
        p->vel() += p->acceleration() * dt_;
        p->acceleration() = Vector3d::Zero();
         //p->pressure() = 0.0;  // clear pressure 
        p->pressure() = 0.5 * last_pressure_[p->idx()]; // warm init
    }

    // compute predict_pressure and aii 
    predict_advection(); 
    compute_pressure(); 
    compute_pressure_gradient_acceleration();

    advect(); 
}

void IISPH::compute_pressure()
{
    double threshold = 0.001; 
    for(unsigned iter = 0; iter < max_iter_; iter++)
    {
        double avg_density_error = pressure_solve_iteration();
        if(enable_debug_output)
        {
            std::cout << YellowHead() << "[iter " << iter << " ] avg density error = " << avg_density_error << ColorTail() << std::endl;
        }
        if(avg_density_error < threshold)
            break;
    }
}

void IISPH::predict_advection()
{
    MatrixXd dii = MatrixXd::Zero(ps_.particles().size(), 3);
    for(auto& p : ps_.particles())
    {
        if (p->type() == Particle::BOUNDARY) {
            continue; 
        }
        predict_density_[p->idx()] = p->density();

        for(auto& q : p->neighbors())
        {
            // 
            // density_i* = density_i - dt * density_i * div velocity
            //          = density_i - dt * density_i \sum mass_j / density_j (v_j - v_i) \cdot grad W_ij
            //          \approx density + dt *  \sum mass_j (v_i - v_j) \cdot grad W_ij
            auto grad = grad_W(p->x() - q->x(), ps_.h());
            predict_density_[p->idx()] += dt_ * ps_.mass() *(p->vel() - q->vel()).dot(grad);

            dii.row(p->idx()) += ps_.mass() / p->density() / p->density() * grad; 
        }
    }

    // ------------------- compute pressure acceleration -------------------
    for(auto &p: ps_.particles())
    {
        if (p->type() == Particle::BOUNDARY) {
            continue; 
        }
        auto density2 = p->density() * p->density();
        auto tmp_p = ps_.mass() / density2;
        aii_[p->idx()] = 0.0;
        for(auto& q : p->neighbors())
        {
            Vector3d grad_W_ji = grad_W(q->x() - p->x(), ps_.h()); // Note: W_{ji} instead of W_{ij}
            Vector3d grad_W_ij = - grad_W_ji;
            Vector3d d_ji =  ps_.mass() / p->density() / p->density() * grad_W_ji;
            Vector3d d_ii_ = dii.row(p->idx()); 
            aii_[p->idx()] += - ps_.mass() * (d_ii_ - d_ji).dot(grad_W_ij);
        }
    }
}

double IISPH::pressure_solve_iteration()
{
    // now we have already updated the particle velocities with non-pressure acc 
    // Where do we store this new density? 
    // To begin with, we can directly use a vector<double>
    // A more complex way is to inherent Particle class to customize the physical properties
    // that carried by the particles.
    // ------------------- compute pressure acceleration -------------------
    MatrixXd acc_pressure = MatrixXd::Zero(ps_.particles().size(), 3);
    for(auto &p: ps_.particles())
    {
        if (p->type() == Particle::BOUNDARY) {
            continue; 
        }
        auto density2 = p->density() * p->density();
        auto tmp_p = p->pressure() / density2;
        for(auto& q : p->neighbors())
        {
            auto tmp_q = q->pressure() / (q->density() * q->density());
            Vector3d grad = grad_W(p->x() - q->x(), ps_.h());
            acc_pressure.row(p->idx()) += -ps_.mass() * (tmp_p + tmp_q) * grad;
        }
    }

    // ---------------------------------------------------------------
    for(auto &p : ps_.particles())
    {
        if (p->type() == Particle::BOUNDARY) {
            continue; 
        }
        double Api = 0.0;
        for(auto& q : p->neighbors())
        {
            Vector3d grad = grad_W(p->x() - q->x(), ps_.h());
            Api += ps_.mass() * (acc_pressure.row(p->idx()) - acc_pressure.row(q->idx())).dot(grad);
        }
        Api_[p->idx()] = Api; // for debugging, in deploy no need to store  

        double s = (ps_.density0() - predict_density_[p->idx()]) / dt_ / dt_;
        if(fabs(aii_[p->idx()]) > 1e-9)
            p->pressure() = std::max(0.0, p->pressure() + omega_ / aii_[p->idx()] * (s - Api)); 
        else 
            p->pressure() = 0.0;
        last_pressure_[p->idx()] = p->pressure();
    }
    
    // ------------------------------------------------------------------------
    if(enable_debug_output)
    {
        unsigned count = 0; 
        for(int i = 0; i < ps_.particles().size(); i++)
        {
            auto p = ps_.particles()[i];
			if (p->type() == Particle::BOUNDARY) {
				continue; 
			}
            if(p->density() > ps_.density0())
            {
                std::cout << "[ " << i << " ]pressure: " << p->pressure() << " ";
                std::cout << "aii: " << aii_[i] << " "; 
                std::cout << "Api: " << Api_[i] << " "; 
                std::cout << "predict_density: " << predict_density_[i] << " "; 
                std::cout << "final density: " << predict_density_[i] + dt_* dt_* Api_[i] << " "; 
                std::cout << "density: " << p->density() << std::endl;
                count ++; 
                if(count > 2)
                    break;
            }
        }
        if(count == 0)
            std::cout << "No particles with density > density0" << std::endl;
    }
    // ------------------------------------------------------------------------

    // compute avg_density_error here, do we need to update position here? No.
    double avg_density_err = 0.0;
    for(auto& p : ps_.particles())
    {
        if (p->type() == Particle::BOUNDARY) {
            continue; 
        }
        if(p->pressure() > 0.0)
        {
            double final_density_p = predict_density_[p->idx()] + dt_ * dt_ * Api_[p->idx()];
            avg_density_err += fabs(final_density_p - ps_.density0()); 
        }
    }
    avg_density_err /= ps_.num_fluid_particles(); 
    avg_density_err /= ps_.density0(); 

    return avg_density_err; 
}


void IISPH::reset()
{
    SPHBase::reset();
    predict_density_ = VectorXd::Zero(ps_.particles().size());
    aii_ = VectorXd::Zero(ps_.particles().size());

    // for debugging 
    Api_ = VectorXd::Zero(ps_.particles().size());
    last_pressure_ = VectorXd::Zero(ps_.particles().size());
}

} // namespace USTC_CG::node_sph_fluid
