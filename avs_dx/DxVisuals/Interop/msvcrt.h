#pragma once

// AVS loads some DLLs (with APE extension) from Winamp\Plugins\avs directory. These APEs are compiled with VC6 and link C runtime (without the ++) in msvcrt.dll from SysWOW64, and they create their renderers in dynamic RAM.
// To correctly free them, we have to use the same memory management.
// To simplify things, we use the same allocator for the rest of the renderers.
namespace msvcrt
{
	bool startup();
	void* operatorNew( size_t size );
	void operatorDelete( void* ptr );
};