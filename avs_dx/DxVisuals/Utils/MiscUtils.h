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

void iaClearBuffer();

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

enum struct eBlend : uint8_t
{
	// Disable blending, just write whatever was produced by pixel shader
	None,
	// Set premultiplied alpha blending mode
	Premultiplied,
	// Additive + premultiplied alpha
	Add
};

void omBlend( eBlend mode );

// Draw the full-screen triangle; you must bind the pixel shader before this call. Pass bindShaders=true to bind vertex & geometry shaders, false if already bound.
void drawFullscreenTriangle( bool bindShaders = true );

inline const char* cstr( const CStringA &str ) { return str; }