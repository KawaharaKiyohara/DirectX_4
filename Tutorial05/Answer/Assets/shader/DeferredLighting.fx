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
	float2 pos2 : TEXCOORD1;
};
/*!
 * @brief	���C�g�p�̒萔�o�b�t�@�B
 */
cbuffer cbLight : register(b0){
	float4 diffuseLightDir;			//�f�B�t���[�Y���C�g�̕����B
	float4 diffuseLightColor;		//�f�B�t���[�Y���C�g�̃J���[�B
	float4 ambientLight;			//�A���r�G���g���C�g�B
	int numPointLight;				//�|�C���g���C�g�̐��B
};

//�|�C���g���C�g�B
struct SPointLight {
	float4	position;		//�ʒu�B
	float4	color;			//�J���[�B
	float4  attn;			//�����萔�B(x������������ƃ��C�g�̖��邳�������Ay��z���傫���Ȃ�ƃ��C�g�������܂œ͂��悤�ɂȂ�B)
};
	
/*!
 * @brief	�s��֌W�̒萔�o�b�t�@�B
 */
cbuffer cbMatrix : register(b1){
	float4x4 mViewProjInv;		//�r���[�v���W�F�N�V�����s��̋t�s��B
};

//�T���v���B
sampler Sampler : register(s0);

//�f�B�t���[�Y�e�N�X�`���B
Texture2D<float4> diffuseTexture : register(t0);
//�@���e�N�X�`���B
Texture2D<float4> normalTexture : register(t1);
//�[�x�e�N�X�`���B
Texture2D<float4> depthTexture : register(t2);
//�|�C���g���C�g�B
StructuredBuffer<SPointLight> pointLightList : register(t3);

/*!
 * @brief	�X�N���[�����W���烏�[���h���W���v�Z����B
 *@param[in]	screenPos		�X�N���[�����W(���K�����W�n�ł�xyz���W)�B
 *@param[in]	mViewProjInv	�r���[�v���W�F�N�V�����s��̋t�s��B
 */
float3 ScreenPosToWorldPos(float3 screenPos, float4x4 mViewProjInv)
{
	float4 worldPos = mul(mViewProjInv, float4(screenPos, 1.0f));
	worldPos.xyz /= worldPos.w;
	return worldPos.xyz;
}
//--------------------------------------------------------------------------------------
// ���_�V�F�[�_�[�̃G���g���֐��B
//--------------------------------------------------------------------------------------
PSIn VSMain(VSIn In) 
{
	PSIn psIn;
	psIn.pos = In.pos;
	psIn.tex = In.tex;
	psIn.pos2 = In.pos.xy;
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
	//�A���r�G���g���C�g�����Z�B
	lig += ambientLight.xyz;
	
	//�|�C���g���C�g���v�Z����B
	//�s�N�Z�����W�Ɛ[�x�l���烏�[���h���W�����߂�B
	float3 screenPos;
	screenPos.xy = (In.tex * float2(2.0f, -2.0f)) + float2( -1.0f, 1.0f);
	screenPos.z = depthTexture.Sample(Sampler, In.tex).r;
	float3 worldPos = ScreenPosToWorldPos(screenPos, mViewProjInv);

	for(int i = 0; i < numPointLight; i++){
		float3 lightDir = worldPos - pointLightList[i].position.xyz;
		float len = length(lightDir);
		lightDir = normalize(lightDir);	//���K���B
		float3 pointLightColor = saturate(-dot(normal, lightDir)) * pointLightList[i].color.xyz;
		//�������v�Z����B
		float3 attn = pointLightList[i].attn.xyz;
		pointLightColor /= (attn.x + attn.y * len + attn.z * len * len);
		lig += pointLightColor;
	
	}
	color.xyz *= lig;
    return color;
}
