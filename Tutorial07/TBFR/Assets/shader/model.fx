/*!
 * @brief	���f���V�F�[�_�[�B
 */

#include "modelCB.h"
#include "modelStruct.h"
#include "modelSRV.h"

#define TILE_WIDTH	16		//�^�C���̕��B


/*!--------------------------------------------------------------------------------------
 * @brief	�X�L���Ȃ����f���p�̒��_�V�F�[�_�[�B
-------------------------------------------------------------------------------------- */
PSInput VSMain( VSInputNmTxVcTangent In ) 
{
	PSInput psInput = (PSInput)0;
	float4 pos;
	pos = mul(mWorld, In.Position);
	psInput.wPos = pos.xyz;
	pos = mul(mView, pos);
	pos = mul(mProj, pos);
	psInput.Position = pos;
	psInput.TexCoord = In.TexCoord;
	psInput.Normal = In.Normal;
	psInput.Pos = pos;
    return psInput;
}
/*!--------------------------------------------------------------------------------------
 * @brief	�X�L�����胂�f���p�̒��_�V�F�[�_�[�B
-------------------------------------------------------------------------------------- */
PSInput VSMainSkin( VSInputNmTxWeights In ) 
{
	PSInput psInput = (PSInput)0;
	float4 pos;
	pos = mul(mWorld, In.Position);
	psInput.wPos = pos.xyz;
	pos = mul(mView, pos);
	pos = mul(mProj, pos);
	psInput.Position = pos;
	psInput.TexCoord = In.TexCoord;
	psInput.Normal = In.Normal;
	psInput.Pos = pos;
    return psInput;
}
//--------------------------------------------------------------------------------------
// �s�N�Z���V�F�[�_�[�̃G���g���֐��B
//--------------------------------------------------------------------------------------
float4 PSMain( PSInput In ) : SV_Target0
{
	if(isZPrepass){
		//ZPrepass
		return In.Pos.z / In.Pos.w;
	}
	//�X�N���[���̍����(0,0)�A�E����(1,1)�Ƃ�����W�n�ɕϊ�����B
	float2 screenPos = (In.Pos.xy / In.Pos.w) * float2(0.5f, -0.5f) + 0.5f;
	//�r���[�|�[�g���W�n�ɕϊ�����B
	float2 viewportPos = screenParam.zw * screenPos;
	//�X�N���[�����^�C���ŕ��������Ƃ��̃Z����X���W�����߂�B
	uint numCellX = (screenParam.z + TILE_WIDTH - 1) / TILE_WIDTH;
	//�^�C���C���f�b�N�X���v�Z����B
	uint tileIndex = floor( viewportPos.x / TILE_WIDTH ) + floor( viewportPos.y / TILE_WIDTH ) * numCellX;
	
	//�|�C���g���C�g�̐����擾�B
	uint numLights, dummy;
	pointLightList.GetDimensions(numLights, dummy);
	//���̃s�N�Z�����܂܂��^�C���̃��C�g�C���f�b�N�X���X�g�̊J�n�ʒu���v�Z����B
	uint lightStart = tileIndex * numLights;
	//���̃s�N�Z�����܂܂��^�C���̃��C�g�C���f�b�N�X���X�g�̏I���ʒu���v�Z����B
	uint lightEnd = lightStart + numLights;
	
	float3 lig = 0.0f;
	for (uint lightListIndex = lightStart; lightListIndex < lightEnd; lightListIndex++){
		uint lightIndex = pointLightListInTile[lightListIndex];
		if(lightIndex == 0xffffffff){
			//���̃^�C���Ɋ܂܂��|�C���g���C�g�͂����Ȃ��B
			break;
		}
		SPointLight light = pointLightList[lightIndex];
		float3 lightDir = In.wPos - light.position;
		float len = length(lightDir);
		lightDir = normalize(lightDir);	//���K���B
		float3 pointLightColor = saturate(-dot(In.Normal, lightDir)) * light.color.xyz;
		//�������v�Z����B
		float	litRate = len / light.attn.x;
		float	attn = max(1.0 - litRate * litRate, 0.0);
		pointLightColor *= attn;
		lig += pointLightColor;
	}
	float4 color = float4(Texture.Sample(Sampler, In.TexCoord).xyz, 1.0f);
	color.xyz *= lig;
    return color; 
}

