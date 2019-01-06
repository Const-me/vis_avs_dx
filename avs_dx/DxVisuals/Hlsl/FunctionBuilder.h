#pragma once

namespace Hlsl
{
	class FunctionBuilder
	{
		CStringA hlsl;

	public:
		void print( const char* pszFormat, ... );

		void print( const array<uint32_t, 4>& uint4 );

		HRESULT uintConstants( const char* constantName, const int* arr, int count );

		HRESULT uint4Constants( const char* constantName, const int* arr, int count );

		void operator+=( const CStringA& that ) { hlsl += that; }
	};
}