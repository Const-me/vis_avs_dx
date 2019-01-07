#pragma once

namespace msvcrt
{
	void* operatorNew( size_t size );
	void operatorDelete( void* ptr );

	template<class T>
	inline void deleteObject( T* t )
	{
		t->~T();
		operatorDelete( t );
	}
};