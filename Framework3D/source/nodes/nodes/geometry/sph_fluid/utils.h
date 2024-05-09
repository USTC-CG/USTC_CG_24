#pragma once 
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <string>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/primvarsAPI.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>


namespace USTC_CG::node_sph_fluid {

inline std::string GreenHead() { return "\x1b[1;30;92m"; }
inline std::string RedHead() { return "\x1b[1;30;91m"; };
inline std::string YellowHead() { return "\x1b[1;30;93m"; };
inline std::string CyanHead() { return "\x1b[1;30;96m"; };
inline std::string ColorTail() { return "\x1b[0m"; };

// ------------------------- helper functions -------------------------------

inline pxr::VtArray<pxr::GfVec3f> eigen_to_usd_vertices(const Eigen::MatrixXd& V)
{
    pxr::VtArray<pxr::GfVec3f> vertices;
    for (int i = 0; i < V.rows(); i++) {
        vertices.push_back(pxr::GfVec3f(V(i, 0), V(i, 1), V(i, 2)));
    }
    return vertices;
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


// --------------------------------------------------------------------------

}  // namespace USTC_CG::node_sph_fluid