#pragma once

enum struct eInput : uint8_t
{
	Ignore, Replace
};

enum struct eOutput : uint8_t
{
	Texture, Maskedtexture
};

struct sTexerData
{
	const char* texture;
	eInput input;
	eOutput output;
	uint32_t particles;
};

class C_RBASE;
sTexerData getTexerData( const C_RBASE* pTexerFx );