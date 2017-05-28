/*!
 * @brief	ディファードライティング。。
 */


struct VSIn{
	float4 pos : SV_Position;
	float2 tex : TEXCOORD0;
};
struct PSIn{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};
/*!
 * @brief	ライト用の定数バッファ。
 */
cbuffer cbLight : register(b0){
	float4 diffuseLightDir;			//ディフューズライトの方向。
	float4 diffuseLightColor;		//ディフューズライトのカラー。
};
//ディフューズテクスチャ。
Texture2D<float4> diffuseTexture : register(t0);
//法線テクスチャ。
Texture2D<float4> normalTexture : register(t1);
sampler Sampler : register(s0);

//--------------------------------------------------------------------------------------
// 頂点シェーダーのエントリ関数。
//--------------------------------------------------------------------------------------
PSIn VSMain(VSIn In) 
{
	PSIn psIn;
	psIn.pos = In.pos;
	psIn.tex = In.tex;
	return psIn;
}
//--------------------------------------------------------------------------------------
// ピクセルシェーダーのエントリ関数。
//--------------------------------------------------------------------------------------
float4 PSMain( PSIn In ) : SV_Target
{
	//ディフューズカラーを取得。
	float4 color = diffuseTexture.Sample(Sampler, In.tex); 
	//ディフューズライトを計算。
	float3 normal = normalTexture.Sample(Sampler, In.tex).xyz;
	float3 lig = 0.0f;
	lig = max(0.0f, -dot(normal, diffuseLightDir.xyz)) * diffuseLightColor.xyz;
	color.xyz *= lig;
    return color; 
}
