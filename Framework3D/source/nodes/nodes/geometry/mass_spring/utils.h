#pragma once 
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <set>
#include "pxr/usd/usdGeom/xform.h"
#include <vector>

namespace USTC_CG::node_mass_spring {

inline auto flatten = [&](const Eigen::MatrixXd& A) {
    Eigen::MatrixXd A_flatten = A.transpose();
    A_flatten.resize(A.rows() * A.cols(), 1);
    return A_flatten;
};
inline auto unflatten = [&](const Eigen::MatrixXd& A_flatten) {
    Eigen::MatrixXd A = A_flatten;
    A.resize(3, A_flatten.rows() / 3);
    A.transposeInPlace();
    return A;
};


inline Eigen::MatrixXi usd_faces_to_eigen(
    const pxr::VtArray<int>& faceVertexCount,
    const pxr::VtArray<int>& faceVertexIndices)
{
    unsigned nFaces = faceVertexCount.size();
    Eigen::MatrixXi F(nFaces, 3);
    unsigned count = 0;
    for (int i = 0; i < nFaces; i++) {
        for (int j = 0; j < 3; j++) {
            F(i, j) = faceVertexIndices[count];
            count += 1;
        }
    }
    return F;
}

inline Eigen::MatrixXd usd_vertices_to_eigen(const pxr::VtArray<pxr::GfVec3f>& v)
{
    unsigned nVertices = v.size();
    Eigen::MatrixXd V(nVertices, 3);
    for (int i = 0; i < nVertices; i++) {
        for (int j = 0; j < 3; j++) {
            V(i, j) = v[i][j];
        }
    }
    return V;
}

inline pxr::VtArray<pxr::GfVec3f> eigen_to_usd_vertices(const Eigen::MatrixXd& V)
{
    pxr::VtArray<pxr::GfVec3f> vertices;
    for (int i = 0; i < V.rows(); i++) {
        vertices.push_back(pxr::GfVec3f(V(i, 0), V(i, 1), V(i, 2)));
    }
    return vertices;
}

using Edge = std::pair<int, int>;
using EdgeSet = std::set<Edge>;
// Here F is of shape [nFaces, 3] for triangular mesh
inline EdgeSet get_edges(const Eigen::MatrixXi& F)
{
    EdgeSet edges;
    for (int i = 0; i < F.rows(); i++) {
        for (int j = 0; j < F.cols(); j++) {
            int v0 = F(i, j);
            int v1 = F(i, (j + 1) % F.cols());
            if (v0 > v1) {
                std::swap(v0, v1);
            }
            edges.insert(std::make_pair(v0, v1));
        }
    }
    return edges;
}

inline std::vector<bool> VtIntArray_to_vector_bool(const pxr::VtArray<float>& v)
{
	std::vector<bool> mask;
    for (int i = 0; i < v.size(); i++) {
		mask.push_back(v[i] > 0);
	}
	return mask;
}

}  // namespace USTC_CG::node_mass_spring