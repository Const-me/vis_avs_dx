#pragma once

// Implements updateAvs() method by constructong another copy of TThis, calling `TBase::operator ==` to compare them, then if updated `operator =` to replace the current one.
template<class TThis, class TBase>
struct ShaderUpdatesSimple : public TBase
{
	template<class S>
	HRESULT updateAvs( const S& s )
	{
		const TThis newState{ s };
		if( (const TBase&)newState == *static_cast<const TBase*>( this ) )
			return S_FALSE;	// Same values, no need to do anything

		// Detected a change, returning S_OK will cause EffectBase1::updateParameters to re-compile the shader with a new macro values.
		*this = newState;
		return S_OK;
	}
};