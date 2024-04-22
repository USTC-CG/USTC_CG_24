#pragma once 
#include "MassSpring.h"
#include <memory>

namespace USTC_CG::node_mass_spring {
// Impliment the Liu13's paper: https://tiantianliu.cn/papers/liu13fast/liu13fast.pdf
class FastMassSpring : public MassSpring {
   public:
    FastMassSpring() = default;
    ~FastMassSpring() = default; 

    FastMassSpring(const Eigen::MatrixXd& X, const EdgeSet& E);
    void step() override;
    unsigned max_iter = 100; // (HW Optional) add UI for this parameter

   protected:
    // Custom variables, like prefactorized A 
   
};
}  // namespace USTC_CG::node_mass_spring