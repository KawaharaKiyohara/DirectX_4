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
	int highScore;
	int minScore;
};
//入力バッファ。
StructuredBuffer<SInputParam> InputBuffer : register(t0);
//出力バッファ。
RWStructuredBuffer<SOutputParam> OutBuffer : register(u0);

[numthreads(1, 1, 1)]
void CSMain( uint3 DTid : SV_DispatchThreadID )
{
	int totalScore = 0;
	int highScore = 0;
	int minScore = 100;
	for(int i = 0; i < NUM_STUDENT; i++ ){
		totalScore += InputBuffer[i].score;
		highScore = max(highScore, InputBuffer[i].score);
		minScore = min(minScore, InputBuffer[i].score);
	}
	OutBuffer[0].avarage = totalScore / NUM_STUDENT;
	OutBuffer[0].highScore = highScore;
	OutBuffer[0].minScore = minScore;
}


