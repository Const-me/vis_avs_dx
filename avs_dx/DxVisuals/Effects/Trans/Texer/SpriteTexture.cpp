#include "stdafx.h"
#include "SpriteTexture.h"
#include <Utils/DXTK/WICTextureLoader.h>
#include <Utils/findAssets.h>

bool isPictureExt( const CString& ext );

HRESULT SpriteTexture::update( const char* source )
{
	if( source == m_name )
		return S_FALSE;

	m_srv = nullptr;
	m_size = Vector2::Zero;
	m_name = source;
	if( m_name.GetLength() <= 0 )
		return S_OK;

	CString path;
	CHECK( getAssetFilePath( source, &isPictureExt, path ) );

	CHECK( coInit() );

	CComPtr<ID3D11Resource> res;
	CHECK( DirectX::CreateWICTextureFromFile( device, context, path, &res, &m_srv ) );

	CComQIPtr<ID3D11Texture2D> tex = res.operator ->();
	if( !tex )
		return E_NOINTERFACE;

	D3D11_TEXTURE2D_DESC desc;
	tex->GetDesc( &desc );

	m_size = Vector2{ (float)desc.Width, (float)desc.Height };
	return S_OK;
}