/*!
 *@brief	テクスチャコンバート
 */
#include "stdafx.h"
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"

using namespace tkEngine2;

class SkinModelTest : public IGameObject {
	CSkinModelData skinModelData;
	CSkinModel skinModel;
public:
	bool Start() override
	{
		skinModelData.Load(L"Resources/modelData/Thethief_H.cmo");
		return true;
	}
	void Update() override
	{
	}
	void Render(CRenderContext& rc)
	{
		DirectX::CommonStates state(Engine().GetD3DDevice());
		state.DepthDefault();
		DirectX::XMMATRIX worldMat, viewMat, projMat;
		DirectX::XMVECTORF32 eyePos, targetPos, up;
		eyePos.f[0] = 0.0f;
		eyePos.f[1] = 20.0f;
		eyePos.f[2] = 30.0f;
		eyePos.f[3] = 1.0f;
		targetPos.f[0] = 0.0f;
		targetPos.f[1] = 20.0f;
		targetPos.f[2] = 0.0f;
		targetPos.f[3] = 0.0f;
		up.f[0] = 0.0f;
		up.f[1] = 1.0f;
		up.f[2] = 0.0f;
		up.f[3] = 0.0f;
		viewMat = DirectX::XMMatrixLookAtLH(eyePos, targetPos, up);

		projMat = DirectX::XMMatrixPerspectiveFovLH(3.14 * 0.3f, (float)Engine().GetFrameBufferWidth()/(float)Engine().GetFrameBufferHeight(), 0.1f, 10000.0f);

		worldMat = DirectX::XMMatrixIdentity();
		skinModelData.GetBody().Draw(
			Engine().GetD3DDeviceContext(),
			state,
			worldMat,
			viewMat,
			projMat);

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

