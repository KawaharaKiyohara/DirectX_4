/*!
 *@brief	スキンモデルデータ。
 */

#pragma once

namespace tkEngine2{
	/*!
	 *@brief	スキンモデルデータ。
	 */
	class CSkinModelData : Noncopyable{
	public:
		/*!
		 *@brief	コンストラクタ。
		 */
		CSkinModelData();
		/*!
		 *@brief	デストラクタ。
		 */
		~CSkinModelData();
		/*!
		 *@brief	ロード。
		 *@param[in]	filePath	ファイルパス。
		 *@return	trueが返ってきたらロード成功。
		 */
		bool Load( const wchar_t* filePath );
		DirectX::Model& GetBody()
		{
			return *m_modelDx;
		}
	private:
		std::unique_ptr<DirectX::Model> m_modelDx;
	};
}