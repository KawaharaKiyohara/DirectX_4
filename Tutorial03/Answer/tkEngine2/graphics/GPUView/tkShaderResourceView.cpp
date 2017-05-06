/*!
 *@brief	ShaderResourceView
 */

#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/graphics/GPUView/tkShaderResourceView.h"
#include "tkEngine2/graphics/GPUBuffer/tkStructuredBuffer.h"
#include "tkEngine2/tkEngine.h"

namespace tkEngine2{
	CShaderResourceView::CShaderResourceView()
	{
	}
	CShaderResourceView::~CShaderResourceView()
	{
		Release();
	}
	void CShaderResourceView::Release()
	{
		if(m_srv != nullptr){
			m_srv->Release();
			m_srv = nullptr;
		}
	}
	bool CShaderResourceView::Create(CStructuredBuffer& structuredBuffer)
	{
		Release();
		ID3D11Buffer* pBuf = structuredBuffer.GetBody();
		if(pBuf != nullptr){
			D3D11_BUFFER_DESC descBuf;
	    	ZeroMemory( &descBuf, sizeof(descBuf) );
	   		pBuf->GetDesc( &descBuf );
	   		
	   		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	    	ZeroMemory( &desc, sizeof(desc) );
	    	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	    	desc.BufferEx.FirstElement = 0;
	    	
	    	desc.Format = DXGI_FORMAT_UNKNOWN;
	        desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
	        
	        HRESULT hr = Engine().GetD3DDevice()->CreateShaderResourceView( pBuf, &desc, &m_srv);
			if (FAILED(hr)) {
				return false;
			}
	    }
		return true;
	}
	/*!
	*@brief	TextureData����SRV���쐬�B
	*@param[in]	texData		�e�N�X�`���f�[�^�B
	*/
	bool CShaderResourceView::Create(CTextureData& texData)
	{
		Release();
		//�V�F�[�_�[���\�[�X�r���[���쐬�B
		HRESULT hr = DirectX::CreateShaderResourceView(Engine().GetD3DDevice(), texData.GetImages(), texData.GetNumImages(), texData.GetMetadata(), &m_srv);
		if (FAILED(hr)) {
			TK_LOG("Failed CTexture::Init\n");
			return false;
		}
		return true;
	}
}