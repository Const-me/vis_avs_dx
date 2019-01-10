#include "stdafx.h"
#include <EASTL/allocator.h>

namespace eastl
{
	// Implement assertion failure. This version just traps to debugger.
	void __cdecl AssertionFailure( const char *af )
	{
		OutputDebugStringA( af );
		__debugbreak();
	}

	// Implement allocators.
	// The default ones are incompatible with Windows because they use global operator delete, which is incompatible with memory allocated by _aligned_malloc API.
	// Because of this issue, you must define EASTL_USER_DEFINED_ALLOCATOR everywhere you use any EASTL stuff.
	// For example, you could add that define in project setting (for every combination of platform + configuration), or in stdafx.h precompiled header.
	allocator::allocator( const char* EASTL_NAME( pName ) )
	{
#ifndef EASTL_USER_DEFINED_ALLOCATOR
		static_assert( false, "You must define EASTL_USER_DEFINED_ALLOCATOR on Windows." );
#endif

#if EASTL_NAME_ENABLED
		mpName = pName ? pName : EASTL_ALLOCATOR_DEFAULT_NAME;
#endif
	}

	allocator::allocator( const allocator& EASTL_NAME( alloc ) )
	{
#if EASTL_NAME_ENABLED
		mpName = alloc.mpName;
#endif
	}

	allocator::allocator( const allocator&, const char* EASTL_NAME( pName ) )
	{
#if EASTL_NAME_ENABLED
		mpName = pName ? pName : EASTL_ALLOCATOR_DEFAULT_NAME;
#endif
	}

	allocator& allocator::operator=( const allocator& EASTL_NAME( alloc ) )
	{
#if EASTL_NAME_ENABLED
		mpName = alloc.mpName;
#endif
		return *this;
	}

	const char* allocator::get_name() const
	{
#if EASTL_NAME_ENABLED
		return mpName;
#else
		return EASTL_ALLOCATOR_DEFAULT_NAME;
#endif
	}

	void allocator::set_name( const char* EASTL_NAME( pName ) )
	{
#if EASTL_NAME_ENABLED
		mpName = pName;
#endif
	}

	void* allocator::allocate( size_t n, int flags )
	{
		// Using aligned version of malloc to be able to use _aligned_free everywhere.
		// Apparently, EASTL doesn't distinguish aligned versus unaligned memory, this means we must use _aligned_malloc / _aligned_free everywhere or it will prolly crash in runtime.
		constexpr size_t defaultAlignment = alignof( void* );

		return _aligned_malloc( n, defaultAlignment );
	}

	void* allocator::allocate( size_t n, size_t alignment, size_t offset, int flags )
	{
		return  _aligned_offset_malloc( n, alignment, offset );
	}

	void allocator::deallocate( void* p, size_t )
	{
		return _aligned_free( p );
	}

	bool operator==( const allocator&, const allocator& )
	{
		return true; // All allocators are considered equal, as they merely use global new/delete.
	}

	bool operator!=( const allocator&, const allocator& )
	{
		return false; // All allocators are considered equal, as they merely use global new/delete.
	}
}