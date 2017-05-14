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
	int hensa;
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
	int avarage = 0;
	for(int i = 0; i < NUM_STUDENT; i++ ){
		totalScore += InputBuffer[i].score;
	}
	avarage = totalScore / NUM_STUDENT;
	//標準偏差を計算する。
	int hensa = 0;
	for(int i = 0; i < NUM_STUDENT; i++){
		hensa += (InputBuffer[i].score - avarage) * (InputBuffer[i].score - avarage);
	}
	hensa = sqrt(hensa/NUM_STUDENT);
	OutBuffer[0].hensa = hensa;
}


