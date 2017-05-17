/*!
 * @brief	�����_�����O�R���e�L�X�g�B
 */

#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/graphics/tkRenderContext.h"


namespace tkEngine2{
	void CRenderContext::Init(ID3D11DeviceContext* pD3DDeviceContext)
	{
		TK_ASSERT(pD3DDeviceContext != nullptr, "pD3DDeviceContext��NULL�ł��B�쐬���Ă���Ă�łˁB");
		m_pD3DDeviceContext = pD3DDeviceContext;
	}
	void CRenderContext::OMSetRenderTargets(unsigned int NumViews, ID3D11RenderTargetView *const *ppRenderTargetViews, ID3D11DepthStencilView *pDepthStencilView)
	{
		m_pD3DDeviceContext->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
		m_renderTargetViews = ppRenderTargetViews;
		m_depthStencilView = pDepthStencilView;
		m_numRenderTargetView = NumViews;
	}
}