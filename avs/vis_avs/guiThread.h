#pragma once

struct winampVisModule;

namespace guiThread
{
	HRESULT start( winampVisModule *this_mod );
	HRESULT stop();

	HRESULT openConfig();
	HRESULT closeConfig();
}