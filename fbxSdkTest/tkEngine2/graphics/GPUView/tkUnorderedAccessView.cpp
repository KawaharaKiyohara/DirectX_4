/*!
 *@brief	ShaderResourceView
 */

#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/graphics/GPUView/tkUnorderedAccessView.h"
#include "tkEngine2/graphics/GPUBuffer/tkStructuredBuffer.h"
#include "tkEngine2/tkEngine.h"


namespace tkEngine2{
	CUnorderedAccessView::CUnorderedAccessView()
	{
	}
	CUnorderedAccessView::~CUnorderedAccessView()
	{
	}
	void CUnorderedAccessView::Release()
	{
		if(m_uav != nullptr)
		{
			m_uav->Release();
			m_uav = nullptr;
		}
	}
	bool CUnorderedAccessView::Create(CStructuredBuffer& structuredBuffer)
	{
		Release();
		ID3D11Buffer* pBuf = structuredBuffer.GetBody();
		if(pBuf != nullptr){
			D3D11_BUFFER_DESC descBuf;
	    	ZeroMemory( &descBuf, sizeof(descBuf) );
	   		pBuf->GetDesc( &descBuf );
	   		
	   		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		    ZeroMemory( &desc, sizeof(desc) );
		    desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		    desc.Buffer.FirstElement = 0;
		    
		    desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
	        desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride; 
	        
			HRESULT hr = Engine().GetD3DDevice()->CreateUnorderedAccessView(pBuf, &desc, &m_uav);
			if (FAILED(hr)) {
				return false;
			}
		}
		return true;
	}
}