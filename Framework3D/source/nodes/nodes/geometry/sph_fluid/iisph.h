#pragma once 
#include "utils.h"
#include "particle_system.h"
#include "sph_base.h"
#include <Eigen/Dense>

namespace USTC_CG::node_sph_fluid {

using namespace Eigen;

class IISPH : public SPHBase {
   public:
    IISPH() = default;
    IISPH(const MatrixXd& fluid_particles_X, const MatrixXd& boundary_particle_X,
            const Vector3d& sim_area_min, const Vector3d& sim_area_max,
        const bool sim_2d);
    ~IISPH() = default;

    void step() override;
    void compute_pressure() override;

    double pressure_solve_iteration();
    void predict_advection();

    void advect() override; 
    void reset() override;

    inline int& max_iter()
    {
        return max_iter_;
    }
    inline double& omega()
    {
        return omega_;
    }

   protected:
    int max_iter_ = 100;
    double omega_ = 0.3;

    // (HW TODO) Feel free to modify this part to remove or add necessary member variables
    VectorXd predict_density_;
    VectorXd aii_;
    VectorXd Api_;  
    MatrixXd dii_;  
    VectorXd last_pressure_;
};
}  // namespace USTC_CG::node_sph_fluid