#include "particle_system.h"
#include <iostream>

namespace USTC_CG::node_sph_fluid {

using namespace Eigen;
using namespace std;
#define M_PI 3.14159265358979323846

ParticleSystem::ParticleSystem(const MatrixXd &X, const Vector3d &box_min, const Vector3d &box_max)
    : num_particles_(X.rows())
{
    support_radius_ = 4 * particle_radius_;
    cell_size_ = support_radius_;

    const double diam = 2 * particle_radius_;
    particle_volume_ = 0.8 * pow(diam, 3);
    particle_mass_ = particle_volume_ * density0_;

    // Initialize the particles
    for (int i = 0; i < X.rows(); i++) {
        shared_ptr<Particle> p = make_shared<Particle>();
        p->X_ = X.row(i).transpose();
        p->vel_ = Vector3d::Zero();
        p->density_ = 0.0;
        p->pressure_ = 0.0;
        p->type_ = Particle::FLUID;
        p->idx_ = i;
        particles_.push_back(p);
    }

    // Initialize the spatial grid
    // Compute the bounding box of the particles
    // Vector3d min = X.colwise().minCoeff();
    // Vector3d max = X.colwise().maxCoeff();

    // Compute the number of cells in each axis
    box_max_ = box_max;
    box_min_ = box_min;
    n_cell_per_axis_ = ((box_max - box_min) / cell_size_)
                           .array()
                           .ceil()
                           .cast<int>();  // Extend one more for safety
    // TODO: need to check here box_max is bigger then box_min

    cells_.resize(n_cell_per_axis_[0] * n_cell_per_axis_[1] * n_cell_per_axis_[2]);

    assign_particles_to_cells();
    searchNeighbors();
}

void ParticleSystem::searchNeighbors()
{
    // update the neighbors for each particle
    for (auto &p : particles_) {
        p->neighbors_.clear();
        if (p->type_ == Particle::BOUNDARY) {
            // TODO: do we need to search neighbors for boundary particles?
            continue;
        }

        // Traverse neighbor grid cells
        auto neighbor_cell_indices = get_neighbor_cell_indices(p->X_);
        for (auto &cell_idx : neighbor_cell_indices) {
            for (auto &q : cells_[cell_idx]) {
                if ((p->X_ - q->X_).norm() < 1.001 * support_radius_) {
                    p->neighbors_.push_back(q);
                }
            }
        }
    }
}

// A hash function mapping spatial position to cell index
unsigned ParticleSystem::pos_to_cell_index(const Vector3d &pos) const
{
    auto xyz = pos_to_cell_xyz(pos);
    return cell_xyz_to_cell_index(xyz[0], xyz[1], xyz[2]);
}

unsigned
ParticleSystem::cell_xyz_to_cell_index(const unsigned x, const unsigned y, const unsigned z) const
{
    return x * n_cell_per_axis_[1] * n_cell_per_axis_[2] + y * n_cell_per_axis_[2] + z;
}

Vector3i ParticleSystem::pos_to_cell_xyz(const Vector3d &pos) const
{
    double eps = 1e-8;
    int x = static_cast<int>(floor((pos[0] - box_min_[0]) / cell_size_ + eps));
    int y = static_cast<int>(floor((pos[1] - box_min_[1]) / cell_size_ + eps));

    // double zz = (pos[2] - box_min_[2]);
    // double tmp_zz = (pos[2] - box_min_[2]) / cell_size_;
    // double floor_tmp_zz = floor(tmp_zz);

    int z = static_cast<int>(floor((pos[2] - box_min_[2]) / cell_size_ + eps));

    return Vector3i(x, y, z);
}

// return center_cell index and its neighbors
std::vector<unsigned> ParticleSystem::get_neighbor_cell_indices(const Vector3d &pos) const
{
    std::vector<unsigned> neighbor_cell_indices;

    auto xyz = pos_to_cell_xyz(pos);
    int x = xyz[0];
    int y = xyz[1];
    int z = xyz[2];

    // compute indices
    const unsigned n_cell = n_cell_per_axis_.prod();
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            for (int k = -1; k <= 1; k++) {
                if (x + i < 0 || x + i >= n_cell_per_axis_[0] || y + j < 0 ||
                    y + j >= n_cell_per_axis_[1] || z + k < 0 || z + k >= n_cell_per_axis_[2]) {
                    continue;
                }
                unsigned idx = cell_xyz_to_cell_index(x + i, y + j, z + k);
                // Check if valid
                if (idx >= 0 && idx < n_cell) {
                    neighbor_cell_indices.push_back(idx);
                }
            }
        }
    }
    return neighbor_cell_indices;
}

void ParticleSystem::assign_particles_to_cells()
{
    // Clear all cells
    for (auto &cell : cells_) {
        cell.clear();
    }

    // Assign particles to cells
    for (auto &p : particles_) {
        unsigned cell_idx = pos_to_cell_index(p->X_);
        if (cell_idx >= 0 && cell_idx < cells_.size())
            cells_[cell_idx].push_back(p);
        else {
            std::cout << "[assign_particles_to_cells] cell_idx out of range: cell= " << cell_idx;
            std::cout << " particle pos= " << p->X_.transpose() << std::endl;
            std::cout << "n cells per axis = " << n_cell_per_axis_.transpose() << std::endl;
            std::cout << "cell size = " << cell_size_ << std::endl;
            exit(1);
        }
    }
}

// First, add a particle sample function from a box area, which is needed in node system
MatrixXd ParticleSystem::sample_particle_pos_in_a_box(
    const Vector3d min,
    const Vector3d max,
    const Vector3i n_per_axis)
{
    const Vector3d step = (max - min).array() / n_per_axis.array().cast<double>();
    const int n_particles = n_per_axis.prod();

    // TODO: potential access violation here: more than n_particles particles
    MatrixXd X = MatrixXd::Zero(n_particles, 3);
    for (int i = 0; i < n_per_axis[0]; i++) {
        for (int j = 0; j < n_per_axis[1]; j++) {
            for (int k = 0; k < n_per_axis[2]; k++) {
                X.row(i * n_per_axis[1] * n_per_axis[2] + j * n_per_axis[2] + k)
                    << min[0] + i * step[0],
                    min[1] + j * step[1], min[2] + k * step[2];
            }
        }
    }
    return X;
}
}  // namespace USTC_CG::node_sph_fluid
