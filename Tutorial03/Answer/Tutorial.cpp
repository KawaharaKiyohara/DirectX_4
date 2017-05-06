/*!
 *@brief	�e�N�X�`���R���o�[�g
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
//#define USE_GPU		//��`�Ńe�N�X�`���R���o�[�g��GPU�ōs���B
/////////////////////////////////////////////////////////////////////////
//GPU�Ńe�N�X�`���R���o�[�g���s���B
/////////////////////////////////////////////////////////////////////////
class CTexConvertGPU : public IGameObject {
	CTextureData m_textureData;			//�e�N�X�`���f�[�^�B
	CShader m_csShader;
	CShaderResourceView m_textureSRV;	//�V�F�[�_�[���\�[�X�r���[�B
	CStructuredBuffer m_outputBuffer;
	CStructuredBuffer m_outputBufferCPU;
	CUnorderedAccessView m_outputBufferUAV;
public:
	bool Start() override
	{
		//�e�N�X�`�������[�h�B
		m_textureData.Load(L"original.jpg");
		m_textureSRV.Create(m_textureData);

		//�o�̓o�b�t�@���쐬�B
		{
			const DirectX::TexMetadata& texMetadata = m_textureData.GetMetadata();
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;	//UAV�Ƃ��ăo�C���h�\�B
			desc.ByteWidth = texMetadata.width * texMetadata.height * sizeof(SColor);		//�o�b�t�@�̃T�C�Y�B
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = sizeof(SColor);	//��v�f�̃T�C�Y�B
			m_outputBuffer.Create(m_textureData.GetImage(0, 0, 0)->pixels, desc);
			m_outputBufferUAV.Create(m_outputBuffer);

			//CPU����A�N�Z�X�ł���o�b�t�@���쐬�B
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;	//CPU����ǂݍ��݉\�ɐݒ肷��B
			m_outputBufferCPU.Create(m_textureData.GetImage(0, 0, 0)->pixels, desc);
		}

		//�V�F�[�_�[�����[�h�B
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
		//�R���s���[�g�V�F�[�_�[�����s�B
		const int NUM_THREAD = 16;
		const DirectX::TexMetadata& texMetadata = m_textureData.GetMetadata();
		renderContext.Dispatch(texMetadata.width/ NUM_THREAD, texMetadata.height/ NUM_THREAD, 1);	
		//VRAM���烁�C���������ɃR�s�[�B
		renderContext.CopyResource(m_outputBufferCPU, m_outputBuffer);

		//�}�b�v�B
		CMapper<CStructuredBuffer> mapper(renderContext, m_outputBufferCPU);
		SColor* color = (SColor*)mapper.GetData();
		const DirectX::Image* image = m_textureData.GetImage(0, 0, 0);
		memcpy(image->pixels, color, sizeof(SColor)*image->width*image->height);

		//�ۑ��B
		m_textureData.Save(L"GenerateGPU.jpg", 0, 0, 0);
		
		sw.Stop();
		char text[256];
		sprintf(text, "�������� %f�~���b\n", sw.GetElapsedMillisecond());
		MessageBox(NULL, text, "����", MB_OK);
		DeleteGO(this);
	}
};
/////////////////////////////////////////////////////////////////////////
//CPU�Ńe�N�X�`���R���o�[�g���s���B
/////////////////////////////////////////////////////////////////////////
class CTexConvertCPU : public IGameObject {
	CTextureData m_textureData;			//�e�N�X�`���f�[�^�B
	public:
		bool Start() override
		{
			//�e�N�X�`�������[�h�B
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
			
			//�������B
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
			sprintf(text, "�������� %f�~���b\n", sw.GetElapsedMillisecond());
			MessageBox(NULL, text, "����", MB_OK);
			DeleteGO(this);
		}
};

/*!
 *@brief	���C���֐��B
 */
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );
    //tkEngine2�̏������p�����[�^��ݒ肷��B
	SInitParam initParam;
	initParam.nCmdShow = nCmdShow;
	initParam.hInstance = hInstance;
	initParam.screenWidth = 1280;
	initParam.screenHeight = 720;
	initParam.frameBufferWidth = 1280;
	initParam.frameBufferHeight = 720;
	//�G���W�����������B
	if (Engine().Init(initParam) == true) {
#ifdef USE_GPU
		NewGO<CTexConvertGPU>(0);
#else
		NewGO<CTexConvertCPU>(0);
#endif
		//�������ɐ����B
		//�Q�[�����[�v�����s�B
		Engine().RunGameLoop();
	}
	//�G���W���̏I�������B
	Engine().Final();
	
    return 0;
}

