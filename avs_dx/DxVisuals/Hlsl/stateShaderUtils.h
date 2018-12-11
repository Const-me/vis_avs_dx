#pragma once

class MacroValues : public std::vector<std::pair<CStringA, CStringA>>
{
public:
	MacroValues( int stateBufferOffset )
	{
		reserve( 4 );
		add( "STATE_OFFSET", stateBufferOffset );
	}

	void add( const CStringA &k, const CStringA &v )
	{
		emplace_back( std::pair<CStringA, CStringA>{ k, v } );
	}

	void add( const CStringA &k, int v )
	{
		CStringA str;
		str.Format( "%i", v );
		add( k, str );
	}
};

HRESULT splitStateGs( const CStringA& src, CStringA& main, CAtlMap<CStringA, int>& globals );

CStringA replaceMacros( const CStringA& src, const MacroValues& vals );

CStringA uintConstants( const int* arr, int count );