#pragma once
#include "USTC_CG.h"
#include "GCore/GOP.h"
#include "GCore/GOperators.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
DeclareGOperator(SpringSystemOperator);


class USTC_CG_API SpringSystemOperator : public GOperator
{
protected:
    void process(GeometryHandle in, GeometryHandle out) override;
    void preprocess(GeometryHandle in, GeometryHandle& out) override;
};


USTC_CG_NAMESPACE_CLOSE_SCOPE
