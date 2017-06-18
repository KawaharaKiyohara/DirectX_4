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
	float2 offset = float2( 2.0f/screenParam.z, 2.0f / screenParam.w);
	
	float depth_0 = depthTexture.Sample(Sampler, screenPos).x;

	float2 screenPos2;
	screenPos2 = screenPos;
	screenPos2.x = screenPos.x + offset.x;
	float depth_1 = depthTexture.Sample(Sampler, screenPos2).x;
	if(abs(depth_0 - depth_1) > 0.2f ){
		//�G�b�W�̐F��Ԃ��B
		return 0.0f;
	}
	screenPos2 = screenPos;
	screenPos2.x = screenPos.x - offset.x;
	depth_1 = depthTexture.Sample(Sampler, screenPos2).x;
	if(abs(depth_0 - depth_1) > 0.2f ){
		//�G�b�W�̐F��Ԃ��B
		return 0.0f;
	}
	screenPos2 = screenPos;
	screenPos2.y = screenPos.y + offset.y;
	depth_1 = depthTexture.Sample(Sampler, screenPos2).x;
	if(abs(depth_0 - depth_1) > 0.2f ){
		//�G�b�W�̐F��Ԃ��B
		return 0.0f;
	}
	screenPos2 = screenPos;
	screenPos2.y = screenPos.y - offset.y;
	depth_1 = depthTexture.Sample(Sampler, screenPos2).x;
	if(abs(depth_0 - depth_1) > 0.2f ){
		//�G�b�W�̐F��Ԃ��B
		return 0.0f;
	}
	
	float4 color = float4(lightTexture.Sample(Sampler, In.TexCoord).xyz, 1.0f);
    return color; 
}

