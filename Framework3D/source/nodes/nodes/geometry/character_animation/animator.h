#pragma once

#include <memory>
#include <vector>
#include <iostream>

#include <pxr/usd/usdSkel/cache.h>
#include <pxr/usd/usdSkel/skeleton.h>

#include "GCore/Components/MeshOperand.h"
#include "GCore/Components/SkelComponent.h"

#include <Eigen/Dense>

namespace USTC_CG::node_character_animation {

using namespace std;
using namespace Eigen;
using namespace pxr; 

class Joint
{

public:
    Joint(int idx, string name, int parent_idx, const GfMatrix4f& bind_transform); 

	void compute_world_transform();

	GfMatrix4f get_local_transform() { return local_transform_; }
	GfMatrix4f get_world_transform() { return world_transform_; }
	GfMatrix4f get_bind_transform() { return bind_transform_; }

    shared_ptr<Joint> parent_;
    vector<shared_ptr<Joint>> children_;

	friend class JointTree;

private: 
	int idx_;
    std::string name_;
    int parent_idx_; 
	
	GfMatrix4f local_transform_;
    GfMatrix4f world_transform_;
    GfMatrix4f bind_transform_;
};

class JointTree
{
 public:
    JointTree() = default;

	shared_ptr<Joint> get_root() { return root_; }
	shared_ptr<Joint> get_joint(int idx) { return joints_[idx]; }

	void compute_world_transforms_for_each_joint(); 

	void add_joint(int idx, std::string name, int parent_idx, const GfMatrix4f& bind_transform); 

	void update_joint_local_transform(const VtArray<GfMatrix4f>& new_local_transforms); 

	void print(); 

protected: 
    shared_ptr<Joint> root_;

    // All joints
    vector<shared_ptr<Joint>> joints_;
};

class Animator{

public: 
	Animator() = default;
	~Animator() = default;

	// Load skeleton and mesh from a usd file 
	Animator(const shared_ptr<MeshComponent> mesh, 
		const shared_ptr<SkelComponent> skel); 

	// Each timestep, update the world-space transforms of all joints,
	// then apply transform to each vertices of mesh 
	void step(const shared_ptr<SkelComponent> skel); 

	void update_mesh_vertices(); 

protected:
	JointTree joint_tree_;
	shared_ptr<MeshComponent> mesh_;
	shared_ptr<SkelComponent> skel_;

};

}