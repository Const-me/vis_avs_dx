#pragma once

inline HRESULT getLastHr() { return HRESULT_FROM_WIN32( GetLastError() ); }

constexpr bool dbgBreakOnErrors = false;

#ifdef NDEBUG
#define CHECK( hr ) { const HRESULT __hr = ( hr ); if( FAILED( __hr ) ) { logError( __hr, #hr ); return __hr; } }
#else
#define CHECK( hr ) { const HRESULT __hr = ( hr ); if( FAILED( __hr ) ) { if( dbgBreakOnErrors )__debugbreak(); logError( __hr, #hr ); return __hr; } }
#endif

inline void omSetTarget( ID3D11RenderTargetView* rtv )
{
	context->OMSetRenderTargets( 1, &rtv, nullptr );
}

inline void omClearTargets()
{
	context->OMSetRenderTargets( 0, nullptr, nullptr );
}

inline void setShaders( ID3D11VertexShader* vs, ID3D11GeometryShader* gs, ID3D11PixelShader* ps )
{
	context->VSSetShader( vs, nullptr, 0 );
	context->GSSetShader( gs, nullptr, 0 );
	context->PSSetShader( ps, nullptr, 0 );
}

void iaSetBuffer( ID3D11Buffer* vb, UINT vbStride, ID3D11Buffer* ib, DXGI_FORMAT ibFormat = DXGI_FORMAT_R16_UINT );

void iaClearBuffers();

class LockExternCs
{
	CRITICAL_SECTION& m_cs;

public:
	LockExternCs() = delete;
	LockExternCs( const LockExternCs& that ) = delete;

	LockExternCs( CRITICAL_SECTION& cs );
	~LockExternCs();
};

class UnlockExternCs
{
	CRITICAL_SECTION& m_cs;

public:
	UnlockExternCs() = delete;
	UnlockExternCs( const LockExternCs& that ) = delete;

	UnlockExternCs( CRITICAL_SECTION& cs );
	~UnlockExternCs();
};

void setMacro( std::vector<std::pair<CStringA, CStringA>> &macros, const CStringA& key, const CStringA& value );