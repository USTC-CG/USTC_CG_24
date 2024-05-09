#include "wcsph.h"
#include <iostream>
using namespace Eigen;

namespace USTC_CG::node_sph_fluid {


WCSPH::WCSPH(const MatrixXd& X, const MatrixXd& boundary_particle_X, 
    const Vector3d& sim_area_min,
    const Vector3d& sim_area_max,
    const bool sim_2d)
    : SPHBase(X, boundary_particle_X, sim_area_min, sim_area_max, sim_2d)
{
}


void WCSPH::compute_density()
{
    // Traverse all particles
    // This operation can be done in parallel using OpenMP
    #pragma omp parallel for 
    //for (auto& p : ps_.particles()) {
    for (int i = 0; i < ps_.particles().size(); i++) {
        auto p = ps_.particles()[i];
        if (p->is_boundary()) {
            continue;
        }
        p->density() = p->mass() * W_zero(ps_.h());
        // Then traverse all neighbors of p
        for (auto& q : p->neighbors()) {
            p->density() += q->mass() * W(p->x() - q->x(), ps_.h());
        }
       
        p->density() = std::max(p->density(),  ps_.density0());
        p->pressure() = std::max(0.0, stiffness_ * (std::pow(p->density() / ps_.density0(), exponent_) - 1));
    }
}

void WCSPH::step()
{
    TIC(step)

    TIC(1_assign_to_cell)
    ps_.assign_particles_to_cells();
    TOC(1_assign_to_cell)

    TIC(2_search_neighbor)
    ps_.search_neighbors();
    TOC(2_search_neighbor)

    TIC(3_compute_density)
    compute_density();  // merge density and pressure computation
    TOC(3_compute_density)

    TIC(4)
    compute_non_pressure_acceleration();
    TOC(4)

    TIC(5)
    compute_pressure_gradient_acceleration();
    TOC(5)

    TIC(6)
    advect();
    TOC(6)

    //for (auto& p : ps_.particles()) {
    //    check_collision(p);
    //}

    TOC(step)
    if (enable_step_pause)
    {
		std::cout << YellowHead() << "Press Enter to continue..." << ColorTail() << std::endl;
		std::cin.get();
	}

}
}  // namespace USTC_CG::node_sph_fluid