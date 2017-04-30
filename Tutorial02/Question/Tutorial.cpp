/*!
 *@brief	DirectX11チュートリアル01
 *@details
 * GPGPU入門。
 */
#include "stdafx.h"
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/timer/tkStopwatch.h"
#include <time.h>

using namespace tkEngine2;

class CComputeTest : public IGameObject {
	//入力パラメータ。
	struct SInputParam {
		int score;			//!<得点。
	};
	//出力パラメータ。
	struct SOutputParam {
		int avarage;		//!<平均点。
	};
	CShader m_csShader;						//!<コンピュートシェーダー。
	CStructuredBuffer m_inputBuffer;		//!<入力用GPUバッファ0。
	CStructuredBuffer m_outputBuffer;		//!<出力用GPUバッファ。
	CStructuredBuffer m_outputBufferCPU;	//!<CPUでコンピュートの結果を受け取るためのバッファ。
	static const UINT NUM_STUDENT = 30;
	int m_score[NUM_STUDENT];				//!<スコアの配列。
	CShaderResourceView m_inputSRV_0;		//!<入力SRV。
	CUnorderedAccessView m_outputUAV;		//!<出力UAV。
public:
	CComputeTest()
	{

	}
	~CComputeTest()
	{

	}
	bool Start() override
	{
		//コンピュートシェーダーをロード。
		SInputParam score[NUM_STUDENT];
		m_csShader.Load("Assets/shader/BasicCompute11.fx", "CSMain", CShader::EnType::CS);
		for (int i = 0; i < NUM_STUDENT; i++) {
			score[i].score = rand() % 100;
			TK_LOG("Score = %d\n", score[i].score);
		}
		//入力用のStructuredBufferを作成。
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			//SRVとしてバインド可能。
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	//SRVとしてバインド可能。
			desc.ByteWidth = NUM_STUDENT * sizeof(SInputParam);
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = sizeof(SInputParam);
			//StructuredBufferを作成。VRAM上にメモリを確保して入力データを転送。
			m_inputBuffer.Create(score, desc);
		}
		//出力用のStructuredBufferを作成。
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;	//UAVとしてバインド可能。
			desc.ByteWidth = sizeof(SOutputParam);			//バッファのサイズ。
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = sizeof(SOutputParam);	//一要素のサイズ。
			//StructuredBufferを作成。VRAM上に出力用のメモリを確保する。
			m_outputBuffer.Create(NULL, desc);
		}
		//出力結果をCPUで見るためのバッファを作成。
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;	//CPUから読み込み可能に設定する。
			desc.Usage = D3D11_USAGE_STAGING;				//GPUからCPUへのデータコピーをサポートする。
			desc.BindFlags = 0;								//どこにもバインドしない。
			desc.MiscFlags = 0;
			desc.ByteWidth = sizeof(SOutputParam);				//バッファのサイズ。
			desc.StructureByteStride = sizeof(SOutputParam);	//一要素のサイズ。
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			//StructuredBufferを作成。メインメモリ上に作成する。
			m_outputBufferCPU.Create(NULL, desc);
		}
		//SRVを作成。
		m_inputSRV_0.Create(m_inputBuffer);
		//UAVを作成。
		m_outputUAV.Create(m_outputBuffer);
		return true;
	}
	void Update() override
	{

	}
	void Render(CRenderContext& renderContext)
	{
		//コンピュートシェーダーを設定。
		renderContext.CSSetShader(m_csShader);
		//SRVを設定。
		renderContext.CSSetShaderResource(0, m_inputSRV_0);
		//UAVを設定。
		renderContext.CSSetUnorderedAccessView(0, m_outputUAV);
		//コンピュートシェーダーを実行。
		renderContext.Dispatch(1, 1, 1);
		//CPUからアクセスできるバッファにコピー。
		renderContext.CopyResource(m_outputBufferCPU, m_outputBuffer);
		
		//コンピュートシェーダーの結果を取得。
		CMapper<CStructuredBuffer> mapper(renderContext, m_outputBufferCPU);
		SOutputParam* p = (SOutputParam*)mapper.GetData();
		if (p) {
			char text[256];
			sprintf_s(text, "平均点 %d\n", p->avarage);
			MessageBox(NULL, text, "結果", MB_OK);
			DeleteGO(this);
		}
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
	srand(time(NULL));
	//エンジンを初期化。
	if (Engine().Init(initParam) == true) {
		NewGO<CComputeTest>(0);
		//初期化に成功。
		//ゲームループを実行。
		Engine().RunGameLoop();
	}
	//エンジンの終了処理。
	Engine().Final();
	
    return 0;
}

