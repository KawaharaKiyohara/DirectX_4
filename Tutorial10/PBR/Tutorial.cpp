/*!
 *@brief	PBRサンプル。
 */
#include "stdafx.h"
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/graphics/tkCamera.h"
#include "tkEngine2/timer/tkStopwatch.h"
#include <time.h>
using namespace tkEngine2;



class PBRSample : public IGameObject {
	//マテリアルパラメータの数。
	static const int NUM_MATERIAL_PARAM = 3;
	//頂点。
	struct SSimpleVertex {
		CVector4 pos;
		CVector2 tex;
	};

	//ライト構造体。
	struct SLight {
		CVector4 diffuseLightDir;		//ディフューズライトの方向。
		CVector4 diffuseLightColor;		//ディフューズライトの色。
		CVector4 ambientLight;			//アンビエントライト。
		CVector4 eyePos;				//視線の位置。
	};
	
	/*!
	 * @brief	マテリアルパラメータ。
	 */
	struct MaterialParam{
		float roughness;		//!<粗さ
		float metallic;			//!<メタリック。
		float anisotropic;		//!<異方性反射。
	};
	
	SLight m_light;								//ライト。
	CConstantBuffer m_lightCB;					//ライト用の定数バッファ。
	MaterialParam m_materialParam;				//マテリアルパラメータ。
	CConstantBuffer m_materialParamCB;			//マテリアルパラメータ用の定数バッファ。
	CSkinModelData skinModelData;
	CSkinModel bgModel;
	CCamera camera;
	std::unique_ptr<DirectX::SpriteFont>	m_font;
	std::unique_ptr<DirectX::SpriteBatch>	m_bach;
	int m_cursorPos = 0;
public:

	bool Start() override
	{
		skinModelData.Load(L"Assets/modelData/background.cmo");
		bgModel.Init(skinModelData);
		//カメラを初期化。
		camera.SetPosition({ 0.0f, 40.0f, 50.0f });
		camera.SetTarget({ 0.0f, 0.0f, 0.0f });
		camera.SetUp({ 0.0f, 1.0f, 0.0f });
		camera.Update();
		

		//ライトの定数バッファを作成。
		m_light.diffuseLightDir.Set({ 1.0f, 0.0f, 0.0f, 0.0f });
		m_light.diffuseLightColor.Set({ 1.0f, 1.0f, 1.0f, 1.0f });
		m_light.eyePos = camera.GetPosition();
		m_lightCB.Create(&m_light, sizeof(m_light));
		
		//マテリアルパラメータを初期化。
		m_materialParam.roughness = 0.5f;
		m_materialParam.metallic = 0.5f;
		m_materialParam.anisotropic = 0.5f;
		m_materialParamCB.Create(&m_materialParam, sizeof(m_materialParam));
			
		//フォントを初期化。
		m_font.reset(new DirectX::SpriteFont(Engine().GetD3DDevice(), L"Assets/font/myfile.spritefont"));
		m_bach.reset(new DirectX::SpriteBatch(Engine().GetD3DDeviceContext()));
		return true;
	}
	void Update() override
	{
		bgModel.Update({0.5f, 0.0f, 0.0f}, CQuaternion::Identity, CVector3::One);
		//マテリアルパラーメータを更新。
		static bool isPressUPDown = false;
		if (GetAsyncKeyState(VK_UP)) {
			if (isPressUPDown == false) {
				m_cursorPos--;
				isPressUPDown = true;
			}
		}
		else if (GetAsyncKeyState(VK_DOWN)) {
			if (isPressUPDown == false) {
				m_cursorPos++;
				isPressUPDown = true;
			}
		}
		else {
			isPressUPDown = false;
		}
		m_cursorPos = min(NUM_MATERIAL_PARAM-1, m_cursorPos);
		m_cursorPos = max(0, m_cursorPos);

		float* params[] = {
			&m_materialParam.roughness,
			&m_materialParam.metallic,
			&m_materialParam.anisotropic
		};
		if (GetAsyncKeyState(VK_LEFT)) {
			*params[m_cursorPos] += 0.001f;
		}
		else if (GetAsyncKeyState(VK_RIGHT)) {
			*params[m_cursorPos] -= 0.001f;
		}
		*params[m_cursorPos] = max(*params[m_cursorPos], 0.0f);
		*params[m_cursorPos] = min(*params[m_cursorPos], 1.0f);
	}
	/*!------------------------------------------------------------------
	* @brief	シーンの描画。
	------------------------------------------------------------------*/
	void RenderScene(CRenderContext& rc)
	{	
		rc.UpdateSubresource(m_materialParamCB, m_materialParam);
		rc.PSSetConstantBuffer(1, m_lightCB);
		rc.PSSetConstantBuffer(2, m_materialParamCB);
		bgModel.Draw(rc, camera.GetViewMatrix(), camera.GetProjectionMatrix());
	}
	
	/*!------------------------------------------------------------------
	* @brief	マテリアルパラメータを表示。
	------------------------------------------------------------------*/
	void RenderMaterialParam(CRenderContext& rc)
	{
		wchar_t fps[256];
		wchar_t cursor[NUM_MATERIAL_PARAM] = { L' ', L' ', L' ' };
		cursor[m_cursorPos] = L'>';
		wchar_t* fomat = L"%croughness %f\n"
						  "%cmetallic %f\n"
						  "%canisotropic %f\n";
		swprintf(fps, fomat, 
			cursor[0], m_materialParam.roughness, 
			cursor[1], m_materialParam.metallic,
			cursor[2], m_materialParam.anisotropic
		);
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
		//マテリアルパラーメータを表示。
		RenderMaterialParam(rc);
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
		NewGO<PBRSample>(0);
		//ゲームループを実行。
		Engine().RunGameLoop();
	}
	//エンジンの終了処理。
	Engine().Final();
	
    return 0;
}

