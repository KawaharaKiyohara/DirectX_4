/*!
 * @brief	ベーシックコンピュートシェーダー。
 */

#define NUM_STUDENT 30
/*!
 * @brief	入力構造体。
 */
struct SInputParam
{
    int score;

};
/*!
 * @brief	出力構造体。
 */
struct SOutputParam
{
	int avarage;
};
//入力バッファ。
StructuredBuffer<SInputParam> InputBuffer : register(t0);
//出力バッファ。
RWStructuredBuffer<SOutputParam> OutBuffer : register(u0);

[numthreads(1, 1, 1)]
void CSMain( uint3 DTid : SV_DispatchThreadID )
{
	//平均点を計算する。
	int totalScore = 0;
	for(int i = 0; i < NUM_STUDENT; i++ ){
		totalScore += InputBuffer[i].score;
	}
	OutBuffer[0].avarage = totalScore / NUM_STUDENT;
}


