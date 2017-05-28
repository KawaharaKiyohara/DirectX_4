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
/*!
 * @brief	���C�g�p�̒萔�o�b�t�@�B
 */
cbuffer cbLight : register(b0){
	float4 diffuseLightDir;			//�f�B�t���[�Y���C�g�̕����B
	float4 diffuseLightColor;		//�f�B�t���[�Y���C�g�̃J���[�B
};
//�f�B�t���[�Y�e�N�X�`���B
Texture2D<float4> diffuseTexture : register(t0);
//�@���e�N�X�`���B
Texture2D<float4> normalTexture : register(t1);
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
	//�f�B�t���[�Y���C�g���v�Z�B
	float3 normal = normalTexture.Sample(Sampler, In.tex).xyz;
	float3 lig = 0.0f;
	lig = max(0.0f, -dot(normal, diffuseLightDir.xyz)) * diffuseLightColor.xyz;
	color.xyz *= lig;
    return color; 
}
