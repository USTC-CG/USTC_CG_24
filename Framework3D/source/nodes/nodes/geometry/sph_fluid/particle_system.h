#pragma once 
#include <Eigen/Dense>
#include <vector>
#include <memory>

using namespace Eigen;

namespace USTC_CG::node_sph_fluid {

class Particle {
   public:
    enum particleType { FLUID, BOUNDARY };

    unsigned idx() const
    {
        return idx_;
    };
    Eigen::Vector3d& x()
    {
        return X_;
    };
    Eigen::Vector3d& vel()
    {
        return vel_;
    };
    Eigen::Vector3d& acceleration()
    {
        return acceleration_;
    };
    double& density()
    {
        return density_;
    };
    double& pressure()
    {
        return pressure_;
    };

    particleType type() const
    {
        return type_;
    };

    bool is_boundary() const
    {
        return type_ == BOUNDARY;
    }

    const std::vector<std::shared_ptr<Particle>>& const neighbors()
    {
        return neighbors_;
    };

    // protected:
    double density_;
    double pressure_;
    particleType type_;
    unsigned idx_;

    Eigen::Vector3d X_;
    Eigen::Vector3d vel_;
    Eigen::Vector3d acceleration_;

    // should be a vector of pointers?.
    std::vector<std::shared_ptr<Particle>> neighbors_;
};

// This class is for particle neighbor search
class ParticleSystem {
   public:
    ParticleSystem(const MatrixXd& X, const Vector3d& box_min, const Vector3d& box_max);

    const std::vector<std::shared_ptr<Particle>>& particles() const
    {
        return particles_;
    }
    const double h() const
    {
        return support_radius_;
    }

    const double volume() const
    {
        return particle_volume_;
    }
    const double mass() const
    {
        return particle_mass_;
    }
    const double density0() const
    {
        return density0_;
    }
    std::vector<std::vector<std::shared_ptr<Particle>>> cells() const
    {
        return cells_;
    }

    void search_neighbors();
    void add_particle();
    static MatrixXd sample_particle_pos_in_a_box(
        const Vector3d min,
        const Vector3d max,
        const Vector3i n_particle_per_axis);
    unsigned pos_to_cell_index(const Vector3d& x) const;
    unsigned cell_xyz_to_cell_index(const unsigned x, const unsigned y, const unsigned z) const;
    Vector3i pos_to_cell_xyz(const Vector3d& x) const;

    void assign_particles_to_cells();
    std::vector<unsigned> get_neighbor_cell_indices(const Vector3d& x) const;

   protected:
    std::vector<std::shared_ptr<Particle>> particles_;
    double particle_radius_ = 0.025;
    double support_radius_;

    double density0_ = 1000.0;
    // volume and mass are shared by all particles
    double particle_volume_;
    double particle_mass_;

    unsigned num_particles_;

    // ------------------- Particle properties --------------------------------------
    // If store as a big matrix, cache hit may be a problem.
    // MatrixXd init_X_;
    // MatrixXd X_;
    // MatrixXd vel_;
    // MatrixXd acceleration_;
    // MatrixXd density_;
    // MatrixXd pressure_;

    //-------------- Spatial acceleration structure for neighbor search -------------
    // store pointer to particles: TODO: cache friendly?
    std::vector<std::vector<std::shared_ptr<Particle>>> cells_;
    double cell_size_;
    Vector3i n_cell_per_axis_;  // number of cells per axis
    Vector3d box_min_, box_max_;
};
}  // namespace USTC_CG::node_sph_fluid