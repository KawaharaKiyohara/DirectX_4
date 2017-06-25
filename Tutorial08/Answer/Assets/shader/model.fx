/*!
 * @brief	モデルシェーダー。
 */

#include "modelCB.h"
#include "modelStruct.h"
#include "modelSRV.h"

#define TILE_WIDTH	16		//タイルの幅。
#define USE_LIM_LIGHT		//有効にするとリムライト用のサンプル。

/*!--------------------------------------------------------------------------------------
 * @brief	スキンなしモデル用の頂点シェーダー。
-------------------------------------------------------------------------------------- */
PSInput VSMain( VSInputNmTxVcTangent In ) 
{
	PSInput psInput = (PSInput)0;
	float4 pos;
	pos = mul(mWorld, In.Position);
	pos = mul(mView, pos);
	psInput.posInView = pos;
	pos = mul(mProj, pos);
	psInput.Position = pos;
	psInput.TexCoord = In.TexCoord;
	psInput.Normal = mul(mWorld, In.Normal);
	psInput.Pos = pos;
    return psInput;
}
/*!--------------------------------------------------------------------------------------
 * @brief	スキンありモデル用の頂点シェーダー。
-------------------------------------------------------------------------------------- */
PSInput VSMainSkin( VSInputNmTxWeights In ) 
{
	PSInput psInput = (PSInput)0;
	float4 pos;
	pos = mul(mWorld, In.Position);
	pos = mul(mView, pos);
	psInput.posInView = pos;
	pos = mul(mProj, pos);
	psInput.Position = pos;
	psInput.TexCoord = In.TexCoord;
	psInput.Normal = mul(mWorld, In.Normal);
	psInput.Pos = pos;
    return psInput;
}
//--------------------------------------------------------------------------------------
// ピクセルシェーダーのエントリ関数。
//--------------------------------------------------------------------------------------
float4 PSMain( PSInput In ) : SV_Target0
{
	if(isZPrepass){
		//ZPrepass
		return In.posInView.z;
	}
	//スクリーンの左上を(0,0)、右下を(1,1)とする座標系に変換する。
	float2 screenPos = (In.Pos.xy / In.Pos.w) * float2(0.5f, -0.5f) + 0.5f;
	float2 offset = float2( 1.25f/screenParam.z, 1.25f / screenParam.w);
	
	float depth_0 = depthTexture.Sample(Sampler, screenPos).x;


	float2 offsetTbl[] = {
		float2( -offset.x, -offset.y),		//左上
		float2( 0.0f, 	   -offset.y),		//上
		float2( offset.x, -offset.y),		//右上
		float2( offset.x,  0.0f),			//右
		float2( -offset.x, 0.0f),			//左
		float2( -offset.x, +offset.y),		//左下
		float2( 0.0f, 	   +offset.y),		//下
		float2( offset.x,  +offset.y),		//右下
	};

	for(int i = 0; i < 8; i++){
		float2 screenPos2 = screenPos + offsetTbl[i];

		float depth_1 = depthTexture.Sample(Sampler, screenPos2).x;
		if(depth_1 - depth_0 > 0.025f ){
			//エッジの色を返す。
			return 0.0f;
		}
	}

	//ライトの方向はとりあえず固定にしとく<-ほんとは定数バッファとかで送るべき。
#ifdef USE_LIM_LIGHT
	//リムライト用のサンプルではライトはユニティちゃんの真後ろから当たる。
	float3 lightDir = float3(0.0f, 0.0f, 1.0f);
#else
	float3 lightDir = float3(1.0f, 0.0f, 0.0f);
#endif
	//2階調の陰影を求める。
	//ライトの方向と法線の内積を取るとライトの強さが求まる。
	//強さが一定値以下(今回は0.2)なら暗いテクスチャを使う。
	//それ以上なら明るいテクスチャを使う。
	float4 color = 0.0f;
	if(dot(-lightDir, In.Normal) < 0.2f){
		color = float4(darkTexture.Sample(Sampler, In.TexCoord).xyz, 1.0f);
	}else{
		color = float4(lightTexture.Sample(Sampler, In.TexCoord).xyz, 1.0f);
	}

#ifdef USE_LIM_LIGHT
	//リムを当てる。
	float3 eyeDir = float3(0.0f, 0.0f, -1.0f);			//視線は固定にしておく(カメラを回したらアウト。ほんとは定数バッファとかで送るべき。
	float lim = (1.0f - dot(-In.Normal, eyeDir))* 2.0f;	//カメラの方向と垂直な場合逆光の強さが強くなる。
	lim *= dot(eyeDir, -lightDir);						//完全に逆行していたらbaseLimの値がそのままでる。
	if(lim > 0.7f){
		//逆光が発生している。
		color = float4(lightTexture.Sample(Sampler, In.TexCoord).xyz, 1.0f);
	}
#endif
	
    return color; 
}

