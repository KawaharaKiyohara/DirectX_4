/*!
 * @brief	���f����SRV�B
 */

Texture2D<float4> Texture : register(t0);
sampler Sampler : register(s0);

//�^�C�����Ƃ̃|�C���g���C�g�̃C���f�b�N�X�̃��X�g�B
StructuredBuffer<uint> pointLightListInTile : register(t1);

//�|�C���g���C�g�̃��X�g�B
StructuredBuffer<SPointLight> pointLightList : register(t2);
