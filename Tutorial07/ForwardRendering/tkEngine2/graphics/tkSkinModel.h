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
		/*!
		*@brief QueryEffectsを実行したときのコールバックに渡される引数構造体。
		*/
		struct SQuerryEffectsCbArg {
			DirectX::ModelMeshPart*	meshPart;		//現在のエフェクト。
		};
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
		*@param[in]	isZPrepass		ZPrepass?
		*/
		void Draw(
			CRenderContext& renderContext, 
			const CMatrix& viewMatrix, 
			const CMatrix& projMatrix,
			bool isZPrepass = false
		);
	private:
		struct SVSConstantBuffer {
			CMatrix mWorld;
			CMatrix mView;
			CMatrix mProj;
			CVector4 screenParam;
			int isZPrepass;
		};
		CSkinModelData*	m_skinModelData = nullptr;
		CMatrix m_worldMatrix = CMatrix::Identity;
		CConstantBuffer m_cb;			//定数バッファ。
	};
}