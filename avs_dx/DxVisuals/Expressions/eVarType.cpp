#include "stdafx.h"
#include "eVarType.h"

namespace Expressions
{
	const char* hlslName( eVarType vt )
	{
		switch( vt )
		{
		case eVarType::u32: return "uint";
		case eVarType::f32: return "float";
		case eVarType::i32: return "int";
		}
		__debugbreak();
		return nullptr;
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
		}
		__debugbreak();
		return "";
	}
}