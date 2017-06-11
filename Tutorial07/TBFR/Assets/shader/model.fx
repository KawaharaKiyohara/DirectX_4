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
	//ビューポート座標系に変換する。
	float2 viewportPos = screenParam.zw * screenPos;
	//スクリーンをタイルで分割したときのセルのX座標を求める。
	uint numCellX = (screenParam.z + TILE_WIDTH - 1) / TILE_WIDTH;
	//タイルインデックスを計算する。
	uint tileIndex = floor( viewportPos.x / TILE_WIDTH ) + floor( viewportPos.y / TILE_WIDTH ) * numCellX;
	
	//ポイントライトの数を取得。
	uint numLights, dummy;
	pointLightList.GetDimensions(numLights, dummy);
	//このピクセルが含まれるタイルのライトインデックスリストの開始位置を計算する。
	uint lightStart = tileIndex * numLights;
	//このピクセルが含まれるタイルのライトインデックスリストの終了位置を計算する。
	uint lightEnd = lightStart + numLights;
	
	float3 lig = 0.0f;
	for (uint lightListIndex = lightStart; lightListIndex < lightEnd; lightListIndex++){
		uint lightIndex = pointLightListInTile[lightListIndex];
		if(lightIndex == 0xffffffff){
			//このタイルに含まれるポイントライトはもうない。
			break;
		}
		SPointLight light = pointLightList[lightIndex];
		float3 lightDir = In.wPos - light.position;
		float len = length(lightDir);
		lightDir = normalize(lightDir);	//正規化。
		float3 pointLightColor = saturate(-dot(In.Normal, lightDir)) * light.color.xyz;
		//減衰を計算する。
		float	litRate = len / light.attn.x;
		float	attn = max(1.0 - litRate * litRate, 0.0);
		pointLightColor *= attn;
		lig += pointLightColor;
	}
	float4 color = float4(Texture.Sample(Sampler, In.TexCoord).xyz, 1.0f);
	color.xyz *= lig;
    return color; 
}

