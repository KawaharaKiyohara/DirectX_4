/*!
 * @brief	�x�[�V�b�N�R���s���[�g�V�F�[�_�[�B
 */

#define NUM_STUDENT 30
/*!
 * @brief	���͍\���́B
 */
struct SInputParam
{
    int score;

};
/*!
 * @brief	�o�͍\���́B
 */
struct SOutputParam
{
	int avarage;
	int highScore;
	int minScore;
};
//���̓o�b�t�@�B
StructuredBuffer<SInputParam> InputBuffer : register(t0);
//�o�̓o�b�t�@�B
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


