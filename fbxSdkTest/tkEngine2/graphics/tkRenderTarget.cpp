/*!
 * @brief	レンダリングターゲット。
 */

#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/graphics/tkRenderTarget.h"


namespace tkEngine2{
	CRenderTarget::CRenderTarget()
	{
	}
	CRenderTarget::~CRenderTarget()
	{
		Release();
	}
	void CRenderTarget::Create(
		int w,
		int h,
		int mipLevel,
		D3DFORMAT colorFormat,
		D3DFORMAT depthStencilFormat,
		D3DMULTISAMPLE_TYPE multiSampleType,
		int multiSampleQuality
	)
	{
	}
	void CRenderTarget::Release()
	{
	}
}