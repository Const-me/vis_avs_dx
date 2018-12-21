#include "stdafx.h"
#include "varEnums.h"

namespace Expressions
{
	const char* hlslName( eVarType vt )
	{
		switch( vt )
		{
		case eVarType::u32: return "uint";
		case eVarType::f32: return "float";
		case eVarType::i32: return "int";
		case eVarType::f64: return "double";
		}
		__debugbreak();
		return nullptr;
	}

	int variableSize( eVarType vt )
	{
		switch( vt )
		{
		case eVarType::u32:
		case eVarType::f32:
		case eVarType::i32:
			return 1;
		case eVarType::f64:
			return 2;
		}
		__debugbreak();
		return 0;
	}

	CStringA stateLoad( eVarType vt, int offset )
	{
		offset *= 4;
		CStringA res;
		switch( vt )
		{
		case eVarType::u32:
			res.Format( "effectStates.Load( STATE_OFFSET + %i )", offset );
			return res;
		case eVarType::f32:
			res.Format( "asfloat( effectStates.Load( STATE_OFFSET + %i ) )", offset );
			return res;
		case eVarType::i32:
			res.Format( "asint( effectStates.Load( STATE_OFFSET + %i ) )", offset );
			return res;
		case eVarType::f64:
			res.Format( "asdouble( effectStates.Load( STATE_OFFSET + %i ), effectStates.Load( STATE_OFFSET + %i ) )", offset, offset + 4 );
			return res;
		}
		__debugbreak();
		return "";
	}

	CStringA stateStore( eVarType vt, int offset, const char* varName )
	{
		offset *= 4;
		CStringA res;
		switch( vt )
		{
		case eVarType::u32:
			res.Format( "effectStates.Store( STATE_OFFSET + %i, %s )", offset, varName );
			return res;
		case eVarType::f32:
		case eVarType::i32:
			res.Format( "effectStates.Store( STATE_OFFSET + %i, asuint( %s ) )", offset, varName );
			return res;
		case eVarType::f64:
			res.Format( "{ uint lo, hi; asuint( %s, lo, hi ); effectStates.Store2( STATE_OFFSET + %i, uint2( lo, hi ) ); }", varName, offset );
			return res;
		}
		__debugbreak();
		return "";
	}
}