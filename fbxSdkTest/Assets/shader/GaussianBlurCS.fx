/*!
 * @brief	�K�E�X�t�B���^�[
 */



//�萔�o�b�t�@
cbuffer cb_0 : register(b0) {
	float4 weights[2];
	
};
//�e�N�X�`�����p�̒萔�o�b�t�@�B
cbuffer texInfoCB : register(b1){
	float2 texSize;			//�I���W�i���e�N�X�`���̃T�C�Y
	float2 xBlurTexSize;	//X�u���[�̏o�͐�̃e�N�X�`���̃T�C�Y�B
	float2 yBlurTexSize;	//Y�u���[�̏o�͐�̃e�N�X�`���̃T�C�Y�B
};
//���̓e�N�X�`���B
Texture2D inTexture : register( t0 );
//X�u���[�̏o�͐�B
RWStructuredBuffer<uint>	xBlurTexture : register(u0);
//Y�u���[�̏o�͐�B
RWStructuredBuffer<uint>	yBlurTexture : register(u1);
//�ŏI���ʂ̏o�͐�B
RWStructuredBuffer<uint>	finalTexture : register(u2);
/////////////////////////////////////////////////////////////
/*!
 * @brief	RGBA32�t�H�[�}�b�g��float4�ɕϊ�����B
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
 * @brief	float4��RGBA32�t�H�[�}�b�g�ɕϊ��B
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
 * @brief	XBlur���������e�N�X�`������J���[������Ă���B
 */
/////////////////////////////////////////////////////////////
float4 GetFloat4ColorFromXBlur( uint3 texPos )
{
	return UnpackedRGBA32ToFloat4(xBlurTexture[texPos.x + texPos.y * xBlurTexSize.x]);
}
/////////////////////////////////////////////////////////////
/*!
 * @brief	YBlur���������e�N�X�`������J���[������Ă���B
 */
/////////////////////////////////////////////////////////////
float4 GetFloat4ColorFromYBlur( uint3 texPos )
{
	return UnpackedRGBA32ToFloat4(yBlurTexture[texPos.x + texPos.y * yBlurTexSize.x]);
}

/////////////////////////////////////////////////////////////
/*!
 * @brief	XBlur�̃��C���֐��B
 */
/////////////////////////////////////////////////////////////
[numthreads(4, 4, 1)]
void XBlur( uint3 DTid : SV_DispatchThreadID)
{
	float4 color;
	uint3 baseTexelPos = uint3( DTid.x * 2, DTid.y, 0);
	
	//���u���[�������Ă����B
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
	
	//float4��RGBA32�t�H�[�}�b�g�ɕϊ��B
	xBlurTexture[DTid.x + DTid.y * xBlurTexSize.x] = PackedFloat4ToRGBA32(color);
}

/////////////////////////////////////////////////////////////
/*!
 * @brief	YBlur���C�������B
 */
/////////////////////////////////////////////////////////////
[numthreads(4, 4, 1)]
void YBlur( uint3 DTid : SV_DispatchThreadID)
{
	uint3 texelPos = uint3(DTid.x, DTid.y * 2, 0);
	
	//�c�u���[�������Ă����B
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
	
	//float4��RGBA32�t�H�[�}�b�g�ɕϊ��B
	yBlurTexture[DTid.x + DTid.y * yBlurTexSize.x] = PackedFloat4ToRGBA32(color);
}
/////////////////////////////////////////////////////////////
/*!
 * @brief	�ŏI�����B
 */
/////////////////////////////////////////////////////////////
[numthreads(4, 4, 1)]
void Final( uint3 DTid : SV_DispatchThreadID)
{
	uint3 texelPos = uint3(DTid.x/2, DTid.y/2, 0);
	float4 color =  GetFloat4ColorFromYBlur(texelPos); 
	finalTexture[DTid.x + DTid.y * texSize.x] = PackedFloat4ToRGBA32(color);
}