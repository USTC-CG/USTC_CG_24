#include "particle_system.h"
#include "utils.h"
#include <iostream>

namespace USTC_CG::node_sph_fluid {

using namespace Eigen;
using namespace std;
#define M_PI 3.14159265358979323846

ParticleSystem::ParticleSystem(const MatrixXd &fluid_particle_X, 
    const MatrixXd& boundary_particle_X, 
    const bool sim_2d)
{
    init_parameters(sim_2d);

    for (int i = 0; i < fluid_particle_X.rows(); i++) {
        add_particle(fluid_particle_X.row(i).transpose(), Particle::FLUID);
    }
    for (int i = 0; i < boundary_particle_X.rows(); i++) {
        add_particle(boundary_particle_X.row(i).transpose(), Particle::BOUNDARY);
    }

    num_fluid_particles_ = fluid_particle_X.rows();
    num_boundary_particles_ = boundary_particle_X.rows();

    Vector3d scaled_box_min = boundary_particle_X.colwise().minCoeff(); 
    Vector3d scaled_box_max = boundary_particle_X.colwise().maxCoeff(); 
    
    init_neighbor_search(scaled_box_min, scaled_box_max); 
}

void ParticleSystem::init_parameters(const bool sim_2d)
{
    double diam = 2 * particle_radius_; 
    if (sim_2d)
    {
        particle_volume_ = pow(diam, 2);
    }
    else
    {
        particle_volume_ = 0.8 * pow(diam, 3);
    }
    particle_mass_ = particle_volume_ * density0_;
    std::cout << "particle_mass_ = " << particle_mass_ << std::endl;
}

void ParticleSystem::add_particle(const Vector3d X, Particle::particleType type)
{
	shared_ptr<Particle> p = make_shared<Particle>();
    p->X_ = X; 
	p->vel_ = Vector3d::Zero();
	p->density_ = density0_;
	p->pressure_ = 0.0;
    p->type_ = type; 
    p->idx_ = particles_.size(); 
	p->mass_ = particle_mass_;
	particles_.push_back(p);
}


// --------------------------------------- functions for search for neighbors ----------------------------------

void ParticleSystem::search_neighbors(Particle::particleType type)
{
    // update the neighbors for each particle
    for (auto &p : particles_) {
        p->neighbors_.clear();
        if (p->type_ != type){
            continue;
        }

        // Traverse neighbor grid cells
        auto neighbor_cell_indices = get_neighbor_cell_indices(p->X_);
        for (auto &cell_idx : neighbor_cell_indices) {
            for (auto &q : cells_[cell_idx]) {
                if ((p->X_ - q->X_).norm() < 1.001 * support_radius_
                    && p->idx() != q->idx()) {
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
    double eps = 1e-8; // To avoid problems caused by `floor(2.99999) = 2` 
    int x = static_cast<int>(floor((pos[0] - box_min_[0]) / cell_size_ + eps));
    int y = static_cast<int>(floor((pos[1] - box_min_[1]) / cell_size_ + eps));
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
            std::cout << RedHead() << "Error: "; 
            std::cout << "[assign_particles_to_cells] cell_idx out of range: cell= " << cell_idx;
            std::cout << " particle pos= " << p->X_.transpose() << std::endl;
            std::cout << "n cells per axis = " << n_cell_per_axis_.transpose() << std::endl;
            std::cout << "cell size = " << cell_size_ << std::endl;
            std::cout << ColorTail() << std::endl;
            //throw std::runtime_error("cell_idx out of range");
            //exit(1);
        }
    }
}

void ParticleSystem::init_neighbor_search(const Vector3d box_min, const Vector3d box_max)
{
    // Compute the number of cells in each axis
    box_min_ = box_min;
    box_max_ = box_max;
    n_cell_per_axis_ = ((box_max_ - box_min_) / cell_size_)
                           .array()
                           .ceil()
                           .cast<int>() + 1;  // Extend one more for safety

    cells_.resize(n_cell_per_axis_[0] * n_cell_per_axis_[1] * n_cell_per_axis_[2]);

    assign_particles_to_cells();

    search_neighbors(Particle::BOUNDARY);
    search_neighbors(Particle::FLUID);
}


// -------------------------------------------- helper functions -------------------------------------------------------
MatrixXd ParticleSystem::sample_particle_pos_in_a_box(
    const Vector3d min,
    const Vector3d max,
    const Vector3i n_per_axis,
    const bool sample_2d
)
{
    const Vector3d step = (max - min).array() / n_per_axis.array().cast<double>();
    const int n_particles = n_per_axis.prod();

    // TODO: potential access violation here: more than n_particles particles
    MatrixXd X = MatrixXd::Zero(n_particles, 3);
    for (int i = 0; i < n_per_axis[0]; i++) {
        for (int j = 0; j < n_per_axis[1]; j++) {
            for (int k = 0; k < n_per_axis[2]; k++) {
                X.row(i * n_per_axis[1] * n_per_axis[2] + j * n_per_axis[2] + k)
                    <<  min[0] + i * step[0],
                    min[1] + j * step[1], min[2] + k * step[2];
            }
        }
    }
    return X;
}

MatrixXd ParticleSystem::sample_particle_pos_around_a_box(
    const Vector3d min,
    const Vector3d max,
    const Vector3i n_per_axis,
    const bool sample_2d,
    const double scale_factor
)
{
	Vector3d scaled_min = min - scale_factor * (max - min); 
	Vector3d scaled_max = max + scale_factor * (max - min); 

    const Vector3d step = (max - min).array() / n_per_axis.array().cast<double>();
    double s = 1 + 2 * scale_factor;

    vector<Vector3d> pos;

   for (int i = 0; i <= n_per_axis[0] * s; i++) {
        for (int j = 0; j <= n_per_axis[1] * s; j++) {
            for (int k = 0; k <= n_per_axis[2] * s; k++) {

                double x = scaled_min[0] + i * step[0];
                double y = scaled_min[1] + j * step[1];
                double z = scaled_min[2] + k * step[2];

                if (x < min[0] || x > max[0] || y < min[1] || y > max[1] || z < min[2] ||
                    z > max[2]) {
                    pos.push_back(Vector3d{ x, y, z });
                }
            }
        }
    }

     MatrixXd X = MatrixXd::Zero(pos.size(), 3);
     for (auto& p : pos) {
		X.row(&p - &pos[0]) = p.transpose();
	}   

     return X; 
}
// --------------------------------------------------------------------------------------------------------


}  // namespace USTC_CG::node_sph_fluid
