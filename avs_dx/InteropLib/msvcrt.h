#pragma once

namespace msvcrt
{
	void* malloc( size_t size );
	void free( void* ptr );

	void* operatorNew( size_t size );
	void operatorDelete( void* ptr );

	template<class T>
	inline void deleteObject( T* t )
	{
		t->~T();
		operatorDelete( t );
	}
};