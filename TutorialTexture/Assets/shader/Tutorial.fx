/*!
 * @brief	シンプルなテクスチャ貼り付けシェーダー。
 */

//定数バッファ。
cbuffer c0 : register(b0){
	float4	color;		//カラー。
} ;

/*!
 * @brief	頂点シェーダーへの入力。
 */
struct VSInput{
	float4 pos : POSITION;
	float2 uv : TEXCOORD0;
};

/*!
 * @brief	頂点シェーダーからの出力。
 */
struct VSOutput{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

//テクスチャ。
Texture2D texDiffuse : register( t0 );
//サンプラステート。
SamplerState samLinear : register( s0 );

//--------------------------------------------------------------------------------------
// 頂点シェーダーのエントリー関数。
//--------------------------------------------------------------------------------------
VSOutput VS( VSInput In ) 
{
	VSOutput Out = (VSOutput)0;
	Out.pos = In.pos;
	Out.uv = In.uv;
    return Out;
}


//--------------------------------------------------------------------------------------
// ピクセルシェーダーのエントリ関数。
//--------------------------------------------------------------------------------------
float4 PS( VSOutput In ) : SV_Target
{
    return texDiffuse.Sample( samLinear, In.uv );
}
