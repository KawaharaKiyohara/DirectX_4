/*!
 * @brief	ポリゴン用のシェーダー。
 */


struct VSInput{
	float4 pos : SV_Position;
};

struct PSInput{
	float4 pos : SV_POSITION;
};

PSInput VSMain(VSInput In) 
{
	PSInput psIn;
	psIn.pos = In.pos;
	return psIn;
}
float4 PSMain( PSInput In ) : SV_Target0
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}