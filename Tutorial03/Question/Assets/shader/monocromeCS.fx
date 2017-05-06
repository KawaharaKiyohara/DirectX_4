/*!
 * @brief	���m�N�����R���s���[�g�V�F�[�_�[�B
 */



//���̓e�N�X�`���B
Texture2D inTexture : register( t0 );
//�o�̓e�N�X�`���B
RWStructuredBuffer<uint>	outputBuffer : register(u0);

/*!
 * @brief	float4��RGBA32�t�H�[�}�b�g�ɕϊ��B
 */
uint PackedFloat4ToRGBA32( float4 In )
{
	return (uint)(In.a * 255) << 24 
			| (uint)(In.b * 255) << 16 
			| (uint)(In.g * 255) << 8 
			| (uint)(In.r * 255);
}
/*!
 * @brief	���C���֐��B
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
	//float4��RGBA32�t�H�[�}�b�g�ɕϊ��B
	//�摜�̏c����512�ŌŒ�B
	outputBuffer[DTid.x + DTid.y * 512] = PackedFloat4ToRGBA32(color);
}
