/*!
 * @brief	モデルシェーダー。
 */

#include "modelCB.h"
#include "modelStruct.h"
#include "modelSRV.h"
#include "sampleBRDF.h"


/*!--------------------------------------------------------------------------------------
 * @brief	スキンなしモデル用の頂点シェーダー。
-------------------------------------------------------------------------------------- */
PSInput VSMain( VSInputNmTxVcTangent In ) 
{
	PSInput psInput = (PSInput)0;
	float4 pos;
	pos = mul(mWorld, In.Position);
	psInput.Pos = pos;
	pos = mul(mView, pos);
	pos = mul(mProj, pos);
	psInput.Position = pos;
	psInput.TexCoord = In.TexCoord;
	psInput.Normal = mul(mWorld, In.Normal);
	
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
	psInput.Pos = pos;
	pos = mul(mView, pos);
	pos = mul(mProj, pos);
	psInput.Position = pos;
	psInput.TexCoord = In.TexCoord;
	psInput.Normal = mul(mWorld, In.Normal);
    return psInput;
}
//--------------------------------------------------------------------------------------
// ピクセルシェーダーのエントリ関数。
//--------------------------------------------------------------------------------------
float4 PSMain( PSInput In ) : SV_Target0
{
	float4 color = float4(Texture.Sample(Sampler, In.TexCoord).xyz, 1.0f);
	//従ベクトルと接ベクトルを求める。<-多くの場合で事前計算済みのデータとして、頂点データに埋め込まれているから計算しなくてよい。
//	float3 lig = max(0.0f, -dot( In.Normal, diffuseLightDir )) * diffuseLightColor;
	float3 biNormal;
	float3 tangentNormal;
	if(In.Normal.x < 0.001f && In.Normal.z < 0.001f){
		//法線がほぼ真上
		tangentNormal = normalize(cross(In.Normal, float3( 1.0f, 0.0f, 0.0f)));
	}else{
		tangentNormal = normalize(cross(In.Normal, float3( 0.0f, 1.0f, 0.0f)));
	}
	float3 toEye = normalize(eyeVec - In.Pos);
	biNormal = normalize(cross(In.Normal, tangentNormal));
	color.xyz = BRDF(-diffuseLightDir, toEye, In.Normal, tangentNormal, biNormal, color.xyz);
	color.xyz *= dot( In.Normal, -diffuseLightDir );
    return color; 
}

