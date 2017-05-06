/*!
 * @brief	StructuredBuffer
 */

#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/graphics/GPUBuffer/tkStructuredBuffer.h"
#include "tkEngine2/tkEngine.h"

namespace tkEngine2{
	CStructuredBuffer::CStructuredBuffer()
	{
	}
	CStructuredBuffer::~CStructuredBuffer()
	{
	}
	void CStructuredBuffer::Release()
	{
		if (m_structuredBuffer != nullptr) {
			m_structuredBuffer->Release();
			m_structuredBuffer = nullptr;
		}
	}
	bool CStructuredBuffer::Create(const void* pInitData, D3D11_BUFFER_DESC& bufferDesc)
	{
		Release();		
		HRESULT hr;
		if (pInitData) {
			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = pInitData;
			hr = Engine().GetD3DDevice()->CreateBuffer(&bufferDesc, &InitData, &m_structuredBuffer);
		}
		else {
			hr = Engine().GetD3DDevice()->CreateBuffer(&bufferDesc, NULL, &m_structuredBuffer);
		}
		if (FAILED(hr)) {
			return false;
		}
		return true;
	}
}