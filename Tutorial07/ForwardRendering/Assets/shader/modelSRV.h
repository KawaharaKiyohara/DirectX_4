/*!
 * @brief	���f����SRV�B
 */

Texture2D<float4> Texture : register(t0);
sampler Sampler : register(s0);

//�|�C���g���C�g�̃��X�g�B
StructuredBuffer<SPointLight> pointLightList : register(t1);
