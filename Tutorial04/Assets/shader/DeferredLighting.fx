/*!
 * @brief	�f�B�t�@�[�h���C�e�B���O�B�B
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
// ���_�V�F�[�_�[�̃G���g���֐��B
//--------------------------------------------------------------------------------------
PSIn VSMain(VSIn In) 
{
	PSIn psIn;
	psIn.pos = In.pos;
	psIn.tex = In.tex;
	return psIn;
}
//--------------------------------------------------------------------------------------
// �s�N�Z���V�F�[�_�[�̃G���g���֐��B
//--------------------------------------------------------------------------------------
float4 PSMain( PSIn In ) : SV_Target
{
	//�f�B�t���[�Y�J���[���擾�B
	float4 color = diffuseTexture.Sample(Sampler, In.tex); 
    return color; 
}
