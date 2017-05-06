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
struct SColorf {
	float r;
	float g;
	float b;
	float a;
};
//#define USE_GPU		//定義でテクスチャコンバートをGPUで行う。
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
		const int NUM_THREAD = 16;
		const DirectX::TexMetadata& texMetadata = m_textureData.GetMetadata();
		renderContext.Dispatch(texMetadata.width/ NUM_THREAD, texMetadata.height/ NUM_THREAD, 1);	
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
/////////////////////////////////////////////////////////////////////////
//CPUでテクスチャコンバートを行う。
/////////////////////////////////////////////////////////////////////////
class CTexConvertCPU : public IGameObject {
	CTextureData m_textureData;			//テクスチャデータ。
	public:
		bool Start() override
		{
			//テクスチャをロード。
			m_textureData.Load(L"original.jpg");
			return true;
		}
		void Update() override
		{
		
		}
		void Render(CRenderContext& renderContext) override
		{
			CStopwatch sw;
			sw.Start();
			
			//白黒化。
			const DirectX::Image* image = m_textureData.GetImage(0, 0, 0);
			
			SColor* color = (SColor*)image->pixels;
			
			for (int i = 0; i < image->width * image->height; i++) {	
				//for (int i = 0; i < 1000; i++) {
				SColorf fColor;
				fColor.r = color[i].r / 255.0f;
				fColor.g = color[i].g / 255.0f;
				fColor.b = color[i].b / 255.0f;
				fColor.a = color[i].a / 255.0f;
				float Cb = -0.2f;
				float Cr = 0.1f;
				float Y = 0.299f * fColor.r + 0.587f * fColor.g + 0.114f * fColor.b;
				fColor.r = Y + 1.402f * Cr;
				fColor.g = Y - 0.34414f * Cb - 0.71414f * Cr;
				fColor.b = Y + 1.772f * Cb;
				fColor.a = 1.0f;
				color[i].r = (unsigned char)(fColor.r * 255.0f);
				color[i].g = (unsigned char)(fColor.g * 255.0f);
				color[i].b = (unsigned char)(fColor.b * 255.0f);
				color[i].a = (unsigned char)(fColor.a * 255.0f);
				//}
			}
			m_textureData.Save(L"GenerateCPU.jpg", 0, 0, 0);
			
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
#ifdef USE_GPU
		NewGO<CTexConvertGPU>(0);
#else
		NewGO<CTexConvertCPU>(0);
#endif
		//初期化に成功。
		//ゲームループを実行。
		Engine().RunGameLoop();
	}
	//エンジンの終了処理。
	Engine().Final();
	
    return 0;
}

