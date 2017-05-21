/*!
 * @brief	ガウスフィルター
 */



//定数バッファ
cbuffer cb_0 : register(b0) {
	float4 weights[2];
	
};
//テクスチャ情報用の定数バッファ。
cbuffer texInfoCB : register(b1){
	float2 texSize;			//オリジナルテクスチャのサイズ
	float2 xBlurTexSize;	//Xブラーの出力先のテクスチャのサイズ。
	float2 yBlurTexSize;	//Yブラーの出力先のテクスチャのサイズ。
};
//入力テクスチャ。
Texture2D inTexture : register( t0 );
//Xブラーの出力先。
RWStructuredBuffer<uint>	xBlurTexture : register(u0);
//Yブラーの出力先。
RWStructuredBuffer<uint>	yBlurTexture : register(u1);
//最終結果の出力先。
RWStructuredBuffer<uint>	finalTexture : register(u2);
/////////////////////////////////////////////////////////////
/*!
 * @brief	RGBA32フォーマットをfloat4に変換する。
 */
/////////////////////////////////////////////////////////////
float4 UnpackedRGBA32ToFloat4( uint In )
{
	float4 ret = 0.0f;
	ret.r = (In & 0x000000FF) / 255.0f;
	ret.g = ((In >> 8 ) & 0x000000FF) / 255.0f;
	ret.b = ((In >> 16 ) & 0x000000FF) / 255.0f;
	ret.a = ((In >> 24 ) & 0x000000FF) / 255.0f;
	return ret;
}
/////////////////////////////////////////////////////////////
/*!
 * @brief	float4をRGBA32フォーマットに変換。
 */
/////////////////////////////////////////////////////////////
uint PackedFloat4ToRGBA32( float4 In )
{
	return (uint)(In.a * 255) << 24 
			| (uint)(In.b * 255) << 16 
			| (uint)(In.g * 255) << 8 
			| (uint)(In.r * 255);
}
/////////////////////////////////////////////////////////////
/*!
 * @brief	XBlurをかけたテクスチャからカラーを取ってくる。
 */
/////////////////////////////////////////////////////////////
float4 GetFloat4ColorFromXBlur( uint3 texPos )
{
	return UnpackedRGBA32ToFloat4(xBlurTexture[texPos.x + texPos.y * xBlurTexSize.x]);
}
/////////////////////////////////////////////////////////////
/*!
 * @brief	YBlurをかけたテクスチャからカラーを取ってくる。
 */
/////////////////////////////////////////////////////////////
float4 GetFloat4ColorFromYBlur( uint3 texPos )
{
	return UnpackedRGBA32ToFloat4(yBlurTexture[texPos.x + texPos.y * yBlurTexSize.x]);
}

/////////////////////////////////////////////////////////////
/*!
 * @brief	XBlurのメイン関数。
 */
/////////////////////////////////////////////////////////////
[numthreads(4, 4, 1)]
void XBlur( uint3 DTid : SV_DispatchThreadID)
{
	float4 color;
	uint3 baseTexelPos = uint3( DTid.x * 2, DTid.y, 0);
	
	//横ブラーをかけていく。
	color =  inTexture.Load(baseTexelPos) * weights[0].x; 
	color += inTexture.Load(baseTexelPos + uint3(1, 0, 0)) * weights[0].y; 
	color += inTexture.Load(baseTexelPos + uint3(2, 0, 0)) * weights[0].z; 
	color += inTexture.Load(baseTexelPos + uint3(3, 0, 0)) * weights[0].w; 
	color += inTexture.Load(baseTexelPos + uint3(4, 0, 0)) * weights[1].x; 
	color += inTexture.Load(baseTexelPos + uint3(5, 0, 0)) * weights[1].y; 
	color += inTexture.Load(baseTexelPos + uint3(6, 0, 0)) * weights[1].z; 
	color += inTexture.Load(baseTexelPos + uint3(7, 0, 0)) * weights[1].w; 
	
	color += inTexture.Load(baseTexelPos - uint3(1, 0, 0)) * weights[0].y; 
	color += inTexture.Load(baseTexelPos - uint3(2, 0, 0)) * weights[0].z; 
	color += inTexture.Load(baseTexelPos - uint3(3, 0, 0)) * weights[0].w; 
	color += inTexture.Load(baseTexelPos - uint3(4, 0, 0)) * weights[1].x; 
	color += inTexture.Load(baseTexelPos - uint3(5, 0, 0)) * weights[1].y; 
	color += inTexture.Load(baseTexelPos - uint3(6, 0, 0)) * weights[1].z; 
	color += inTexture.Load(baseTexelPos - uint3(7, 0, 0)) * weights[1].w; 
	
	//float4をRGBA32フォーマットに変換。
	xBlurTexture[DTid.x + DTid.y * xBlurTexSize.x] = PackedFloat4ToRGBA32(color);
}

/////////////////////////////////////////////////////////////
/*!
 * @brief	YBlurメイン処理。
 */
/////////////////////////////////////////////////////////////
[numthreads(4, 4, 1)]
void YBlur( uint3 DTid : SV_DispatchThreadID)
{
	uint3 texelPos = uint3(DTid.x, DTid.y * 2, 0);
	
	//縦ブラーをかけていく。
	float4 color =  GetFloat4ColorFromXBlur(texelPos + uint3(0, 0, 0)) * weights[0].x; 
	color += GetFloat4ColorFromXBlur(texelPos + uint3(0, 1, 0)) * weights[0].y; 
	color += GetFloat4ColorFromXBlur(texelPos + uint3(0, 2, 0)) * weights[0].z; 
	color += GetFloat4ColorFromXBlur(texelPos + uint3(0, 3, 0)) * weights[0].w; 
	color += GetFloat4ColorFromXBlur(texelPos + uint3(0, 4, 0)) * weights[1].x; 
	color += GetFloat4ColorFromXBlur(texelPos + uint3(0, 5, 0)) * weights[1].y; 
	color += GetFloat4ColorFromXBlur(texelPos + uint3(0, 6, 0)) * weights[1].z; 
	color += GetFloat4ColorFromXBlur(texelPos + uint3(0, 7, 0)) * weights[1].w; 
	
	color += GetFloat4ColorFromXBlur(texelPos - uint3(0, 1, 0)) * weights[0].y; 
	color += GetFloat4ColorFromXBlur(texelPos - uint3(0, 2, 0)) * weights[0].z; 
	color += GetFloat4ColorFromXBlur(texelPos - uint3(0, 3, 0)) * weights[0].w; 
	color += GetFloat4ColorFromXBlur(texelPos - uint3(0, 4, 0)) * weights[1].x; 
	color += GetFloat4ColorFromXBlur(texelPos - uint3(0, 5, 0)) * weights[1].y; 
	color += GetFloat4ColorFromXBlur(texelPos - uint3(0, 6, 0)) * weights[1].z; 
	color += GetFloat4ColorFromXBlur(texelPos - uint3(0, 7, 0)) * weights[1].w; 
	
	//float4をRGBA32フォーマットに変換。
	yBlurTexture[DTid.x + DTid.y * yBlurTexSize.x] = PackedFloat4ToRGBA32(color);
}
/////////////////////////////////////////////////////////////
/*!
 * @brief	最終合成。
 */
/////////////////////////////////////////////////////////////
[numthreads(4, 4, 1)]
void Final( uint3 DTid : SV_DispatchThreadID)
{
	uint3 texelPos = uint3(DTid.x/2, DTid.y/2, 0);
	float4 color =  GetFloat4ColorFromYBlur(texelPos); 
	finalTexture[DTid.x + DTid.y * texSize.x] = PackedFloat4ToRGBA32(color);
}