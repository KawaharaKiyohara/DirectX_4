/*!
 * @brief	モデルのSRV。
 */

Texture2D<float4> Texture : register(t0);
sampler Sampler : register(s0);

//ポイントライトのリスト。
StructuredBuffer<SPointLight> pointLightList : register(t1);
