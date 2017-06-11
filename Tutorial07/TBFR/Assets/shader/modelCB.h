/*!
 * @brief	モデル用の定数バッファ。
 */
/*!
 * @brief	頂点シェーダー用の定数バッファ。
 */
cbuffer VSPSCb : register(b0){
	float4x4 mWorld;
	float4x4 mView;
	float4x4 mProj;
	float4 screenParam;	// スクリーンパラメータ(near, far, screenWidth, screenHeight)
	int isZPrepass;		//Zプリパス？
};
