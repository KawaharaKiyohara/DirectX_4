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
		const int NUM_THREAD = 2;
		const DirectX::TexMetadata& texMetadata = m_textureData.GetMetadata();
		renderContext.Dispatch(texMetadata.width / NUM_THREAD, texMetadata.height / NUM_THREAD, 1);
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

/*!
*@brief	���C���֐��B
*/
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
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
		NewGO<CTexConvertGPU>(0);
		//�������ɐ����B
		//�Q�[�����[�v�����s�B
		Engine().RunGameLoop();
	}
	//�G���W���̏I�������B
	Engine().Final();

	return 0;
}

