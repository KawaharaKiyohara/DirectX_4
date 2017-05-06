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
[numthreads(2, 2, 1)]
void CSMain( uint3 DTid : SV_DispatchThreadID)
{
	float4 color = inTexture.Load(uint3( DTid.x, DTid.y, 0));
//	for(int i = 0; i < 1000; i++){
		float Y = 0.29900 * color.r + 0.58700 * color.g + 0.11400 * color.b;
		color.r = Y;
		color.g = Y;
		color.b = Y;
		color.a = 1.0f;
//	}
	//float4をRGBA32フォーマットに変換。
	//画像の縦幅は512で固定。
	outputBuffer[DTid.x + DTid.y * 512] = PackedFloat4ToRGBA32(color);
}
