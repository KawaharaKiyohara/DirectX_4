/*!
 * @brief	�����_�����O�R���e�L�X�g
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
		 * @brief	�������B
		 *@param[in]	pD3DDeviceContext	D3D�f�o�C�X�R���e�L�X�g�B�J���͌Ăяo�����Ƃōs���Ă��������B
		 */
		void Init(ID3D11DeviceContext* pD3DDeviceContext);
		/*!
		 * @brief	�����_�����O�^�[�Q�b�g�r���[��ݒ�B
		 * @details
		 *  ID3D11DeviceContext::OMSetRenderTargets�Ɠ����B
		 *@param[in]	NumViews			�o�C���h���郌���_�����O�^�[�Q�b�g�̐��B
		 *@param[in]	ppRenderTargetViews	�o�C���h���郌���_�����O�^�[�Q�b�g�̔z��ւ̃|�C���^�B
		 *@param[in]	pDepthStencilView	�o�C���h����[�x�X�e���V���r���[�ւ̃|�C���^�B
		 */
		void OMSetRenderTargets(unsigned int NumViews, ID3D11RenderTargetView *const *ppRenderTargetViews, ID3D11DepthStencilView *pDepthStencilView);
		/*!
		 * @brief	�r���[�|�[�g��ݒ�B
		 *@param[in]	topLeftX	�r���[�|�[�g�̍����X���W�B
		 *@param[in]	topLeftY	�r���[�|�[�g�̍����Y���W�B
		 *@param[in]	width		�r���[�|�[�g�̕��B
		 *@param[in]	height		�r���[�|�[�g�̍����B
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
		 * @brief	�����_�����O�^�[�Q�b�g���N���A�B
		 *@param[in]	rtNo	�����_�����O�^�[�Q�b�g�̔ԍ��B
		 *@param[in]	clearColor	�N���A�J���[�B
		 */
		void ClearRenderTargetView(unsigned int rtNo, float* clearColor)
		{
			if (rtNo < m_numRenderTargetView
				&& m_renderTargetViews != nullptr) {
				m_pD3DDeviceContext->ClearRenderTargetView(m_renderTargetViews[rtNo], clearColor);
			}
		}
		/*!
		* @brief	���_�o�b�t�@��ݒ�B
		*@param[in]	rtNo	�����_�����O�^�[�Q�b�g�̔ԍ��B
		*@param[in]	clearColor	�N���A�J���[�B
		*/
		void SetVertexBuffer(CVertexBuffer& vertexBuffer)
		{
			UINT offset = 0;
			UINT stride = vertexBuffer.GetStride();
			m_pD3DDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer.GetBody(), &stride, &offset);
		}
		/*!
		* @brief	�v���~�e�B�u�̃g�|���W�[��ݒ�B
		*@param[in]	topology	�g�|���W�[�B
		*/
		void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
		{
			m_pD3DDeviceContext->IASetPrimitiveTopology(topology);
		}
		/*!
		* @brief	���_�V�F�[�_�[��ݒ�B
		*@param[in]	shader		���_�V�F�[�_�[�B
		*/
		void VSSetShader(CShader& shader)
		{
			m_pD3DDeviceContext->VSSetShader((ID3D11VertexShader*)shader.GetBody(), NULL, 0);
		}
		/*!
		* @brief	�s�N�Z���V�F�[�_�[��ݒ�B
		*@param[in]	shader		���_�V�F�[�_�[�B
		*/
		void PSSetShader(CShader& shader)
		{
			m_pD3DDeviceContext->PSSetShader((ID3D11PixelShader*)shader.GetBody(), NULL, 0);
		}
		/*!
		* @brief	�R���s���[�g�V�F�[�_�[��ݒ�B
		*@param[in]	shader		�R���s���[�g�V�F�[�_�[�B
		*/
		void CSSetShader(CShader& shader)
		{
			m_pD3DDeviceContext->CSSetShader((ID3D11ComputeShader*)shader.GetBody(), NULL, 0);
		}
		/*!
		* @brief	�R���s���[�g�V�F�[�_�[��SRV��ݒ�B�B
		*@param[in]	slotNo		�X���b�g�ԍ�
		*@param[in]	srv			�V�F�[�_�[���\�[�X�r���[�B
		*/
		void CSSetShaderResource(int slotNo, CShaderResourceView& srv) 
		{
			m_pD3DDeviceContext->CSSetShaderResources(slotNo, 1, &srv.GetBody());
		}
		/*!
		* @brief	�R���s���[�g�V�F�[�_�[��UAV��ݒ�B
		*@param[in]	slotNo		�X���b�g�ԍ�
		*@param[in]	uav			UAV�B
		*/
		void CSSetUnorderedAccessView(int slotNo, CUnorderedAccessView& uav)
		{
			m_pD3DDeviceContext->CSSetUnorderedAccessViews(slotNo, 1, &uav.GetBody(), NULL);
		}
		/*!
		* @brief	�`��B
		* @param[in]	vertexCount			���_���B
		* @param[in]	startVertexLocation	�`����J�n���钸�_�̈ʒu�B���0�ő��v�B
		*/
		void Draw(
			unsigned int vertexCount,
			unsigned int startVertexLocation
		)
		{
			m_pD3DDeviceContext->Draw(vertexCount, startVertexLocation);
		}
		/*!
		* @brief	�f�B�X�p�b�`�B
		* @details
		*  �R���s���[�g�V�F�[�_�[�����s�B
		* @param[in]	threadGroupCountX	x �����Ƀf�B�X�p�b�`�����O���[�v�̐��B
		* @param[in]	threadGroupCountY	y �����Ƀf�B�X�p�b�`�����O���[�v�̐��B
		* @param[in]	thredGroupCountZ	�� �����Ƀf�B�X�p�b�`�����O���[�v�̐��B
		*/
		void Dispatch(UINT threadGroupCountX, UINT threadGroupCountY, UINT thredGroupCountZ)
		{
			m_pD3DDeviceContext->Dispatch(threadGroupCountX, threadGroupCountY, thredGroupCountZ);
		}
		/*!
		* @brief	���̓��C�A�E�g��ݒ�B
		*/
		void SetInputLayout(ID3D11InputLayout* inputLayout)
		{
			m_pD3DDeviceContext->IASetInputLayout(inputLayout);
		}
		/*!
		* @brief	���\�[�X���R�s�[�B
		*@param[out]	destRes		�R�s�[��B
		*@param[in]		srcRes		�R�s�[���B
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
		ID3D11DeviceContext*			m_pD3DDeviceContext = nullptr;	//!<D3D�f�o�C�X�R���e�L�X�g�B
		D3D11_VIEWPORT 					m_viewport;						//!<�r���[�|�[�g�B
		ID3D11RenderTargetView *const* 	m_renderTargetViews = nullptr;	//!<���ݎg�p����Ă��郌���_�����O�^�[�Q�b�g�r���[�B
		ID3D11DepthStencilView*			m_depthStencilView = nullptr;	//!<���ݐݒ肳��Ă���f�v�X�X�e���V���r���[�B
		unsigned int 					m_numRenderTargetView = 0;		//!<�����_�����O�^�[�Q�b�g�r���[�̐��B
	};
}