#pragma once

class SavedBlendMode
{
	const int savedBlendMode;

public:
	SavedBlendMode() :
		savedBlendMode( g_line_blend_mode ) { }
	~SavedBlendMode()
	{
		g_line_blend_mode = savedBlendMode;
	}
};

class SavedContext: public SavedBlendMode
{
	GlobalBuffers& savedBuffers;

	static void swapBuffers( GlobalBuffers& a, GlobalBuffers& b )
	{
		constexpr size_t cb = sizeof( GlobalBuffers );
		array<uint8_t, cb> tmp;
		CopyMemory( tmp.data(), &a, cb );	// tmp = a
		CopyMemory( &a, &b, cb );			// a = b
		CopyMemory( &b, tmp.data(), cb );	// b = tmp
	}

public:

	SavedContext( GlobalBuffers& buffers ) :
		savedBuffers( buffers )
	{
		swapBuffers( savedBuffers, StaticResources::globalBuffers );
	}

	~SavedContext()
	{
		swapBuffers( savedBuffers, StaticResources::globalBuffers );
	}
};