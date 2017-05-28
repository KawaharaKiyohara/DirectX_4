/*!
 *@brief	テクスチャコンバート
 */
#include "stdafx.h"
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/graphics/tkCamera.h"
#include <time.h>

using namespace tkEngine2;


class DeferredRender : public IGameObject {
	enum EnGBuffer {
		enGBuffer_DiffuseTexture,		//ディフューズテクスチャ
		enGBuffer_Normal,				//法線。
		enGBuffer_Depth,				//深度用のG-Buffer
		enGBuffer_Num,					//Gバッファの数。
	};
	//ライト構造体。
	struct SLight {
		CVector4 diffuseLightDir;		//ディフューズライトの方向。
		CVector4 diffuseLightColor;		//ディフューズライトの色。
		CVector4 ambientLight;			//アンビエントライト。
		int numPointLight;				//ポイントライトの数。
	};
	//行列パラメータ。
	struct SMatrixParam {
		CMatrix mViewProjInv;			//ビュープロジェクション行列の逆行列。
	};
	//ポイントライト。
	struct SPointLight {
		CVector4	position;		//位置。
		CVector4	color;			//カラー。
		CVector4	attn;			//減衰定数。(xを小さくするとライトの明るさが増す、yとzを小さくするとライトが遠くまで届くようになる。)
	};
	static const int NUM_POINT_LIGHT = 32;	//ポイントライトの数。
	CRenderTarget gbuffer[enGBuffer_Num];	//GBuffer
	CShader vsDeferredLightingShader;		//ディファードライティング用の頂点シェーダー。
	CShader psDeferredLightingShader;		//ディファードライティング用のピクセルシェーダー。
	CVertexBuffer vertexBuffer;				//頂点バッファ。
	SLight m_light;							//ライト。
	CConstantBuffer m_lightCB;				//ライト用の定数バッファ。
	SMatrixParam m_matrixParam;							//行列パラメータ。
	SPointLight	m_pointLightList[NUM_POINT_LIGHT];		//ポイントライトのリスト。
	CStructuredBuffer m_pointLightSB;		//ポイントライトのストラクチャーバッファ。
	CShaderResourceView m_pointLightSRV;	//ポイントライトのSRV。
	CConstantBuffer m_matrixParamCB;		//行列用の定数バッファ。
	CSkinModelData skinModelData;
	CSkinModel bgModel;
	
	CCamera camera;
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

			m_pointLightList[i].attn.x = 1.0f;
			m_pointLightList[i].attn.y = 0.01f;
			m_pointLightList[i].attn.z = 0.01f;
		}
		m_pointLightList[0].position.x = 0.0f;
		m_pointLightList[0].position.y = 10.0f;
		m_pointLightList[0].position.z = 0.0f;
		//StructuredBufferを初期化。
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		//SRVとしてバインド可能。
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	//SRVとしてバインド可能。
		desc.ByteWidth = sizeof(m_pointLightList);
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(m_pointLightList[0]);

		m_pointLightSB.Create(m_pointLightList, desc);
		//SRVを作成。
		m_pointLightSRV.Create(m_pointLightSB);
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
		//行列用の定数バッファを作成。
		m_matrixParamCB.Create(NULL, sizeof(m_matrixParam));
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
		DXGI_FORMAT colorFormatTbl[] = {
			DXGI_FORMAT_R16G16B16A16_FLOAT,	//ディフューズテクスチャ。
			DXGI_FORMAT_R16G16B16A16_FLOAT,	//法線。
			DXGI_FORMAT_R32_FLOAT,			//深度(深度は精度がほしいので32bitのR成分のみの浮動小数点フォーマット)。
		};
		for (int i = 0; i < enGBuffer_Num; i++) {
			gbuffer[i].Create(
				Engine().GetFrameBufferWidth(),
				Engine().GetFrameBufferHeight(),
				1,
				colorFormatTbl[i],
				i == 0 ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_UNKNOWN,	//iが0ならデプスステンシルを作る。
				multiSampleDesc
			);
		}
		skinModelData.Load(L"Assets/modelData/background.cmo");
		bgModel.Init(skinModelData);
		//カメラを初期化。
		camera.SetPosition({ 0.0f, 40.0f, 50.0f });
		camera.SetTarget({ 0.0f, 0.0f, 0.0f });
		camera.SetUp({ 0.0f, 1.0f, 0.0f });
		camera.Update();
		return true;
	}
	void Update() override
	{
		bgModel.Update({0.5f, 0.0f, 0.0f}, CQuaternion::Identity, CVector3::One);
		//点光源を回してみる。
		CQuaternion qRot;
		qRot.SetRotationDeg(CVector3::AxisY, 0.2f);
		qRot.Multiply(m_light.diffuseLightDir);
		for (auto& ptLight : m_pointLightList) {
			qRot.Multiply(ptLight.position);
		}
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
		//背景を描画。
		bgModel.Draw(rc, camera.GetViewMatrix(), camera.GetProjectionMatrix());
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
		int srvNo = 0;
		for (int i = 0; i < enGBuffer_Num; i++) {
			rc.PSSetShaderResource(srvNo++, gbuffer[i].GetRenderTargetSRV());
		}
		//ポイントライトを設定。
		rc.UpdateSubresource(m_pointLightSB, m_pointLightList);
		rc.PSSetShaderResource(srvNo++, m_pointLightSRV);
		//ライト用の定数バッファを設定。
		rc.UpdateSubresource(m_lightCB, m_light);
		rc.PSSetConstantBuffer(0, m_lightCB);
		//行列用の定数バッファを設定。
		m_matrixParam.mViewProjInv.Inverse(camera.GetViewProjectionMatrix());
		rc.UpdateSubresource(m_matrixParamCB, m_matrixParam);
		rc.PSSetConstantBuffer(1, m_matrixParamCB);
		
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
		NewGO<DeferredRender>(0);
		//ゲームループを実行。
		Engine().RunGameLoop();
	}
	//エンジンの終了処理。
	Engine().Final();
	
    return 0;
}

