/*!
 *@brief	スキンモデル。
 */

#pragma once

namespace tkEngine2{
	class CSkinModelData;
	/*!
	 *@brief	スキンモデル。
	 */
	class CSkinModel : Noncopyable{
	public:
		CSkinModel();
		~CSkinModel();
		/*!
		*@brief	初期化
		*/
		void Init(CSkinModelData& modelData);
		/*!
		*@brief	更新。
		*@details
		*@param[in]		trans	平行移動。
		*@param[in]		rot		回転。
		*@param[in]		scale	拡大。
		*/
		void Update(const CVector3& trans, const CQuaternion& rot, const CVector3& scale);
		/*!
		*@brief	描画
		*@param[in]	viewMatrix		ビュー行列。
		*@param[in]	projMatrix		プロジェクション行列。
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
		CConstantBuffer m_cb;			//定数バッファ。
	};
}