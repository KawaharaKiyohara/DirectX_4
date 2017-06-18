/*!
 * @brief	モデルシェーダー。
 */

#include "modelCB.h"
#include "modelStruct.h"
#include "modelSRV.h"

#define TILE_WIDTH	16		//タイルの幅。


/*!--------------------------------------------------------------------------------------
 * @brief	スキンなしモデル用の頂点シェーダー。
-------------------------------------------------------------------------------------- */
PSInput VSMain( VSInputNmTxVcTangent In ) 
{
	PSInput psInput = (PSInput)0;
	float4 pos;
	pos = mul(mWorld, In.Position);
	psInput.wPos = pos.xyz;
	pos = mul(mView, pos);
	pos = mul(mProj, pos);
	psInput.Position = pos;
	psInput.TexCoord = In.TexCoord;
	psInput.Normal = In.Normal;
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
	psInput.wPos = pos.xyz;
	pos = mul(mView, pos);
	pos = mul(mProj, pos);
	psInput.Position = pos;
	psInput.TexCoord = In.TexCoord;
	psInput.Normal = In.Normal;
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
		return In.Pos.z / In.Pos.w;
	}
	//スクリーンの左上を(0,0)、右下を(1,1)とする座標系に変換する。
	float2 screenPos = (In.Pos.xy / In.Pos.w) * float2(0.5f, -0.5f) + 0.5f;
	float2 offset = float2( 2.0f/screenParam.z, 2.0f / screenParam.w);
	
	float depth_0 = depthTexture.Sample(Sampler, screenPos).x;

	float2 screenPos2;
	screenPos2 = screenPos;
	screenPos2.x = screenPos.x + offset.x;
	float depth_1 = depthTexture.Sample(Sampler, screenPos2).x;
	if(abs(depth_0 - depth_1) > 0.2f ){
		//エッジの色を返す。
		return 0.0f;
	}
	screenPos2 = screenPos;
	screenPos2.x = screenPos.x - offset.x;
	depth_1 = depthTexture.Sample(Sampler, screenPos2).x;
	if(abs(depth_0 - depth_1) > 0.2f ){
		//エッジの色を返す。
		return 0.0f;
	}
	screenPos2 = screenPos;
	screenPos2.y = screenPos.y + offset.y;
	depth_1 = depthTexture.Sample(Sampler, screenPos2).x;
	if(abs(depth_0 - depth_1) > 0.2f ){
		//エッジの色を返す。
		return 0.0f;
	}
	screenPos2 = screenPos;
	screenPos2.y = screenPos.y - offset.y;
	depth_1 = depthTexture.Sample(Sampler, screenPos2).x;
	if(abs(depth_0 - depth_1) > 0.2f ){
		//エッジの色を返す。
		return 0.0f;
	}
	
	float4 color = float4(lightTexture.Sample(Sampler, In.TexCoord).xyz, 1.0f);
    return color; 
}

