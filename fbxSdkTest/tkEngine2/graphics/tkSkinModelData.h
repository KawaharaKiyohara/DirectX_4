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
		bool Load( const char* filePath );
	private:
		/*!
		*@brief	シーンを解析。。
		*@param[in]	node	ノード。
		*/
		bool Analyze(FbxNode* node);
	};
}