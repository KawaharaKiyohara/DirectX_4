/*!
 * @brief	�e�N�X�`���f�[�^�B
 */

#pragma once


namespace tkEngine2{
	/*!
	 * @brief	�e�N�X�`���f�[�^�B
	 */
	class CTextureData : Noncopyable{
	public:
		CTextureData()
		{
		}
		~CTextureData()
		{
		}
		/*!
		 *@brief	�e�N�X�`����ǂݍ��݁B
		 */
		bool Load(const LPCWSTR fileName);
		/*!
		*@brief	�摜�C���[�W���擾�B
		*/
		const DirectX::Image* GetImages() const
		{
			return m_scratchImage.GetImages();
		}
		/*!
		*@brief	�摜�C���[�W�̐����擾�B
		*/
		int GetNumImages() const
		{
			return m_scratchImage.GetImageCount();
		}
		/*!
		*@brief	�摜�̃��^�f�[�^���擾�B
		*/
		const DirectX::TexMetadata& GetMetadata() const
		{
			return m_texMetadata;
		}
	private:
		DirectX::TexMetadata		m_texMetadata;		//!<�e�N�X�`�����^�f�[�^�B
		DirectX::ScratchImage		m_scratchImage;		//!<
	};
}