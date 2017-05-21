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
		 *@param[in]	multiSampleDesc		マルチサンプリングの定義。
		 *@param[in]	renderTarget		レンダリングターゲットとなるテクスチャ。nullptrが指定されていたら、内部でテクスチャを生成する。
		 *@param[in]	depthStencil		デプスステンシルとなるテクスチャ。nullptrが指定されていたら、内部でテクスチャを生成する。
		 *@return	trueが返ってきたら作成成功。
		 */
		bool Create(
			int w,
			int h,
			int mipLevel,
			DXGI_FORMAT colorFormat,
			DXGI_FORMAT depthStencilFormat,
			DXGI_SAMPLE_DESC multiSampleDesc,
			ID3D11Texture2D* renderTarget = nullptr,
			ID3D11Texture2D* depthStencil = nullptr
		);
		/*!
		 *@brief	レンダリングターゲットの破棄。
		 */
		void Release();
		/*!
		*@brief	レンダリングターゲットを取得。
		*/
		ID3D11Texture2D* GetRenderTarget()
		{
			return m_renderTarget;
		}
		/*!
		*@brief	レンダリングターゲットビューの取得。
		*/
		ID3D11RenderTargetView* GetRenderTargetView()
		{
			return m_renderTargetView;
		}
		/*!
		*@brief	デプスステンシルビューの取得。
		*/
		ID3D11DepthStencilView* GetDepthStencilView()
		{
			return m_depthStencilView;
		}
	
	private:
		ID3D11Texture2D*			m_renderTarget = nullptr;			//!<レンダリングターゲットとなるテクスチャ。
		ID3D11RenderTargetView*		m_renderTargetView = nullptr;		//!<レンダーターゲットビュー。
		ID3D11Texture2D*			m_depthStencil = nullptr;			//!<デプスステンシルとなるテクスチャ。
		ID3D11DepthStencilView*		m_depthStencilView = nullptr;		//!<デプスステンシルビュー。
	};
}