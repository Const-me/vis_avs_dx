#pragma once

// A statically created shader, usually from a pointer in StaticResources.
template<eStage stage>
class StaticShader
{
	const ShaderPtr<stage> m_shader;

public:
	StaticShader( IUnknown* pUnk ) : m_shader( ( IShader<stage>* )pUnk )
	{
		if( nullptr == pUnk )
			logError( "Static shader constructed without the shader" );
	}

	const bool data()
	{
		static_assert( false, "That shader is static, it has no data" );
		return false;
	};

	bool bind( bool isBeat ) const
	{
		bindShader<stage>( m_shader );
		return nullptr != m_shader;
	}
};

// A statically compiled but dynamically created shader, with DXBC usually coming from Effects\shadersCode.cpp: some effects don't need dynamic macros.
// See Effects/Trans/Blitter.cpp for usage example.
template<eStage stage>
class BinaryShader
{
	const ByteRange m_bytecode;
	ShaderPtr<stage> m_shader;

public:
	BinaryShader( ByteRange dxbc ) :
		m_bytecode( dxbc ) { }

	const bool data()
	{
		static_assert( false, "That shader is static, it has no data" );
		return false;
	};

	HRESULT compile()
	{
		if( m_shader )
			return S_FALSE;
		return createShader( m_bytecode, m_shader );
	}

	bool bind( bool isBeat ) const
	{
		bindShader<stage>( m_shader );
		return nullptr != m_shader;
	}
};

// Empty structure when no shader is defined in the effect for particular pipeline stage.
template<eStage stage>
struct NoShader
{
	NoShader( bool unused ) { }

	const bool data()
	{
		static_assert( false, "The effect has no shader for that pipeline stage" );
		return false;
	};

	bool bind( bool isBeat ) const
	{
		bindShader<stage>( nullptr );
		return true;
	}
};