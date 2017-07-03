/*!
 *@brief	トゥーンサンプル。
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
	CRenderTarget depthTexture;	//深度テクスチャ。
	CRenderTarget bokeTexture;	//ボケテクスチャ。
	CRenderTarget blurBokeTexture;	//ブラーをかけたボケテクスチャ。
	CVertexBuffer vertexBuffer;	//頂点バッファ
	CShader vsDofShader;		//被写界深度の頂点シェーダー。
	CShader psDofShader;		//ボケ画像抽出用の被写界深度のピクセルシェーダー。
	CShader psDofShader_1;
	CShader psDofShader_2;		//ボケ画像合成。
	//頂点。
	struct SSimpleVertex {
		CVector4 pos;
		CVector2 tex;
	};
public:
	bool Start() override
	{
		skinModelData.Load(L"Assets/modelData/unityChan.cmo");
		uniModel.Init(skinModelData);
		//カメラを初期化。
		camera.SetPosition({ 0.0f, 1.0f, 2.0f });
		camera.SetTarget({ 0.0f, 1.0f, 0.0f });
		camera.SetUp({ 0.0f, 1.0f, 0.0f });
		camera.SetNear(1.0f);
		camera.Update();
		
		//深度テクスチャ出力用のレンダリングターゲットを作成する。
		DXGI_SAMPLE_DESC multiSampleDesc;
		multiSampleDesc.Count = 1;
		multiSampleDesc.Quality = 0;
		depthTexture.Create(
			Engine().GetFrameBufferWidth(),
			Engine().GetFrameBufferHeight(),
			1,
			DXGI_FORMAT_R32_FLOAT,
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			multiSampleDesc
		);

		//ボケテクスチャ出力用のレンダリングターゲットを作成する。
		multiSampleDesc.Count = 1;
		multiSampleDesc.Quality = 0;
		bokeTexture.Create(
			Engine().GetFrameBufferWidth(),
			Engine().GetFrameBufferHeight(),
			1,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			multiSampleDesc
		);
		//ブラーをかけたボケテクスチャの出力先のレンダリングターゲットを作成する。
		multiSampleDesc.Count = 1;
		multiSampleDesc.Quality = 0;
		//縦横1/2の解像度にする。
		blurBokeTexture.Create(
			Engine().GetFrameBufferWidth() >> 1,
			Engine().GetFrameBufferHeight() >> 1,
			1,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			multiSampleDesc
		);
		//頂点バッファを初期化。
		InitVertexBuffer();
		//シェーダーを初期化。
		InitShaders();
		return true;
	}
	/*!------------------------------------------------------------------
	* @brief	頂点バッファを初期化。
	------------------------------------------------------------------*/
	void InitVertexBuffer()
	{
		//頂点バッファのソースデータ。
		SSimpleVertex vertices[] =
		{
			{
				CVector4(-1.0f, -1.0f, 0.0f, 1.0f),
				CVector2(0.0f, 1.0f),
			},
			{
				CVector4(1.0f, -1.0f, 0.0f, 1.0f),
				CVector2(1.0f, 1.0f),
			},
			{
				CVector4(-1.0f, 1.0f, 0.0f, 1.0f),
				CVector2(0.0f, 0.0f)
			},
			{
				CVector4(1.0f, 1.0f, 0.0f, 1.0f),
				CVector2(1.0f, 0.0f)
			}

		};
		//頂点バッファを作成。
		vertexBuffer.Create(4, sizeof(SSimpleVertex), vertices);
	}
	/*!------------------------------------------------------------------
	* @brief	シェーダーを初期化。
	------------------------------------------------------------------*/
	void InitShaders()
	{
		//頂点シェーダーをロード。
		vsDofShader.Load("Assets/shader/Dof.fx", "VSMain", CShader::EnType::VS);
		//ボケる画像を抽出するピクセルシェーダーをロードする。
		psDofShader.Load("Assets/shader/Dof.fx", "PSDofStep_0", CShader::EnType::PS);
		psDofShader_1.Load("Assets/shader/Dof.fx", "PSDofStep_1", CShader::EnType::PS);
		psDofShader_2.Load("Assets/shader/Dof.fx", "PSDofStep_2", CShader::EnType::PS);
	}
	/*!------------------------------------------------------------------
	 * @brief	更新。
	 ------------------------------------------------------------------*/
	void Update() override
	{
		static float angle = 0.0f;
		CQuaternion rot;
		CVector3 pos = camera.GetPosition();
		if (GetAsyncKeyState(VK_UP)) {
			pos.z -= 0.002f;
		}
		if (GetAsyncKeyState(VK_DOWN)) {
			pos.z += 0.002f;
		}
		camera.SetPosition(pos);
		camera.Update();
	}
	/*!------------------------------------------------------------------
	* @brief	ZPrepass。
	* *@details
	*   深度値を書き込むためのパス。
	------------------------------------------------------------------*/
	void ZPrepass(CRenderContext& rc)
	{
		rc.OMSetRenderTargets(1, &depthTexture);
		float ClearColor[4] = { 10000.0f, 10000.0f, 10000.0f, 0.0f }; //red,green,blue,alpha
		rc.ClearRenderTargetView(0, ClearColor);
		uniModel.Draw(rc, camera.GetViewMatrix(), camera.GetProjectionMatrix(), true);
		rc.OMSetRenderTargets(1, NULL);
	}

	/*!------------------------------------------------------------------
	* @brief	シーンの描画。
	------------------------------------------------------------------*/
	void RenderScene(CRenderContext& rc)
	{
		//メインレンダリングターゲットに戻す。
		rc.OMSetRenderTargets(1, &Engine().GetMainRenderTarget());
		uniModel.Draw(rc, camera.GetViewMatrix(), camera.GetProjectionMatrix());
		rc.OMSetRenderTargets(1, NULL);
	}
	/*!------------------------------------------------------------------
	* @brief	DOF描画。
	------------------------------------------------------------------*/
	void RenderDOF(CRenderContext& rc)
	{
		//ボケるピクセルを選出。
		//DOFステップ0
		{
			//頂点バッファを設定。
			rc.IASetVertexBuffer(vertexBuffer);
			//プリミティブのトポロジーを設定。
			rc.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			//頂点シェーダーを設定。
			rc.VSSetShader(vsDofShader);
			//ピクセルシェーダーを設定。
			rc.PSSetShader(psDofShader);
			//入力レイアウトを設定。
			rc.IASetInputLayout(vsDofShader.GetInputLayout());
			//t0にシーンのテクスチャを設定する。
			rc.PSSetShaderResource(0, Engine().GetMainRenderTarget().GetRenderTargetSRV());
			//t1に深度テクスチャを設定する。
			rc.PSSetShaderResource(1, depthTexture.GetRenderTargetSRV());
			//ボケテクスチャを出力先として設定する。
			rc.OMSetRenderTargets(1, &bokeTexture);
			//クリアする。
			float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; //red,green,blue,alpha
			rc.ClearRenderTargetView(0, ClearColor);
			//描画。
			rc.Draw(4, 0);
			//レンダリングターゲットを外す。
			rc.OMSetRenderTargets(1, NULL);
		}
		//ブラーをかけてボケ画像を生成。
		//DofStep_1
		{
			//ボケテクスチャを出力先として設定する。
			rc.OMSetRenderTargets(1, &blurBokeTexture);
			//抽出したテクスチャをt2に設定する。
			rc.PSSetShaderResource(2, bokeTexture.GetRenderTargetSRV());
			rc.PSSetShader(psDofShader_1);
			//クリアする。
			float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; //red,green,blue,alpha
			rc.ClearRenderTargetView(0, ClearColor);
			rc.RSSetViewport(0.0f, 0.0f, 640.0f, 360.0f);
			//描画。
			rc.Draw(4, 0);
			//レンダリングターゲットを外す。
			rc.OMSetRenderTargets(1, NULL);
		}
		rc.RSSetViewport(0.0f, 0.0f, 1280.0f, 720.0f);
		//ボケ画像とメインシーンを合成。
		//DofStep_2
		{
			Engine().ToggleMainRenderTarget();
			rc.OMSetRenderTargets(1, &Engine().GetMainRenderTarget());
			//t3にブラーかけたテクスチャを設定する。。
			rc.PSSetShaderResource(3, blurBokeTexture.GetRenderTargetSRV());
			rc.PSSetShader(psDofShader_2);
			rc.Draw(4, 0);
			//レンダリングターゲットを外す。
			rc.OMSetRenderTargets(1, NULL);
		}
		//SRVから外す必要がある。
		//rc.PSUnsetShaderResource(0);
		//rc.PSUnsetShaderResource(1);
		
	}
	/*!------------------------------------------------------------------
	* @brief	描画。
	------------------------------------------------------------------*/
	void Render(CRenderContext& rc) override
	{
		//ZPrepass
		ZPrepass(rc);
		//シーンの描画。
		RenderScene(rc);
		//DOF
		RenderDOF(rc);
	}
};

/*!
 *@brief	メイン関数。
 */
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );
	srand((unsigned)time(NULL));
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
		NewGO<ToonRender>(0);
		//ゲームループを実行。
		Engine().RunGameLoop();
	}
	//エンジンの終了処理。
	Engine().Final();
	
    return 0;
}

