#include "stdafx.h"
#include "texerHacks.h"

sTexerData getTexerData( const C_RBASE* pTexerFx )
{
	sTexerData res = {};
	const uint8_t* const ptr = (const uint8_t*)pTexerFx;

	res.texture = (const char*)( ptr + 24 );
	res.particles = *(const uint32_t*)( ptr + 0x120 );

	const uint8_t mode = ptr[ 0x11c ];

	switch( mode )
	{
	case 0x5:
		res.input = eInput::Ignore;
		res.output = eOutput::Texture;
		break;
	case 0x6:
		res.input = eInput::Replace;
		res.output = eOutput::Texture;
		break;
	case 0x9:
		res.input = eInput::Ignore;
		res.output = eOutput::Maskedtexture;
		break;
	case 0x0A:
		res.input = eInput::Replace;
		res.output = eOutput::Maskedtexture;
		break;
	}

	return res;
}