#pragma once

// Create or update a dynamic constant buffer. The size must be the same for all updates.
HRESULT updateCBuffer( CComPtr<ID3D11Buffer> &ptr, const void* pData, size_t cb );

// Create or update a dynamic vertex buffer. The size must be the same for all updates.
HRESULT updateVertexBuffer( CComPtr<ID3D11Buffer> &ptr, const void* pData, size_t cb );