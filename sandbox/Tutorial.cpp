/*!
 *@brief	テクスチャコンバート
 */
#include "stdafx.h"
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/graphics/tkCamera.h"

using namespace tkEngine2;

class SkinModelTest : public IGameObject {
	CSkinModelData skinModelData;
	CSkinModel skinModel;
	CCamera camera;
public:
	bool Start() override
	{
		skinModelData.Load(L"Resources/modelData/Thethief_H.cmo");
		skinModel.Init(skinModelData);
		//カメラを初期化。
		camera.SetPosition({ 0.0f, 20.0f, 30.0f });
		camera.SetTarget({ 0.0f, 20.0f, 0.0f });
		camera.SetUp({ 0.0f, 1.0f, 0.0f });
		camera.Update();
		return true;
	}
	void Update() override
	{
		skinModel.Update(CVector3::Zero, CQuaternion::Identity, CVector3::One);
	}
	void Render(CRenderContext& rc)
	{
		skinModel.Draw(rc, camera.GetViewMatrix(), camera.GetProjectionMatrix());
	}
};

/*!
 *@brief	メイン関数。
 */
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );
    //tkEngine2の初期化パラメータを設定する。
	SInitParam initParam;
	initParam.nCmdShow = nCmdShow;
	initParam.hInstance = hInstance;
	initParam.screenWidth = 1280;
	initParam.screenHeight = 720;
	initParam.frameBufferWidth = 1280;
	initParam.frameBufferHeight = 720;
	//エンジンを初期化。
	if (Engine().Init(initParam) == true) {
		//初期化に成功。
		NewGO<SkinModelTest>(0);
		//ゲームループを実行。
		Engine().RunGameLoop();
	}
	//エンジンの終了処理。
	Engine().Final();
	
    return 0;
}

