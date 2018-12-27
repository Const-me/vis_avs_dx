#pragma once
#include <memory>

class RenderTargets;

// Minimally sufficient interface for the AVS code to render these effects with DX.
// AVS contains quite a bit of C++ code and doesn't use precompiled headers, having this minimal interface helps a lot with rebuild times.
class iRootEffect
{
public:
	virtual ~iRootEffect() { }

	// These methods are only called on the root C_RenderListClass object. Can't make them abstract because non-root effects also inherit from this interface, it's pointless to copy-paste the empty implementation to every one of them.
	virtual HRESULT renderRoot( bool isBeat, RenderTargets& rt ) { return CO_E_NOT_SUPPORTED; }

	virtual HRESULT clearRenders() { return CO_E_NOT_SUPPORTED; }
};

void registerEffect( std::unique_ptr<iRootEffect>* p );
void unregisterEffect( std::unique_ptr<iRootEffect>* p );

// Many effects own D3D resources. We want to reliably clean them up before the rendering thread terminates.
// The GUI thread that creates them is not even owned by vis_avs.dll, it's created from the outside, by winamp.exe.
// That's why this custom smart pointer class that maintains registrations of live effect instances.
class DxEffectPtr
{
	std::unique_ptr<iRootEffect> m_pointer;

public:
	DxEffectPtr() = default;

	DxEffectPtr( const DxEffectPtr& ) = delete;
	DxEffectPtr( DxEffectPtr&& ) = delete;
	void operator=( const DxEffectPtr& ) = delete;

	~DxEffectPtr()
	{
		unregisterEffect( &m_pointer );
	}

	template<class TImpl>
	void create( void* pState )
	{
		assert( empty() );
		m_pointer = std::make_unique<TImpl>( pState );
		registerEffect( &m_pointer );
	}

	operator bool() const
	{
		return m_pointer.operator bool();
	}
	bool empty() const
	{
		return !m_pointer.operator bool();
	}

	iRootEffect* get() const
	{
		return m_pointer.get();
	}
	iRootEffect* operator->() const
	{
		return m_pointer.operator ->();
	}
	iRootEffect& operator*()const
	{
		return *m_pointer;
	}
};