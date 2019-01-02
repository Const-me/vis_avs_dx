#pragma once

// Create or update a dynamic constant buffer.
HRESULT updateCBuffer( CComPtr<ID3D11Buffer> &ptr, const void* pData, size_t cb );