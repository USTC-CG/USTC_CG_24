#pragma once

#include "USTC_CG.h"

#define USTC_CG_BACKEND_NVRHI
//#define USTC_CG_BACKEND_OPENGL 

#ifdef USTC_CG_BACKEND_OPENGL
#include "internal/gl/GLResources.hpp"
#elif defined(USTC_CG_BACKEND_NVRHI)
#include "internal/nvrhi/Resources.hpp"
#endif
