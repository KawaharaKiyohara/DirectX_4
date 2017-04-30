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
};
//���̓o�b�t�@�B
StructuredBuffer<SInputParam> InputBuffer : register(t0);
//�o�̓o�b�t�@�B
RWStructuredBuffer<SOutputParam> OutBuffer : register(u0);

[numthreads(1, 1, 1)]
void CSMain( uint3 DTid : SV_DispatchThreadID )
{
	//���ϓ_���v�Z����B
	int totalScore = 0;
	for(int i = 0; i < NUM_STUDENT; i++ ){
		totalScore += InputBuffer[i].score;
	}
	OutBuffer[0].avarage = totalScore / NUM_STUDENT;
}


