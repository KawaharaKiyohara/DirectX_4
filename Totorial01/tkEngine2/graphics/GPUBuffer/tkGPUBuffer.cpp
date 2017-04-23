/*!
 * @brief	GPUBuffer
 */

#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/graphics/GPUBuffer/tkGPUBuffer.h"
#include "tkEngine2/tkEngine.h"

namespace tkEngine2{
	CGPUBuffer::CGPUBuffer()
	{
	}
	CGPUBuffer::~CGPUBuffer()
	{
	}
	void CGPUBuffer::Release()
	{
		if (m_gpuBuffer != nullptr) {
			m_gpuBuffer->Release();
			m_gpuBuffer = nullptr;
		}
	}
	bool CGPUBuffer::Create(const void* pInitData, D3D11_BUFFER_DESC& bufferDesc)
	{
		Release();		
		HRESULT hr;
		if (pInitData) {
			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = pInitData;
			hr = Engine().GetD3DDevice()->CreateBuffer(&bufferDesc, &InitData, &m_gpuBuffer);
		}
		else {
			hr = Engine().GetD3DDevice()->CreateBuffer(&bufferDesc, NULL, &m_gpuBuffer);
		}
		if (FAILED(hr)) {
			return false;
		}
		return true;
	}
}