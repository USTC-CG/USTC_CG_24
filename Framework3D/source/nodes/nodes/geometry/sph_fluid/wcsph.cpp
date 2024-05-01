#include "wcsph.h"
#include <iostream>
using namespace Eigen;

namespace USTC_CG::node_sph_fluid {

WCSPH::WCSPH(const MatrixXd& X, const Vector3d& box_min, const Vector3d& box_max)
    : SPHBase(X, box_min, box_max)
{
}

void WCSPH::compute_density()
{
    // Traverse all particles
    // This operation can be done in parallel using OpenMP
    for (auto& p : ps_.particles()) {
        p->density() = ps_.mass() * W_zero(ps_.h());
        if (p->is_boundary()) {
            continue;
        }
        // Then traverse all neighbors of p
        for (auto& q : p->neighbors()) {
            p->density() += ps_.mass() * W(p->x() - q->x(), ps_.h());
        }

        if (enable_debug_output) {
            //if (p->density() > ps_.density0()) {
                std::cout << "density: " << p->density() << std::endl;
            //}
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
    ps_.searchNeighbors();
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

    TOC(step)
}
}  // namespace USTC_CG::node_sph_fluid