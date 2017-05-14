/*!
*@brief	テクスチャコンバート
*/
#include "stdafx.h"
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/timer/tkStopwatch.h"

using namespace tkEngine2;

struct SColor {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

/////////////////////////////////////////////////////////////////////////
//GPUでテクスチャコンバートを行う。
/////////////////////////////////////////////////////////////////////////
class CTexConvertGPU : public IGameObject {
	CTextureData m_textureData;			//テクスチャデータ。
	CShader m_csShader;
	CShaderResourceView m_textureSRV;	//シェーダーリソースビュー。
	CStructuredBuffer m_outputBuffer;
	CStructuredBuffer m_outputBufferCPU;
	CUnorderedAccessView m_outputBufferUAV;
public:
	bool Start() override
	{
		//テクスチャをロード。
		m_textureData.Load(L"original.jpg");
		m_textureSRV.Create(m_textureData);

		//出力バッファを作成。
		{
			const DirectX::TexMetadata& texMetadata = m_textureData.GetMetadata();
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;	//UAVとしてバインド可能。
			desc.ByteWidth = texMetadata.width * texMetadata.height * sizeof(SColor);		//バッファのサイズ。
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = sizeof(SColor);	//一要素のサイズ。
			m_outputBuffer.Create(m_textureData.GetImage(0, 0, 0)->pixels, desc);
			m_outputBufferUAV.Create(m_outputBuffer);

			//CPUからアクセスできるバッファを作成。
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;	//CPUから読み込み可能に設定する。
			m_outputBufferCPU.Create(m_textureData.GetImage(0, 0, 0)->pixels, desc);
		}

		//シェーダーをロード。
		m_csShader.Load("Assets/shader/monocromeCS.fx", "CSMain", CShader::EnType::CS);
		return true;
	}
	void Update() override
	{

	}
	void Render(CRenderContext& renderContext) override
	{
		CStopwatch sw;
		sw.Start();

		renderContext.CSSetShader(m_csShader);
		renderContext.CSSetShaderResource(0, m_textureSRV);
		renderContext.CSSetUnorderedAccessView(0, m_outputBufferUAV);
		//コンピュートシェーダーを実行。
		const int NUM_THREAD = 2;
		const DirectX::TexMetadata& texMetadata = m_textureData.GetMetadata();
		renderContext.Dispatch(texMetadata.width / NUM_THREAD, texMetadata.height / NUM_THREAD, 1);
		//VRAMからメインメモリにコピー。
		renderContext.CopyResource(m_outputBufferCPU, m_outputBuffer);

		//マップ。
		CMapper<CStructuredBuffer> mapper(renderContext, m_outputBufferCPU);
		SColor* color = (SColor*)mapper.GetData();
		const DirectX::Image* image = m_textureData.GetImage(0, 0, 0);
		memcpy(image->pixels, color, sizeof(SColor)*image->width*image->height);

		//保存。
		m_textureData.Save(L"GenerateGPU.jpg", 0, 0, 0);

		sw.Stop();
		char text[256];
		sprintf(text, "処理時間 %fミリ秒\n", sw.GetElapsedMillisecond());
		MessageBox(NULL, text, "結果", MB_OK);
		DeleteGO(this);
	}
};

/*!
*@brief	メイン関数。
*/
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
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
		NewGO<CTexConvertGPU>(0);
		//初期化に成功。
		//ゲームループを実行。
		Engine().RunGameLoop();
	}
	//エンジンの終了処理。
	Engine().Final();

	return 0;
}

