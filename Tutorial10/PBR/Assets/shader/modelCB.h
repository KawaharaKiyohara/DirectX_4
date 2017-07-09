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

/*!
 * @brief	ライト用の定数バッファ。
 */
cbuffer lightCb : register(b1)
{
	float4 diffuseLightDir;
	float4 diffuseLightColor;
	float4 ambientLight;
	float3 eyeVec;
};

cbuffer MaterialParamCb : register(b2){
	float roughness;	//粗さ。
	float metallic ;	//メタリック。
};