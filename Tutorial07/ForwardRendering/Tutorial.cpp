/*!
 *@brief	TBFRサンプル。
 */
#include "stdafx.h"
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/graphics/tkCamera.h"
#include "tkEngine2/timer/tkStopwatch.h"
#include <time.h>
using namespace tkEngine2;
namespace{
	const int NUM_POINT_LIGHT = 512;				//ポイントライトの数。
}


class ForwardRender : public IGameObject {
	
	//ライト構造体。
	struct SLight {
		CVector4 diffuseLightDir;		//ディフューズライトの方向。
		CVector4 diffuseLightColor;		//ディフューズライトの色。
		CVector4 ambientLight;			//アンビエントライト。
		int numPointLight;				//ポイントライトの数。
	};
	
	//カメラパラメータ。
	struct SCameraParam {
		CMatrix mProj;			//プロジェクション行列。
		CMatrix mProjInv;		//プロジェクション行列の逆行列。
		CMatrix mViewRot;		//カメラの回転行列の逆行列。
		CVector4 screenParam;	//スクリーンパラメータ。
		
	};
	//ポイントライト。
	struct SPointLight {
		CVector3	position;		//位置。
		CVector3	positionInView;	//カメラ座標系でのポイントライトの位置。
		CVector4	color;			//カラー。
		CVector4	attn;			//減衰定数。(xを小さくするとライトの明るさが増す、yとzを小さくするとライトが遠くまで届くようになる。)
	};

	SLight m_light;										//ライト。
	CConstantBuffer m_lightCB;							//ライト用の定数バッファ。
	SCameraParam m_cameraParam;							//カメラパラメータ。
	SPointLight	m_pointLightList[NUM_POINT_LIGHT];		//ポイントライトのリスト。
	CStructuredBuffer m_pointLightSB;					//ポイントライトのストラクチャーバッファ。
	CShaderResourceView m_pointLightSRV;				//ポイントライトのSRV。
	CStructuredBuffer m_pointLightListInTile;			//各タイルに含まれるポイントライトのリストのストラクチャードバッファ。
	CShaderResourceView m_pointLightListInTileSRV;		//各タイルに含まれるポイントライトのリストのSRV。
	CUnorderedAccessView m_pointLightListInTileUAV;		//各タイルに含まれるポイントライトのリストのUAV。
	CConstantBuffer m_cameraParamCB;					//カメラ用の定数バッファ。
	CSkinModelData skinModelData;
	CSkinModel bgModel;
	const int TILE_WIDTH = 16;
	CCamera camera;
	std::unique_ptr<DirectX::SpriteFont>	m_font;
	std::unique_ptr<DirectX::SpriteBatch>	m_bach;
	//頂点。
	struct SSimpleVertex {
		CVector4 pos;
		CVector2 tex;
	};
public:
	//ポイントライトを初期化。
	void InitPointLightList()
	{
		static const int QuantizationSize = 1000;	//量子化サイズ。
		for (int i = 0; i < NUM_POINT_LIGHT; i++) {
			int ix = rand() % QuantizationSize;
			int iy = rand() % QuantizationSize;
			int iz = rand() % QuantizationSize;

			//0～999までの数字を0.0～1.0の範囲に変換する。
			float fnx = (float)ix / QuantizationSize;
			float fny = (float)iy / QuantizationSize;
			float fnz = (float)iz / QuantizationSize;
			//xとyは-1.0～1.0の範囲に変換する。
			fnx = (fnx - 0.5f) * 2.0f;
			fnz = (fnz - 0.5f) * 2.0f;
			//ポイントライトの位置をランダムに決定。
			m_pointLightList[i].position.x = 50.0f * fnx;
			m_pointLightList[i].position.y = 5.0f * fny;
			m_pointLightList[i].position.z = 50.0f * fnz;


			int ir = rand() % QuantizationSize;
			int ig = rand() % QuantizationSize;
			int ib = rand() % QuantizationSize;

			//0～999までの数字を0.0～1.0の範囲に正規化して、ポイントライトのカラーをランダムに決定。
			m_pointLightList[i].color.x = (float)ir / QuantizationSize;
			m_pointLightList[i].color.y = (float)ig / QuantizationSize;
			m_pointLightList[i].color.z = (float)ib / QuantizationSize;

			m_pointLightList[i].attn.x = 5.0f;
			m_pointLightList[i].attn.y = 0.01f;
			m_pointLightList[i].attn.z = 0.01f;
		}
		m_pointLightList[0].position.x = 0.0f;
		m_pointLightList[0].position.y = 10.0f;
		m_pointLightList[0].position.z = 0.0f;

		
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		//ポイントライト用のStructuredBufferを初期化。
		{
			//SRVとしてバインド可能。
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	//SRVとしてバインド可能。
			desc.ByteWidth = sizeof(m_pointLightList);
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = sizeof(m_pointLightList[0]);

			m_pointLightSB.Create(m_pointLightList, desc);
			//SRVを作成。
			m_pointLightSRV.Create(m_pointLightSB);
		}
		//タイルごとのポイントライトのリスト用のStructuredBufferを初期化。
		{
			//タイルの数
			int numTile = (Engine().GetFrameBufferWidth() / TILE_WIDTH)
				* (Engine().GetFrameBufferHeight() / TILE_WIDTH);
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;	//SRVとUAVにバインド可能。
			desc.ByteWidth = sizeof(unsigned int) * NUM_POINT_LIGHT * numTile; 
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = sizeof(unsigned int);
			m_pointLightListInTile.Create(NULL, desc);
			m_pointLightListInTileSRV.Create(m_pointLightListInTile);
			m_pointLightListInTileUAV.Create(m_pointLightListInTile);
		}
		
	}
	bool Start() override
	{
		//ポイントライトを初期化。
		InitPointLightList();
		//ライトの定数バッファを作成。
		m_light.diffuseLightDir.Set({ 1.0f, 0.0f, 0.0f, 0.0f });
		m_light.diffuseLightColor.Set({ 0.0f, 0.0f, 0.0f, 1.0f });
		m_light.numPointLight = NUM_POINT_LIGHT;
		//m_light.ambientLight.Set({0.2f, 0.2f, 0.2f, 1.0f});
		m_lightCB.Create(&m_light, sizeof(m_light));
		//カメラ用の定数バッファを作成。
		m_cameraParamCB.Create(NULL, sizeof(m_cameraParam));

		skinModelData.Load(L"Assets/modelData/background.cmo");
		bgModel.Init(skinModelData);
		//カメラを初期化。
		camera.SetPosition({ 0.0f, 40.0f, 50.0f });
		camera.SetTarget({ 0.0f, 0.0f, 0.0f });
		camera.SetUp({ 0.0f, 1.0f, 0.0f });
		camera.Update();
		//フォントを初期化。
		m_font.reset(new DirectX::SpriteFont(Engine().GetD3DDevice(), L"Assets/font/myfile.spritefont"));
		m_bach.reset(new DirectX::SpriteBatch(Engine().GetD3DDeviceContext()));
		return true;
	}
	void Update() override
	{
		bgModel.Update({0.5f, 0.0f, 0.0f}, CQuaternion::Identity, CVector3::One);
		//点光源を回してみる。
		CQuaternion qRot;
		qRot.SetRotationDeg(CVector3::AxisY, 0.2f);
		qRot.Multiply(m_light.diffuseLightDir);
		CMatrix mView = camera.GetViewMatrix();
		for (auto& ptLight : m_pointLightList) {
			qRot.Multiply(ptLight.position);
			ptLight.positionInView = ptLight.position;
			mView.Mul(ptLight.positionInView);
		}
		
	}
	/*!------------------------------------------------------------------
	* @brief	シーンの描画。
	------------------------------------------------------------------*/
	void RenderScene(CRenderContext& rc)
	{	
		//ポイントライトのリストをt1に設定。
		rc.PSSetShaderResource(1, m_pointLightSRV);
		bgModel.Draw(rc, camera.GetViewMatrix(), camera.GetProjectionMatrix());
	}
	/*!------------------------------------------------------------------
	* @brief	FPSを表示。
	------------------------------------------------------------------*/
	void RenderFPS(CRenderContext& rc)
	{
		wchar_t fps[256];
		swprintf(fps, L"fps %f", 1.0f / Engine().m_sw.GetElapsed());
		rc.OMSetRenderTargets(1, &Engine().GetMainRenderTarget());
		m_bach->Begin();

		m_font->DrawString(
			m_bach.get(),
			fps,
			DirectX::XMFLOAT2(0, 0),
			DirectX::Colors::White,
			0,
			DirectX::XMFLOAT2(0, 0),
			3.0f
		);
		m_bach->End();
	}
	/*!------------------------------------------------------------------
	* @brief	描画。
	------------------------------------------------------------------*/
	void Render(CRenderContext& rc) override
	{
		//シーンの描画。
		RenderScene(rc);
		//計測用にFPSを描画。
		RenderFPS(rc);
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
		NewGO<ForwardRender>(0);
		//ゲームループを実行。
		Engine().RunGameLoop();
	}
	//エンジンの終了処理。
	Engine().Final();
	
    return 0;
}

