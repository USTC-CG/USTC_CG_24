#pragma once 
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include "utils.h"
#include "particle_system.h"
#include "sph_base.h"

namespace USTC_CG::node_sph_fluid {

using namespace Eigen;

class WCSPH : public SPHBase {
   public:
    WCSPH() = default;
    WCSPH(const MatrixXd& X, const Vector3d& box_min, const Vector3d& box_max);
    ~WCSPH() = default;

    void step() override;
    virtual void compute_density() override;

    inline double& stiffness() 
    {
        return stiffness_;
    };
    inline double& exponent()
    {
        return exponent_;
    };

   protected:
    double stiffness_ = 500.0;
    double exponent_ = 7.0;
};
}  // namespace USTC_CG::node_sph_fluid