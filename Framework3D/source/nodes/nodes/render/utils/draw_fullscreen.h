
#pragma once

#include "USTC_CG.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

#ifdef USTC_CG_BACKEND_OPENGL
void CreateFullScreenVAO(unsigned& VAO, unsigned& VBO);
void DestroyFullScreenVAO(unsigned& VAO, unsigned& VBO);
#endif

USTC_CG_NAMESPACE_CLOSE_SCOPE
