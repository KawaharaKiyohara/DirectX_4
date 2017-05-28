/*!
 *@brief	テクスチャコンバート
 */
#include "stdafx.h"
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/graphics/tkCamera.h"

using namespace tkEngine2;

class DeferredRender : public IGameObject {
	enum EnGBuffer {
		enGBuffer_DiffuseTexture,		//ディフューズテクスチャ
		enGBuffer_Num,					//Gバッファの数。
	};
	CRenderTarget gbuffer[enGBuffer_Num];	//GBuffer
	CShader vsDeferredLightingShader;
	CShader psDeferredLightingShader;		//ディファードライティング用のピクセルシェーダー。
	CVertexBuffer vertexBuffer;		//頂点バッファ。
	CSkinModelData skinModelData;
	CSkinModel playerModel_0;
	CSkinModel playerModel_1;
	CCamera camera;
	//頂点。
	struct SSimpleVertex {
		CVector4 pos;
		CVector2 tex;
	};
public:
	bool Start() override
	{
		//ディファードライティング用のピクセルシェーダー。
		psDeferredLightingShader.Load("Assets/shader/DeferredLighting.fx", "PSMain", CShader::EnType::PS);
		vsDeferredLightingShader.Load("Assets/shader/DeferredLighting.fx", "VSMain", CShader::EnType::VS);
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
		//GBufferを作成する。
		DXGI_SAMPLE_DESC multiSampleDesc;
		multiSampleDesc.Count = 1;
		multiSampleDesc.Quality = 0;
		for (int i = 0; i < enGBuffer_Num; i++) {
			gbuffer[i].Create(
				Engine().GetFrameBufferWidth(),
				Engine().GetFrameBufferHeight(),
				1,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				i == 0 ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_UNKNOWN,	//iが0ならデプスステンシルを作る。
				multiSampleDesc
			);
		}
		skinModelData.Load(L"Assets/modelData/Thethief_H.cmo");
		
		playerModel_0.Init(skinModelData);
		playerModel_1.Init(skinModelData);
		//カメラを初期化。
		camera.SetPosition({ 0.0f, 20.0f, 30.0f });
		camera.SetTarget({ 0.0f, 20.0f, 0.0f });
		camera.SetUp({ 0.0f, 1.0f, 0.0f });
		camera.Update();
		return true;
	}
	void Update() override
	{
		//プレイヤー0のワールド行列を更新。
		playerModel_0.Update({10.5f, 0.0f, 0.0f}, CQuaternion::Identity, CVector3::One);
		//プレイヤー1のワールド行列を更新。
		playerModel_1.Update({ -10.5f, 0.0f, 0.0f }, CQuaternion::Identity, CVector3::One);
	}
	
	/*!------------------------------------------------------------------
	* @brief	シーンの描画を開始。
	------------------------------------------------------------------*/
	void BeginRenderScene(CRenderContext& rc)
	{
		//G-Bufferを設定する。
		rc.OMSetRenderTargets(enGBuffer_Num, gbuffer);
		//G-Bufferをクリアする。
		for (int i = 0; i < enGBuffer_Num; i++) {
			float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			rc.ClearRenderTargetView(i, color);
		}
	}
	/*!------------------------------------------------------------------
	* @brief	シーンを描画。
	------------------------------------------------------------------*/
	void RenderScene(CRenderContext& rc)
	{
		//プレイヤー０とプレイヤー１を描画。
		playerModel_0.Draw(rc, camera.GetViewMatrix(), camera.GetProjectionMatrix());
		playerModel_1.Draw(rc, camera.GetViewMatrix(), camera.GetProjectionMatrix());
	}
	/*!------------------------------------------------------------------
	* @brief	G-Bufferを使ってポストエフェクト的にライティングを行う。
	------------------------------------------------------------------*/
	void RenderMainRenderTargetFromGBuffer(CRenderContext& rc)
	{
		//レンダリングターゲットをメインに戻す。
		rc.OMSetRenderTargets(1, &Engine().GetMainRenderTarget());
		float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		rc.ClearRenderTargetView(0, color);
		//G-Bufferをセットしていく。
		for (int i = 0; i < enGBuffer_Num; i++) {
			rc.PSSetShaderResource(i, gbuffer[i].GetRenderTargetSRV());
		}
		//頂点バッファを設定。
		rc.IASetVertexBuffer(vertexBuffer);
		//プリミティブのトポロジーを設定。
		rc.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		//頂点シェーダーを設定。
		rc.VSSetShader(vsDeferredLightingShader);
		//ピクセルシェーダーを設定。
		rc.PSSetShader(psDeferredLightingShader);
		//入力レイアウトを設定。
		rc.IASetInputLayout(vsDeferredLightingShader.GetInputLayout());
		
		//描画。
		rc.Draw(4, 0);
	}
	/*!------------------------------------------------------------------
	* @brief	描画。
	------------------------------------------------------------------*/
	void Render(CRenderContext& rc) override
	{
		//シーンの描画開始。
		BeginRenderScene(rc);
		
		//シーンの描画開始。
		RenderScene(rc);

		//シーンの描画が完了したのでG-Bufferを使用してライティングを行って、
		//メインレンダリングターゲットに描画していく。
		RenderMainRenderTargetFromGBuffer(rc);
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
		NewGO<DeferredRender>(0);
		//ゲームループを実行。
		Engine().RunGameLoop();
	}
	//エンジンの終了処理。
	Engine().Final();
	
    return 0;
}

