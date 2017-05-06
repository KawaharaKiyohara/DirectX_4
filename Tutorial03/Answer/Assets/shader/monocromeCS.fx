/*!
 * @brief	モノクロ化コンピュートシェーダー。
 */



//入力テクスチャ。
Texture2D inTexture : register( t0 );
//出力テクスチャ。
RWStructuredBuffer<uint>	outputBuffer : register(u0);

/*!
 * @brief	float4をRGBA32フォーマットに変換。
 */
uint PackedFloat4ToRGBA32( float4 In )
{
	return (uint)(In.a * 255) << 24 
			| (uint)(In.b * 255) << 16 
			| (uint)(In.g * 255) << 8 
			| (uint)(In.r * 255);
}
/*!
 * @brief	メイン関数。
 */
[numthreads(16, 16, 1)]
void CSMain( uint3 DTid : SV_DispatchThreadID)
{
	float4 color = inTexture.Load(uint3( DTid.x, DTid.y, 0));
//	for(int i = 0; i < 1000; i++){
	float Cb = -0.2f;
	float Cr = 0.1f;
	float Y = 0.299f * color.r + 0.587f * color.g + 0.114f * color.b;
	color.r = Y + 1.402f * Cr;
	color.g = Y - 0.34414f * Cb - 0.71414f * Cr;
	color.b = Y + 1.772f * Cb;
	color.a = 1.0f;
//	}
	//float4をRGBA32フォーマットに変換。
	//画像の縦幅は512で固定。
	outputBuffer[DTid.x + DTid.y * 512] = PackedFloat4ToRGBA32(color);
}
