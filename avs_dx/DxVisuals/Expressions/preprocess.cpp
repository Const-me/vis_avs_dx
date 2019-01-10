#include "stdafx.h"
#include "preprocess.h"

inline bool isBullshitCharacter( char c )
{
	// Stupid '¤' character, some presets use as a comment. Have no idea how that ancient lex/bison stack handled that.
	// For extra safety, skipping any abnormal characters. The HLSL compiler doesn't like them too much anyway, fails with quite understandable "invalid character" error.

	// https://en.wikipedia.org/wiki/ASCII
	if( (uint8_t)c >= 0x7F )
		return true;	// negative = Latin-1 Supplement, 0x7F = "DEL" control character

	if( c >= ' ' )
		return false;	// Good part of ASCII, space is 0x20

	// Control characters, allow only tabs and newlines.
	return c != '\r' && c != '\n' && c != '\t';
}

void Expressions::preprocess( CStringA& nseel )
{
	if( nseel.GetLength() <= 0 )
		return;

	// Not that important for this particular use case, but still, memmove is implemented in assembly with SSE, speed is limited by RAM bandwidth.
	// For larger strings, simpler char-by-char copy will be couple times slower.
	// The Context class below exposes char-by-char API to filter the string, but implements it with memmove i.e. fast.
	class Context
	{
		CStringA& str;
		char* const begin;
		char* const end;
		char *read, *write;
		char *blockRead = nullptr;

	public:
		Context( CStringA& nseel ) :
			str( nseel ),
			begin( nseel.GetBuffer() ),
			end( begin + nseel.GetLength() )
		{
			read = write = begin;
		}

		~Context()
		{
			exclude();
			str.ReleaseBufferSetLength( (int)( write - begin ) );
		}

		char peek( int i = 0 ) const
		{
			return read[ i ];
		}

		// Stop copying characters at the current offset.
		void exclude()
		{
			if( nullptr == blockRead )
				return;	// Wasn't copying, nothing to do
			const size_t cb = read - blockRead;

			MoveMemory( write, blockRead, cb );
			write += cb;
			blockRead = nullptr;
		}

		// Start or continue copying characters from the current offset.
		void include()
		{
			if( nullptr != blockRead )
				return;	// Already copy
			blockRead = read;
		}

		// Advance read pointer by specified count of characters
		bool advance( int n = 1 )
		{
			assert( n > 0 );
			read += n;
			return read >= end;
		}

		bool advanceToChar( char c )
		{
			for( ; read < end; read++ )
			{
				if( read[ 0 ] == c )
					return false;
			}
			return true;
		}
	};

	Context ctx{ nseel };
	while( true )
	{
		const char c = ctx.peek();
		if( c == '/' )
		{
			if( ctx.peek( 1 ) == '*' )	//< this won't read garbage after the end pointer because GetBuffer() returns null-terminated buffer, i.e. at the end we'll just get '\0'
			{
				// C-style comment
				ctx.exclude();
				if( ctx.advance( 2 ) )
					return;
				while( true )
				{
					if( ctx.advanceToChar( '*' ) || ctx.advance() )
						return;
					if( ctx.peek() != '/' )
						continue;
					if( ctx.advance() )	// Skip the final '/'
						return;
					break;
				}
				// Closed
				continue;
			}
			if( ctx.peek( 1 ) == '/' )
			{
				// C++ comment
				ctx.exclude();
				// https://stackoverflow.com/a/5683030/126995
				if( ctx.advance( 2 ) || ctx.advanceToChar( '\n' ) || ctx.advance() )
					return;
				// Closed
				continue;
			}
		}

		if( isBullshitCharacter( c ) )
		{
			ctx.exclude();
			if( ctx.advance() )
				return;
			while( true )
			{
				const char cn = ctx.peek();
				if( cn == ';' || cn == '\r' || cn == '\n' )
					break;
				if( ctx.advance() )
					return;
			}
			if( ctx.advance() )
				return;
			continue;
		}

		if( c == ' ' || c == '\t' || c == '\r' || c == '\n' )
			ctx.exclude();
		else
			ctx.include();

		if( ctx.advance() )
			return;
	}
}