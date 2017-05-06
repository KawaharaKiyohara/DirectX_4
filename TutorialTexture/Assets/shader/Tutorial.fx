/*!
 * @brief	�V���v���ȃe�N�X�`���\��t���V�F�[�_�[�B
 */

//�萔�o�b�t�@�B
cbuffer c0 : register(b0){
	float4	color;		//�J���[�B
} ;

/*!
 * @brief	���_�V�F�[�_�[�ւ̓��́B
 */
struct VSInput{
	float4 pos : POSITION;
	float2 uv : TEXCOORD0;
};

/*!
 * @brief	���_�V�F�[�_�[����̏o�́B
 */
struct VSOutput{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

//�e�N�X�`���B
Texture2D texDiffuse : register( t0 );
//�T���v���X�e�[�g�B
SamplerState samLinear : register( s0 );

//--------------------------------------------------------------------------------------
// ���_�V�F�[�_�[�̃G���g���[�֐��B
//--------------------------------------------------------------------------------------
VSOutput VS( VSInput In ) 
{
	VSOutput Out = (VSOutput)0;
	Out.pos = In.pos;
	Out.uv = In.uv;
    return Out;
}


//--------------------------------------------------------------------------------------
// �s�N�Z���V�F�[�_�[�̃G���g���֐��B
//--------------------------------------------------------------------------------------
float4 PS( VSOutput In ) : SV_Target
{
    return texDiffuse.Sample( samLinear, In.uv );
}
