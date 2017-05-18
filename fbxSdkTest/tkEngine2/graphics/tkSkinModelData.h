/*!
 *@brief	�X�L�����f���f�[�^�B
 */

#pragma once

namespace tkEngine2{
	/*!
	 *@brief	�X�L�����f���f�[�^�B
	 */
	class CSkinModelData : Noncopyable{
	public:
		/*!
		 *@brief	�R���X�g���N�^�B
		 */
		CSkinModelData();
		/*!
		 *@brief	�f�X�g���N�^�B
		 */
		~CSkinModelData();
		/*!
		 *@brief	���[�h�B
		 *@param[in]	filePath	�t�@�C���p�X�B
		 *@return	true���Ԃ��Ă����烍�[�h�����B
		 */
		bool Load( const char* filePath );
	private:
		/*!
		*@brief	�V�[������́B�B
		*@param[in]	node	�m�[�h�B
		*/
		bool Analyze(FbxNode* node);
	};
}