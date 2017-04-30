/*!
 * @brief	レンダリングコンテキスト
 */

#pragma once


#include "tkEngine2/graphics/tkShader.h"
#include "tkEngine2/graphics/GPUBuffer/tkVertexBuffer.h"
#include "tkEngine2/graphics/GPUView/tkShaderResourceView.h"
#include "tkEngine2/graphics/GPUView/tkUnorderedAccessView.h"

namespace tkEngine2{
	class CVertexBuffer;
	class CRenderContext : Noncopyable{
	public:
		CRenderContext(){}
		~CRenderContext(){}
		/*!
		 * @brief	初期化。
		 *@param[in]	pD3DDeviceContext	D3Dデバイスコンテキスト。開放は呼び出しもとで行ってください。
		 */
		void Init(ID3D11DeviceContext* pD3DDeviceContext);
		/*!
		 * @brief	レンダリングターゲットビューを設定。
		 * @details
		 *  ID3D11DeviceContext::OMSetRenderTargetsと同じ。
		 *@param[in]	NumViews			バインドするレンダリングターゲットの数。
		 *@param[in]	ppRenderTargetViews	バインドするレンダリングターゲットの配列へのポインタ。
		 *@param[in]	pDepthStencilView	バインドする深度ステンシルビューへのポインタ。
		 */
		void OMSetRenderTargets(unsigned int NumViews, ID3D11RenderTargetView *const *ppRenderTargetViews, ID3D11DepthStencilView *pDepthStencilView);
		/*!
		 * @brief	ビューポートを設定。
		 *@param[in]	topLeftX	ビューポートの左上のX座標。
		 *@param[in]	topLeftY	ビューポートの左上のY座標。
		 *@param[in]	width		ビューポートの幅。
		 *@param[in]	height		ビューポートの高さ。
		 */
		void SetViewport( float topLeftX, float topLeftY, float width, float height )
		{
			m_viewport.Width = width;
			m_viewport.Height = height;
			m_viewport.TopLeftX = topLeftX;
			m_viewport.TopLeftY = topLeftY;
			m_viewport.MinDepth = 0.0f;
			m_viewport.MaxDepth = 1.0f;
			m_pD3DDeviceContext->RSSetViewports(1, &m_viewport);
		}
		/*!
		 * @brief	レンダリングターゲットをクリア。
		 *@param[in]	rtNo	レンダリングターゲットの番号。
		 *@param[in]	clearColor	クリアカラー。
		 */
		void ClearRenderTargetView(unsigned int rtNo, float* clearColor)
		{
			if (rtNo < m_numRenderTargetView
				&& m_renderTargetViews != nullptr) {
				m_pD3DDeviceContext->ClearRenderTargetView(m_renderTargetViews[rtNo], clearColor);
			}
		}
		/*!
		* @brief	頂点バッファを設定。
		*@param[in]	rtNo	レンダリングターゲットの番号。
		*@param[in]	clearColor	クリアカラー。
		*/
		void SetVertexBuffer(CVertexBuffer& vertexBuffer)
		{
			UINT offset = 0;
			UINT stride = vertexBuffer.GetStride();
			m_pD3DDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer.GetBody(), &stride, &offset);
		}
		/*!
		* @brief	プリミティブのトポロジーを設定。
		*@param[in]	topology	トポロジー。
		*/
		void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
		{
			m_pD3DDeviceContext->IASetPrimitiveTopology(topology);
		}
		/*!
		* @brief	頂点シェーダーを設定。
		*@param[in]	shader		頂点シェーダー。
		*/
		void VSSetShader(CShader& shader)
		{
			m_pD3DDeviceContext->VSSetShader((ID3D11VertexShader*)shader.GetBody(), NULL, 0);
		}
		/*!
		* @brief	ピクセルシェーダーを設定。
		*@param[in]	shader		頂点シェーダー。
		*/
		void PSSetShader(CShader& shader)
		{
			m_pD3DDeviceContext->PSSetShader((ID3D11PixelShader*)shader.GetBody(), NULL, 0);
		}
		/*!
		* @brief	コンピュートシェーダーを設定。
		*@param[in]	shader		コンピュートシェーダー。
		*/
		void CSSetShader(CShader& shader)
		{
			m_pD3DDeviceContext->CSSetShader((ID3D11ComputeShader*)shader.GetBody(), NULL, 0);
		}
		/*!
		* @brief	コンピュートシェーダーにSRVを設定。。
		*@param[in]	slotNo		スロット番号
		*@param[in]	srv			シェーダーリソースビュー。
		*/
		void CSSetShaderResource(int slotNo, CShaderResourceView& srv) 
		{
			m_pD3DDeviceContext->CSSetShaderResources(slotNo, 1, &srv.GetBody());
		}
		/*!
		* @brief	コンピュートシェーダーにUAVを設定。
		*@param[in]	slotNo		スロット番号
		*@param[in]	uav			UAV。
		*/
		void CSSetUnorderedAccessView(int slotNo, CUnorderedAccessView& uav)
		{
			m_pD3DDeviceContext->CSSetUnorderedAccessViews(slotNo, 1, &uav.GetBody(), NULL);
		}
		/*!
		* @brief	描画。
		* @param[in]	vertexCount			頂点数。
		* @param[in]	startVertexLocation	描画を開始する頂点の位置。大抵0で大丈夫。
		*/
		void Draw(
			unsigned int vertexCount,
			unsigned int startVertexLocation
		)
		{
			m_pD3DDeviceContext->Draw(vertexCount, startVertexLocation);
		}
		/*!
		* @brief	ディスパッチ。
		* @details
		*  コンピュートシェーダーを実行。
		* @param[in]	threadGroupCountX	x 方向にディスパッチしたグループの数。
		* @param[in]	threadGroupCountY	y 方向にディスパッチしたグループの数。
		* @param[in]	thredGroupCountZ	ｚ 方向にディスパッチしたグループの数。
		*/
		void Dispatch(UINT threadGroupCountX, UINT threadGroupCountY, UINT thredGroupCountZ)
		{
			m_pD3DDeviceContext->Dispatch(threadGroupCountX, threadGroupCountY, thredGroupCountZ);
		}
		/*!
		* @brief	入力レイアウトを設定。
		*/
		void SetInputLayout(ID3D11InputLayout* inputLayout)
		{
			m_pD3DDeviceContext->IASetInputLayout(inputLayout);
		}
		/*!
		* @brief	リソースをコピー。
		*@param[out]	destRes		コピー先。
		*@param[in]		srcRes		コピー元。
		*/
		template<class TResource>
		void CopyResource(TResource& destRes, TResource& srcRes)
		{
			m_pD3DDeviceContext->CopyResource(destRes.GetBody(), srcRes.GetBody());
		}
		template<class TBuffer>
		void Map(TBuffer& buffer, UINT subresource, D3D11_MAP mapType, UINT mapFlags, D3D11_MAPPED_SUBRESOURCE& mappedResource)
		{
			m_pD3DDeviceContext->Map(buffer.GetBody(), subresource, mapType, mapFlags, &mappedResource);
		}
		template<class TBuffer>
		void Unmap(TBuffer& buffer, UINT subresource)
		{
			m_pD3DDeviceContext->Unmap(buffer.GetBody(), subresource);
		}
	private:
		ID3D11DeviceContext*			m_pD3DDeviceContext = nullptr;	//!<D3Dデバイスコンテキスト。
		D3D11_VIEWPORT 					m_viewport;						//!<ビューポート。
		ID3D11RenderTargetView *const* 	m_renderTargetViews = nullptr;	//!<現在使用されているレンダリングターゲットビュー。
		ID3D11DepthStencilView*			m_depthStencilView = nullptr;	//!<現在設定されているデプスステンシルビュー。
		unsigned int 					m_numRenderTargetView = 0;		//!<レンダリングターゲットビューの数。
	};
}