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
	
	CRenderTarget depthBuffer;							//�[�x���o�͂��邽�߂̃o�b�t�@�B
	CSkinModelData skinModelData;
	CSkinModel uniModel;
	const int TILE_WIDTH = 16;
	CCamera camera;
	
	//���_�B
	struct SSimpleVertex {
		CVector4 pos;
		CVector2 tex;
	};
public:
	bool Start() override
	{
		//�[�x�l�o�͗p�̃����_�����O�^�[�Q�b�g���쐬����B
		DXGI_SAMPLE_DESC multiSampleDesc;
		multiSampleDesc.Count = 1;
		multiSampleDesc.Quality = 0;
		depthBuffer.Create(
			Engine().GetFrameBufferWidth(),
			Engine().GetFrameBufferHeight(),
			1,
			DXGI_FORMAT_R32_FLOAT,
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			multiSampleDesc
		);

		skinModelData.Load(L"Assets/modelData/unityChan.cmo");
		uniModel.Init(skinModelData);
		//�J�������������B
		camera.SetPosition({ 0.0f, 0.7f, 1.5f });
		camera.SetTarget({ 0.0f, 0.7f, 0.0f });
		camera.SetUp({ 0.0f, 1.0f, 0.0f });
		camera.SetNear(0.1f);
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
		if (GetAsyncKeyState(VK_LEFT)) {

			angle += 0.01f;
		}
		if (GetAsyncKeyState(VK_RIGHT)) {

			angle -= 0.01f;
		}

		uniModel.Update({0.0f, 0.0f, 0.0f}, rot, CVector3::One);
	}
	/*!------------------------------------------------------------------
	* @brief	ZPrepass�B
	* *@details
	*   �[�x�l���������ނ��߂̃p�X�B
	------------------------------------------------------------------*/
	void ZPrepass(CRenderContext& rc)
	{
		rc.OMSetRenderTargets(1, &depthBuffer);
		float ClearColor[4] = { 10000.0f, 10000.0f, 10000.0f, 1.0f }; //red,green,blue,alpha
		rc.ClearRenderTargetView(0, ClearColor);
		uniModel.Draw(rc, camera.GetViewMatrix(), camera.GetProjectionMatrix(), true);
		rc.OMSetRenderTargets(1, NULL);
	}

	/*!------------------------------------------------------------------
	* @brief	�V�[���̕`��B
	------------------------------------------------------------------*/
	void RenderScene(CRenderContext& rc)
	{
		//���C�������_�����O�^�[�Q�b�g�ɖ߂��B
		rc.OMSetRenderTargets(1, &Engine().GetMainRenderTarget());
		//�[�x�e�N�X�`����ݒ�B
		rc.PSSetShaderResource(2, depthBuffer.GetRenderTargetSRV());
		uniModel.Draw(rc, camera.GetViewMatrix(), camera.GetProjectionMatrix());
	}
	/*!------------------------------------------------------------------
	* @brief	�`��B
	------------------------------------------------------------------*/
	void Render(CRenderContext& rc) override
	{
		//�[�x�l�������݂̂��߂�ZPrepass
		ZPrepass(rc);
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

