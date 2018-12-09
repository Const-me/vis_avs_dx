#include "stdafx.h"
#include "DynamicMovement.h"
#include <Hlsl/Variable.h>

using namespace Hlsl;
static const Variable s_cbufferVars[] =
{
	Variable::floatScalar( "channel" ),
	Variable::floatScalar( "yMin" ),
	Variable::floatScalar( "yMax" ),
};

DynamicMovement::DynamicMovement()
{
}

DynamicMovement::~DynamicMovement()
{
}