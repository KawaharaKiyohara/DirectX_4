/*!
 * @brief	レンダリングターゲット。
 */

#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/graphics/tkRenderTarget.h"


namespace tkEngine2{
	CRenderTarget::CRenderTarget()
	{
	}
	CRenderTarget::~CRenderTarget()
	{
		Release();
	}
	bool CRenderTarget::Create(
		int w,
		int h,
		int mipLevel,
		DXGI_FORMAT colorFormat,
		DXGI_FORMAT depthStencilFormat,
		DXGI_SAMPLE_DESC multiSampleDesc,
		ID3D11Texture2D* renderTarget,
		ID3D11Texture2D* depthStencil
	)
	{
		Release();
		//レンダリングターゲットの作成。
		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));
		texDesc.Width = w;
		texDesc.Height = h;
		texDesc.MipLevels = mipLevel;
		texDesc.ArraySize = 1;
		texDesc.Format = colorFormat;
		texDesc.SampleDesc = multiSampleDesc;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		ID3D11Device* pD3DDevice = Engine().GetD3DDevice();
		HRESULT hr;
		if (renderTarget == nullptr) {
			hr = pD3DDevice->CreateTexture2D(&texDesc, NULL, &m_renderTarget);
			if (FAILED(hr)) {
				//レンダリングターゲットの作成に失敗。
				return false;
			}
		}
		else {
			//レンダリングターゲットが指定されている。
			m_renderTarget = renderTarget;
			m_renderTarget->AddRef();	//参照カウンタを加算する。
		}
		//レンダリングターゲットビューを作成。
		hr = pD3DDevice->CreateRenderTargetView(m_renderTarget, NULL, &m_renderTargetView);
		if (FAILED(hr)) {
			return false;
		}
		//レンダリングターゲットのSRVを作成。
		m_renderTargetSRV.Create(m_renderTarget);
		//レンダリングターゲットのUAVを作成。
		m_renderTargetUAV.Create(m_renderTarget);

		if (depthStencilFormat != DXGI_FORMAT_UNKNOWN) {
			//デプスステンシルを作成。
			texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			texDesc.Format = depthStencilFormat;
			if (depthStencil == nullptr) {
				hr = pD3DDevice->CreateTexture2D(&texDesc, NULL, &m_depthStencil);
				if (FAILED(hr)) {
					return false;
				}
			}
			else {
				//デプスステンシルが指定されている。
				m_depthStencil = depthStencil;
				m_depthStencil->AddRef();	//参照カウンタを増やす。
			}
			//デプスステンシルビューを作成。
			// Create the depth stencil view
			D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
			ZeroMemory(&descDSV, sizeof(descDSV));
			descDSV.Format = texDesc.Format;
			descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			descDSV.Texture2D.MipSlice = 0;
			hr = pD3DDevice->CreateDepthStencilView(m_depthStencil, &descDSV, &m_depthStencilView);
			if (FAILED(hr)) {
				return false;
			}
		}
		return true;
	}
	void CRenderTarget::Release()
	{
		m_renderTargetSRV.Release();
		m_renderTargetUAV.Release();
		if(m_renderTarget != nullptr){
			m_renderTarget->Release();
			m_renderTarget = nullptr;
		}
		if(m_renderTargetView != nullptr){
			m_renderTargetView->Release();
			m_renderTargetView = nullptr;
		}
		if(m_depthStencil != nullptr){
			m_depthStencil->Release();
			m_depthStencil = nullptr;
		}
		if(m_depthStencilView != nullptr){
			m_depthStencilView->Release();
			m_depthStencilView = nullptr;
		}
	}
}