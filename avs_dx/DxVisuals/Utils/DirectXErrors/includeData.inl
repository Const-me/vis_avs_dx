#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)

// Desktop-only APIs
#ifdef USE_DIRECTDRAW
#include "data/ddraw.inl"
#endif

#ifdef USE_INPUT
#include "data/dinput.inl"
#endif

#ifdef USE_DIRECT3D_9
#include "data/d3d9.inl"
#endif

#ifdef USE_AUDIO
#include "data/dsound.inl"
#endif

#endif	// WINAPI_FAMILY_DESKTOP_APP

#include "data/d3d10-11.inl"

#ifdef USE_DIRECT2D
#include "data/d2d.inl"
#endif

#ifdef USE_WIC
#include "data/wic.inl"
#endif

#ifdef USE_DXUT
#include "data/dxut.inl"
#endif

#ifdef USE_AUDIO
#include "data/xaudio.inl"
#endif

#ifdef  USE_MF
#include "data/mf.inl"
#endif