#pragma once 
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <string>

namespace USTC_CG::node_sph_fluid {

inline std::string GreenHead() { return "\x1b[1;30;92m"; }
inline std::string RedHead() { return "\x1b[1;30;91m"; };
inline std::string YellowHead() { return "\x1b[1;30;93m"; };
inline std::string CyanHead() { return "\x1b[1;30;96m"; };
inline std::string ColorTail() { return "\x1b[0m"; };
}  // namespace USTC_CG::node_sph_fluid