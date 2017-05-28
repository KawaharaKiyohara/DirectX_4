/*!
 * @brief	モデルシェーダー。
 */

Texture2D<float4> Texture : register(t0);
sampler Sampler : register(s0);

/*!
 * @brief	著店シェーダー用の定数バッファ。
 */
cbuffer VSCb : register(b0){
	float4x4 mWorld;
	float4x4 mView;
	float4x4 mProj;
};
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
};
/*!
 * @brief	ピクセルシェーダーの出力。
 */
struct PSOutput{
	float4 diffuse : SV_Target0;
	float4 normal : SV_Target1;
};
/*!--------------------------------------------------------------------------------------
 * @brief	スキンなしモデル用の頂点シェーダー。
-------------------------------------------------------------------------------------- */
PSInput VSMain( VSInputNmTxVcTangent In ) 
{
	PSInput psInput = (PSInput)0;
	psInput.Position = mul(mWorld, In.Position);
	psInput.Position = mul(mView, psInput.Position);
	psInput.Position = mul(mProj, psInput.Position);
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
	pos = mul(mProj, pos);
	psInput.Position = pos;
	psInput.TexCoord = In.TexCoord;
	psInput.Normal = In.Normal;
    return psInput;
}
//--------------------------------------------------------------------------------------
// ピクセルシェーダーのエントリ関数。
//--------------------------------------------------------------------------------------
PSOutput PSMain( PSInput In )
{
	PSOutput psOut;
	psOut.diffuse = float4(Texture.Sample(Sampler, In.TexCoord).xyz, 1.0f) ;
	psOut.normal = float4(In.Normal, 1.0f);
    return psOut; 
}
