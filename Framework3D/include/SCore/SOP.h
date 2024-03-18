#pragma once
#include"USTC_CG.h"
#include "GCore/GOP.h"



USTC_CG_NAMESPACE_OPEN_SCOPE
class USTC_CG_API SOperandBase : public GOperandBase
{
public:
    virtual void Run() = 0;
    SOperandBase();
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
