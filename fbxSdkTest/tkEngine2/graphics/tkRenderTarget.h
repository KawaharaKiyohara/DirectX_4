/*!
 * @brief	�����_�����O�^�[�Q�b�g�B
 */

#pragma once

namespace tkEngine2{
	/*!
	 * @brief	�����_�����O�^�[�Q�b�g�B
	 */
	class CRenderTarget{
	public:
		CRenderTarget();
		~CRenderTarget();
		/*!
		 *@brief	�����_�����O�^�[�Q�b�g�̍쐬�B
		 *@param[in]	w					�����_�����O�^�[�Q�b�g�̕��B
		 *@param[in]	h					�����_�����O�^�[�Q�b�g�̍����B
		 *@param[in]	mipLevel			�~�b�v�}�b�v���x���B0���w�肵���ꍇ�̓~�b�v�}�b�v���T�|�[�g����Ă���GPU�ł�1*1�s�N�Z���܂ł̃~�b�v�}�b�v���쐬�����B
		 *@param[in]	colorFormat			�J���[�o�b�t�@�̃t�H�[�}�b�g�B
		 *@param[in]	depthStencilFormat	�[�x�X�e���V���o�b�t�@�̃t�H�[�}�b�g�B
		 *@param[in]	multiSampleType		�}���`�T���v�����O�̎�ށB
		 *@param[in]	multiSampleQuality	�}���`�T���v�����O�̃N�I���e�B�B
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
		 *@brief	�����_�����O�^�[�Q�b�g�̔j���B
		 */
		void Release();
	private:
		ID3D11Texture2D*			m_renderTarget = nullptr;			//!<�����_�����O�^�[�Q�b�g�ƂȂ�e�N�X�`���B
		ID3D11RenderTargetView*		m_renderTargetView = nullptr;		//!<�����_�[�^�[�Q�b�g�r���[�B
		ID3D11Texture2D*			m_depthStencil = nullptr;			//!<�f�v�X�X�e���V���ƂȂ�e�N�X�`���B
		ID3D11DepthStencilView*		m_depthStencilView = nullptr;		//!<�f�v�X�X�e���V���r���[�B
	};
}