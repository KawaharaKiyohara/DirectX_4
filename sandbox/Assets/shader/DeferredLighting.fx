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

Texture2D<float4> diffuseTexture : register(t0);
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
    return color; 
}
