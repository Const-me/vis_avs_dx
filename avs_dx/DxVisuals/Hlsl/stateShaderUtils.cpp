#include "stdafx.h"
#include "stateShaderUtils.h"

CStringA replaceMacros( const CStringA& src, const MacroValues& vals )
{
	CStringA res = src;
	for( const auto &m : vals )
		res.Replace( m.first, m.second );
	return res;
}

CStringA uintConstants( const int* arr, int count )
{
	CStringA hlsl;
	for( int i = 0; i < count; i++ )
	{
		hlsl.AppendFormat( "0x%08X", arr[ i ] );
		if( i + 1 < count )
			hlsl += ", ";
	}
	return hlsl;
}

HRESULT splitStateGs( const CStringA& src, CStringA& main, CAtlMap<CStringA, int>& globals )
{
	// Global sections
	const CStringA markerStart = "<global>";
	const CStringA markerEnd = "</global>";

	int iStart = 0;
	while( true )
	{
		iStart = src.Find( markerStart, iStart );
		if( iStart < 0 )
			break;
		iStart += markerStart.GetLength();

		int iEnd = src.Find( markerEnd, iStart );
		if( iEnd < 0 )
			return E_INVALIDARG;
		const int iNextStart = iEnd + markerEnd.GetLength();

		iEnd--;
		while( true )
		{
			if( iEnd <= iStart )
				return E_INVALIDARG;
			const char c = src[ iEnd ];
			if( c == '\r' || c == '\n' )
				break;
			iEnd--;
		}
		CStringA section = src.Mid( iStart, iEnd - iStart );
		section.Trim();
		auto p = globals.Lookup( section );
		if( nullptr == p )
			globals.SetAt( section, 1 );
		else
			p->m_value++;

		iStart = iNextStart;
	}

	// Main
	const CStringA theMain = "void main()";
	int iMain = src.Find( theMain );
	if( iMain < 0 )
		return E_INVALIDARG;
	iMain += theMain.GetLength();
	main = src.Mid( iMain );
	main.Trim();
	return S_OK;
}