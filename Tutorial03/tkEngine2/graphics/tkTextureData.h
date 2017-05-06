/*!
 * @brief	テクスチャデータ。
 */

#pragma once


namespace tkEngine2{
	/*!
	 * @brief	テクスチャデータ。
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
		 *@brief	テクスチャを読み込み。
		 */
		bool Load(const LPCWSTR fileName);
		/*!
		*@brief	画像イメージを取得。
		*/
		const DirectX::Image* GetImages() const
		{
			return m_scratchImage.GetImages();
		}
		/*!
		*@brief	画像イメージの数を取得。
		*/
		int GetNumImages() const
		{
			return m_scratchImage.GetImageCount();
		}
		/*!
		*@brief	画像のメタデータを取得。
		*/
		const DirectX::TexMetadata& GetMetadata() const
		{
			return m_texMetadata;
		}
	private:
		DirectX::TexMetadata		m_texMetadata;		//!<テクスチャメタデータ。
		DirectX::ScratchImage		m_scratchImage;		//!<
	};
}