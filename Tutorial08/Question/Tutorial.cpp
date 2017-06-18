/*!
 *@brief	�g�D�[���T���v���B
 */
#include "stdafx.h"
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/graphics/tkCamera.h"
#include "tkEngine2/timer/tkStopwatch.h"
#include <time.h>
using namespace tkEngine2;


class ToonRender : public IGameObject {
	CSkinModelData skinModelData;
	CSkinModel uniModel;
	CCamera camera;
	//���_�B
	struct SSimpleVertex {
		CVector4 pos;
		CVector2 tex;
	};
public:
	bool Start() override
	{
		skinModelData.Load(L"Assets/modelData/unityChan.cmo");
		uniModel.Init(skinModelData);
		//�J�������������B
		camera.SetPosition({ 0.0f, 1.0f, 2.0f });
		camera.SetTarget({ 0.0f, 1.0f, 0.0f });
		camera.SetUp({ 0.0f, 1.0f, 0.0f });
		camera.SetNear(1.0f);
		camera.Update();
		
		return true;
	}
	/*!------------------------------------------------------------------
	 * @brief	�X�V�B
	 ------------------------------------------------------------------*/
	void Update() override
	{
		static float angle = 0.0f;
		CQuaternion rot;
		rot.SetRotation(CVector3::AxisY, angle);
		angle += 0.01f;
		uniModel.Update({0.0f, 0.0f, 0.0f}, rot, CVector3::One);
	}
	/*!------------------------------------------------------------------
	* @brief	�V�[���̕`��B
	------------------------------------------------------------------*/
	void RenderScene(CRenderContext& rc)
	{
		//���C�������_�����O�^�[�Q�b�g�ɖ߂��B
		rc.OMSetRenderTargets(1, &Engine().GetMainRenderTarget());
		uniModel.Draw(rc, camera.GetViewMatrix(), camera.GetProjectionMatrix());
	}
	/*!------------------------------------------------------------------
	* @brief	�`��B
	------------------------------------------------------------------*/
	void Render(CRenderContext& rc) override
	{
		//�V�[���̕`��B
		RenderScene(rc);
	}
};

/*!
 *@brief	���C���֐��B
 */
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );
	srand((unsigned)time(NULL));
    //tkEngine2�̏������p�����[�^��ݒ肷��B
	SInitParam initParam;
	initParam.nCmdShow = nCmdShow;
	initParam.hInstance = hInstance;
	initParam.screenWidth = 1280;
	initParam.screenHeight = 720;
	initParam.frameBufferWidth = 1280;
	initParam.frameBufferHeight = 720;
	//�G���W�����������B
	if (Engine().Init(initParam) == true) {
		//�������ɐ����B
		NewGO<ToonRender>(0);
		//�Q�[�����[�v�����s�B
		Engine().RunGameLoop();
	}
	//�G���W���̏I�������B
	Engine().Final();
	
    return 0;
}
