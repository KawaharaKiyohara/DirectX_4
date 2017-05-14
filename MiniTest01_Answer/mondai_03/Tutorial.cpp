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


class CTexConvertGPU : public IGameObject {
	//ガウスフィルターで使用する定数バッファ。
	struct SWeightCB {
		float weights[8];			//重みテーブル。
		
	};
	//テクスチャ情報。
	struct STexInfoCB {
		float texSize[2];
		float xBlurTexSize[2];
		float yBlurTexSize[2];
	};
	CShader m_csXBlurShader;				//Xブラー用のシェーダー。
	CShader m_csYBlurShader;				//Yブラー用のシェーダー。
	CShader m_csFinalShader;				//最終シェーダー。

	CTextureData m_textureData;					//オリジナルのテクスチャ。
	CShaderResourceView m_textureSRV;			//m_textureDataのシェーダーリソースビュー。
	CConstantBuffer m_weightCB;					//ウェイト用の定数バッファ。
	CConstantBuffer m_texInfoCB;				//テクスチャ情報用の定数バッファ。
	CStructuredBuffer m_xblurTexture;			//Xブラーをかけたテクスチャ。
	CUnorderedAccessView m_xblurTextureUAV;		//m_xblurTextureのUAV
	CStructuredBuffer m_yblurTexture;			//Yブラーをかけたテクスチャ。	
	CUnorderedAccessView m_yblurTextureUAV;		//m_yblurTextureのUAV
	CStructuredBuffer m_finalTexture;			//最終合成テクスチャ。
	CUnorderedAccessView m_finalTextureUAV;		//m_finalTextureのUAV.
	CStructuredBuffer m_finalTextureCPU;		//最終合成のテクスチャをCPUで参照するためのバッファ。
	
public:
	bool Start() override
	{
		//テクスチャをロード。
		m_textureData.Load(L"original.jpg");
		m_textureSRV.Create(m_textureData);
		//定数バッファを作成する。
		m_weightCB.Create(NULL, sizeof(SWeightCB));
		m_texInfoCB.Create(NULL, sizeof(STexInfoCB));

		const DirectX::TexMetadata& texMetadata = m_textureData.GetMetadata();
		//XBlur出力用のバッファを作成。
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;	//UAVとしてバインド可能。
			desc.ByteWidth = texMetadata.width * texMetadata.height * sizeof(SColor) * 0.5f;	//バッファのサイズは元の画像の半分のサイズ。
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = sizeof(SColor);	//一要素のサイズ。
			m_xblurTexture.Create(NULL, desc);			//出力用のバッファを作成。
			m_xblurTextureUAV.Create(m_xblurTexture);	//UAVを作成。

		}
		//YBlur出力用のバッファを作成。
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;	//UAVとしてバインド可能。
			desc.ByteWidth = texMetadata.width * texMetadata.height * sizeof(SColor) * 0.25f;	//バッファのサイズは元の画像の1/4のサイズ。
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = sizeof(SColor);	//一要素のサイズ。
			m_yblurTexture.Create(NULL, desc);		//出力用のバッファを作成。
			m_yblurTextureUAV.Create(m_yblurTexture);		//UAVを作成。
		}
		//最終合成用のバッファを作成。
		{
			
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;	//UAVとしてバインド可能。
			desc.ByteWidth = texMetadata.width * texMetadata.height * sizeof(SColor);		//バッファのサイズ。
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = sizeof(SColor);	//一要素のサイズ。
			m_finalTexture.Create(m_textureData.GetImage(0, 0, 0)->pixels, desc);
			m_finalTextureUAV.Create(m_finalTexture);

			//CPUからアクセスできるバッファを作成。
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;	//CPUから読み込み可能に設定する。
			m_finalTextureCPU.Create(m_textureData.GetImage(0, 0, 0)->pixels, desc);
		}

		//シェーダーをロード。
		m_csXBlurShader.Load("Assets/shader/GaussianBlurCS.fx", "XBlur", CShader::EnType::CS);
		m_csYBlurShader.Load("Assets/shader/GaussianBlurCS.fx", "YBlur", CShader::EnType::CS);
		m_csFinalShader.Load("Assets/shader/GaussianBlurCS.fx", "Final", CShader::EnType::CS);
		return true;
	}
	void Update() override
	{
	}
	//定数バッファの更新。
	void UpdateConstantBuffer(CRenderContext& renderContext)
	{
		float total = 0;
		SWeightCB cb;
		for (int i = 0; i < 8; i++) {
			cb.weights[i] = expf(-0.5f*(float)(i*i) / 100.0f);
			if (i == 0) {
				total += cb.weights[i];
			}
			else {
				total += 2.0f*cb.weights[i];
			}
		}
		// 規格化
		for (int i = 0; i < 8; i++) {
			cb.weights[i] /= total;
		}
		renderContext.UpdateSubresource(m_weightCB, cb);
		//テクスチャの情報を設定。
		const DirectX::TexMetadata& texMetadata = m_textureData.GetMetadata();
		STexInfoCB texInfoCB;
		texInfoCB.texSize[0] = texMetadata.width;
		texInfoCB.texSize[1] = texMetadata.height;
		texInfoCB.xBlurTexSize[0] = texMetadata.width / 2;
		texInfoCB.xBlurTexSize[1] = texMetadata.height;
		texInfoCB.yBlurTexSize[0] = texMetadata.width / 2;
		texInfoCB.yBlurTexSize[1] = texMetadata.height/ 2;
		//定数バッファの内容をVRAMにコピー。
		renderContext.UpdateSubresource(m_texInfoCB, texInfoCB);

	}
	void Render(CRenderContext& renderContext) override
	{
		//定数バッファを更新する。
		UpdateConstantBuffer(renderContext);

		const int NUM_THREAD = 4;
		//定数バッファとかいろいろ設定する。
		renderContext.CSSetConstantBuffer(0, m_weightCB);
		renderContext.CSSetConstantBuffer(1, m_texInfoCB);

		const DirectX::TexMetadata& texMetadata = m_textureData.GetMetadata();
		//Xブラー。
		{
			renderContext.CSSetShaderResource(0, m_textureSRV);
			renderContext.CSSetUnorderedAccessView(0, m_xblurTextureUAV);	//Xブラーの結果の出力先を設定。
			renderContext.CSSetShader(m_csXBlurShader);
			renderContext.Dispatch(texMetadata.width / NUM_THREAD / 2, texMetadata.height / NUM_THREAD, 1);	//横の解像度は半分。
		}
		//Yブラー。
		{
			renderContext.CSSetUnorderedAccessView(1, m_yblurTextureUAV);	//Yブラーの結果の出力先を設定。
			renderContext.CSSetShader(m_csYBlurShader);
			renderContext.Dispatch(texMetadata.width / NUM_THREAD / 2, texMetadata.height / NUM_THREAD / 2, 1);	//縦、横の解像度を半分に。
		}
		//最終合成。
		{
			renderContext.CSSetUnorderedAccessView(2, m_finalTextureUAV);	//最終の出力先を設定。
			renderContext.CSSetShader(m_csFinalShader);
			renderContext.Dispatch(texMetadata.width / NUM_THREAD, texMetadata.height / NUM_THREAD, 1);	
		}
		//VRAMからメインメモリにコピー。
		renderContext.CopyResource(m_finalTextureCPU, m_finalTexture);
		//マップ。
		CMapper<CStructuredBuffer> mapper(renderContext, m_finalTextureCPU);
		SColor* color = (SColor*)mapper.GetData();
		const DirectX::Image* image = m_textureData.GetImage(0, 0, 0);
		memcpy(image->pixels, color, sizeof(SColor)*image->width*image->height);

		//保存。
		m_textureData.Save(L"GenerateGPU.jpg", 0, 0, 0);

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
		NewGO<CTexConvertGPU>(0);
		//初期化に成功。
		//ゲームループを実行。
		Engine().RunGameLoop();
	}
	//エンジンの終了処理。
	Engine().Final();
	
    return 0;
}

