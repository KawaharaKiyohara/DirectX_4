/*!
 * @brief	レンダリングターゲット。
 */

#pragma once

namespace tkEngine2{
	/*!
	 * @brief	レンダリングターゲット。
	 */
	class CRenderTarget{
	public:
		CRenderTarget();
		~CRenderTarget();
		/*!
		 *@brief	レンダリングターゲットの作成。
		 *@param[in]	w					レンダリングターゲットの幅。
		 *@param[in]	h					レンダリングターゲットの高さ。
		 *@param[in]	mipLevel			ミップマップレベル。0を指定した場合はミップマップがサポートされているGPUでは1*1ピクセルまでのミップマップが作成される。
		 *@param[in]	colorFormat			カラーバッファのフォーマット。
		 *@param[in]	depthStencilFormat	深度ステンシルバッファのフォーマット。
		 *@param[in]	multiSampleType		マルチサンプリングの種類。
		 *@param[in]	multiSampleQuality	マルチサンプリングのクオリティ。
		 */
		void Create(
			int w,
			int h,
			int mipLevel,
			D3DFORMAT colorFormat,
			D3DFORMAT depthStencilFormat,
			D3DMULTISAMPLE_TYPE multiSampleType,
			int multiSampleQuality
		);
		/*!
		 *@brief	レンダリングターゲットの破棄。
		 */
		void Release();
	private:
		ID3D11Texture2D*			m_renderTarget = nullptr;			//!<レンダリングターゲットとなるテクスチャ。
		ID3D11RenderTargetView*		m_renderTargetView = nullptr;		//!<レンダーターゲットビュー。
		ID3D11Texture2D*			m_depthStencil = nullptr;			//!<デプスステンシルとなるテクスチャ。
		ID3D11DepthStencilView*		m_depthStencilView = nullptr;		//!<デプスステンシルビュー。
	};
}