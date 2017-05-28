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
	float2 pos2 : TEXCOORD1;
};
/*!
 * @brief	ライト用の定数バッファ。
 */
cbuffer cbLight : register(b0){
	float4 diffuseLightDir;			//ディフューズライトの方向。
	float4 diffuseLightColor;		//ディフューズライトのカラー。
	float4 ambientLight;			//アンビエントライト。
	int numPointLight;				//ポイントライトの数。
};

//ポイントライト。
struct SPointLight {
	float4	position;		//位置。
	float4	color;			//カラー。
	float4  attn;			//減衰定数。(xを小さくするとライトの明るさが増す、yとzが大きくなるとライトが遠くまで届くようになる。)
};
	
/*!
 * @brief	行列関係の定数バッファ。
 */
cbuffer cbMatrix : register(b1){
	float4x4 mViewProjInv;		//ビュープロジェクション行列の逆行列。
};

//サンプラ。
sampler Sampler : register(s0);

//ディフューズテクスチャ。
Texture2D<float4> diffuseTexture : register(t0);
//法線テクスチャ。
Texture2D<float4> normalTexture : register(t1);


/*!
 * @brief	スクリーン座標からワールド座標を計算する。
 *@param[in]	screenPos		スクリーン座標(正規化座標系でのxyz座標)。
 *@param[in]	mViewProjInv	ビュープロジェクション行列の逆行列。
 */
float3 ScreenPosToWorldPos(float3 screenPos, float4x4 mViewProjInv)
{
	float4 worldPos = mul(mViewProjInv, float4(screenPos, 1.0f));
	worldPos.xyz /= worldPos.w;
	return worldPos.xyz;
}
//--------------------------------------------------------------------------------------
// 頂点シェーダーのエントリ関数。
//--------------------------------------------------------------------------------------
PSIn VSMain(VSIn In) 
{
	PSIn psIn;
	psIn.pos = In.pos;
	psIn.tex = In.tex;
	psIn.pos2 = In.pos.xy;
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
	//アンビエントライトを加算。
	lig += ambientLight.xyz;
	
	color.xyz *= lig;
    return color;
}
