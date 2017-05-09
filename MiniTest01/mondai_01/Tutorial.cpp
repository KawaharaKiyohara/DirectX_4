/*!
 *@brief	���e�X�g01_00
 */
#include "stdafx.h"
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/timer/tkStopwatch.h"
#include <time.h>

using namespace tkEngine2;

class Mondai_01 : public IGameObject {
	//���̓p�����[�^�B
	struct SInputParam {
		int score;			//!<���_�B
	};
	static const int NUM_STUDENT = 30;		//���k���B
public:
	Mondai_01()
	{

	}
	~Mondai_01()
	{

	}
	//�����������B
	bool Start() override
	{
		//����N���X�̓_���������_���Ɍ���B
		SInputParam score[NUM_STUDENT];
		for (int i = 0; i < NUM_STUDENT; i++) {
			score[i].score = rand() % 100;
			TK_LOG("Score = %d\n", score[i].score);
		}
		return true;
	}
	void Update() override
	{
		//���������Ȃ��Ă�����B
	}
	void Render(CRenderContext& renderContext)
	{
		//�����ŃR���s���[�g�V�F�[�_�[���f�B�X�p�b�`���āA���ʂ����b�Z�[�W�{�b�N�X�ŏo���v���O�������L�q����B�B
		
		DeleteGO(this);	//�I���B
	}
};
/*!
 *@brief	���C���֐��B
 */
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );
    //tkEngine2�̏������p�����[�^��ݒ肷��B
	SInitParam initParam;
	initParam.nCmdShow = nCmdShow;
	initParam.hInstance = hInstance;
	initParam.screenWidth = 1280;
	initParam.screenHeight = 720;
	initParam.frameBufferWidth = 1280;
	initParam.frameBufferHeight = 720;
	srand(time(NULL));
	//�G���W�����������B
	if (Engine().Init(initParam) == true) {
		NewGO<Mondai_01>(0);
		//�������ɐ����B
		//�Q�[�����[�v�����s�B
		Engine().RunGameLoop();
	}
	//�G���W���̏I�������B
	Engine().Final();
	
    return 0;
}

