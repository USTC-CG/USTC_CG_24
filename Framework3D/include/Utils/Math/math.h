#pragma once
#include "USTC_CG.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

inline int div_ceil(int dividend, int divisor)
{
    return (dividend + (divisor - 1)) / divisor;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE