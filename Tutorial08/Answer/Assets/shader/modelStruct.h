/*!
 * @brief	モデル関係の構造体ヘッダ。
 */
 
/*!
 * @brief	スキンなしモデルの頂点構造体。
 */
struct VSInputNmTxVcTangent
{
    float4 Position : SV_Position;
    float3 Normal   : NORMAL;
    float4 Tangent  : TANGENT;
    float2 TexCoord : TEXCOORD0;
    float4 Color    : COLOR;
};
/*!
 * @brief	スキンありモデルの頂点構造体。
 */
struct VSInputNmTxWeights
{
    float4 Position : SV_Position;
    float3 Normal   : NORMAL;
    float2 TexCoord	: TEXCOORD0;
    uint4  Indices  : BLENDINDICES0;
    float4 Weights  : BLENDWEIGHT0;
};

/*!
 * @brief	ピクセルシェーダーの入力。
 */
struct PSInput{
	float4 Position : SV_POSITION;
	float3 Normal	: NORMAL;
	float2 TexCoord : TEXCOORD0;
	float4 Pos			: TEXCOORD1;
	float3 posInView	: TEXCOORD2;		//カメラ空間での座標。
};
/*!
 * @brief	ピクセルシェーダーの出力。
 */
struct PSOutput{
	float4 diffuse : SV_Target0;
	float4 normal : SV_Target1;
	float depth : SV_Target2;
};

/*!
 * @brief	ZPrepass用のピクセルシェーダーの入力。
 */
struct PSInputZPrepass{
	float4 Position : SV_POSITION;
};

//ポイントライト。
struct SPointLight {
	float3	position;		//位置。
	float3	positionInView;	//カメラ座標系でのポイントライトの位置。
	float4	color;			//カラー。
	float4  attn;			//減衰定数。(xを小さくするとライトの明るさが増す、yとzが大きくなるとライトが遠くまで届くようになる。)
};
