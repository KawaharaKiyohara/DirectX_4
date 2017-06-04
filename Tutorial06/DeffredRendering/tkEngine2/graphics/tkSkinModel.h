/*!
 *@brief	�X�L�����f���B
 */

#pragma once

namespace tkEngine2{
	class CSkinModelData;
	/*!
	 *@brief	�X�L�����f���B
	 */
	class CSkinModel : Noncopyable{
	public:
		CSkinModel();
		~CSkinModel();
		/*!
		*@brief	������
		*/
		void Init(CSkinModelData& modelData);
		/*!
		*@brief	�X�V�B
		*@details
		*@param[in]		trans	���s�ړ��B
		*@param[in]		rot		��]�B
		*@param[in]		scale	�g��B
		*/
		void Update(const CVector3& trans, const CQuaternion& rot, const CVector3& scale);
		/*!
		*@brief	�`��
		*@param[in]	viewMatrix		�r���[�s��B
		*@param[in]	projMatrix		�v���W�F�N�V�����s��B
		*/
		void Draw(CRenderContext& renderContext, const CMatrix& viewMatrix, const CMatrix& projMatrix);
	private:
		struct SVSConstantBuffer {
			CMatrix mWorld;
			CMatrix mView;
			CMatrix mProj;
		};
		CSkinModelData*	m_skinModelData = nullptr;
		CMatrix m_worldMatrix = CMatrix::Identity;
		CConstantBuffer m_cb;			//�萔�o�b�t�@�B
	};
}