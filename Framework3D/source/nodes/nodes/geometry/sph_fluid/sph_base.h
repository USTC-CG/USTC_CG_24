#pragma once 
#include <Eigen/Dense>
#include "particle_system.h"
#include <memory>
#include <chrono>

namespace USTC_CG::node_sph_fluid {
#define TIC(name) auto start_##name = std::chrono::high_resolution_clock::now();
#define TOC(name)                                                           \
    auto end_##name = std::chrono::high_resolution_clock::now();            \
    if (enable_time_profiling)                                              \
        std::cout << "Time taken by " << #name << ": "                      \
                  << std::chrono::duration_cast<std::chrono::microseconds>( \
                         end_##name - start_##name)                         \
                         .count()                                           \
                  << " microseconds\n";

class SPHBase {
   public:
    SPHBase() = default;

    SPHBase(const Eigen::MatrixXd& fluid_particle_X, 
        const Eigen::MatrixXd& boundary_particle_X, 
        const Vector3d& sim_area_min,
        const Vector3d& sim_area_max,
        const bool sim_2d);

    virtual ~SPHBase() = default;

    virtual void step();
    virtual void reset();

    inline Eigen::MatrixXd get_fluid_particle_X() const
    {
        return fluid_particle_X_;
    };
    inline Eigen::MatrixXd getVel() const
    {
        return fluid_particle_vel_;
    };

    // SPH kernel function: h is the support radius
    double W(const Eigen::Vector3d& r, double h);
    Eigen::Vector3d grad_W(const Eigen::Vector3d& r, double h);
    double W_zero(double h);

    // SPH functions
    virtual void compute_density();
    void init_boundary_particle_mass();  // TODO: should not be put here! 

    virtual Vector3d compute_viscosity_acceleration(
        const std::shared_ptr<Particle>& p,
        const std::shared_ptr<Particle>& q);
    virtual void compute_pressure_gradient_acceleration();
    virtual void compute_non_pressure_acceleration();
    virtual void compute_pressure();
    // virtual void compute_acceleration();
    virtual void check_collision(const std::shared_ptr<Particle>& p);

    virtual void advect();

    ParticleSystem& ps()
    {
        return ps_;
    }
    double& dt()
    {
        return dt_;
    }
    double& viscosity()
    {
        return viscosity_;
    }
    Vector3d& gravity()
	{
		return gravity_;
	}

    Eigen::MatrixXd get_boundary_X() const; 

    Vector3d gravity_ = Vector3d(0, 0, -9.8);

    // useful switches
    bool enable_debug_output = false;
    bool enable_time_profiling = false;
    bool enable_step_pause = false; 
    bool enable_sim_2d = false; 

    // -------- for display ----------
    MatrixXd get_vel_color_jet();


   protected:
    ParticleSystem ps_;
    double dt_ = 0.005;  // TODO: make this a parameter
    double viscosity_ = 0.03;

    Vector3d sim_area_min_, sim_area_max_;

    Eigen::MatrixXd init_fluid_particle_X_;
    Eigen::MatrixXd fluid_particle_X_;
    Eigen::MatrixXd fluid_particle_vel_;

};
}  // namespace USTC_CG::node_sph_fluid
