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


class CTexConvertGPU : public IGameObject {
	//�K�E�X�t�B���^�[�Ŏg�p����萔�o�b�t�@�B
	struct SWeightCB {
		float weights[8];			//�d�݃e�[�u���B
		
	};
	//�e�N�X�`�����B
	struct STexInfoCB {
		float texSize[2];
		float xBlurTexSize[2];
		float yBlurTexSize[2];
	};
	CShader m_csXBlurShader;				//X�u���[�p�̃V�F�[�_�[�B
	CShader m_csYBlurShader;				//Y�u���[�p�̃V�F�[�_�[�B
	CShader m_csFinalShader;				//�ŏI�V�F�[�_�[�B

	CTextureData m_textureData;					//�I���W�i���̃e�N�X�`���B
	CShaderResourceView m_textureSRV;			//m_textureData�̃V�F�[�_�[���\�[�X�r���[�B
	CConstantBuffer m_weightCB;					//�E�F�C�g�p�̒萔�o�b�t�@�B
	CConstantBuffer m_texInfoCB;				//�e�N�X�`�����p�̒萔�o�b�t�@�B
	CStructuredBuffer m_xblurTexture;			//X�u���[���������e�N�X�`���B
	CUnorderedAccessView m_xblurTextureUAV;		//m_xblurTexture��UAV
	CStructuredBuffer m_yblurTexture;			//Y�u���[���������e�N�X�`���B	
	CUnorderedAccessView m_yblurTextureUAV;		//m_yblurTexture��UAV
	CStructuredBuffer m_finalTexture;			//�ŏI�����e�N�X�`���B
	CUnorderedAccessView m_finalTextureUAV;		//m_finalTexture��UAV.
	CStructuredBuffer m_finalTextureCPU;		//�ŏI�����̃e�N�X�`����CPU�ŎQ�Ƃ��邽�߂̃o�b�t�@�B
	
public:
	bool Start() override
	{
		//�e�N�X�`�������[�h�B
		m_textureData.Load(L"original.jpg");
		m_textureSRV.Create(m_textureData);
		//�萔�o�b�t�@���쐬����B
		m_weightCB.Create(NULL, sizeof(SWeightCB));
		m_texInfoCB.Create(NULL, sizeof(STexInfoCB));

		const DirectX::TexMetadata& texMetadata = m_textureData.GetMetadata();
		//XBlur�o�͗p�̃o�b�t�@���쐬�B
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;	//UAV�Ƃ��ăo�C���h�\�B
			desc.ByteWidth = texMetadata.width * texMetadata.height * sizeof(SColor) * 0.5f;	//�o�b�t�@�̃T�C�Y�͌��̉摜�̔����̃T�C�Y�B
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = sizeof(SColor);	//��v�f�̃T�C�Y�B
			m_xblurTexture.Create(NULL, desc);			//�o�͗p�̃o�b�t�@���쐬�B
			m_xblurTextureUAV.Create(m_xblurTexture);	//UAV���쐬�B

		}
		//YBlur�o�͗p�̃o�b�t�@���쐬�B
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;	//UAV�Ƃ��ăo�C���h�\�B
			desc.ByteWidth = texMetadata.width * texMetadata.height * sizeof(SColor) * 0.25f;	//�o�b�t�@�̃T�C�Y�͌��̉摜��1/4�̃T�C�Y�B
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = sizeof(SColor);	//��v�f�̃T�C�Y�B
			m_yblurTexture.Create(NULL, desc);		//�o�͗p�̃o�b�t�@���쐬�B
			m_yblurTextureUAV.Create(m_yblurTexture);		//UAV���쐬�B
		}
		//�ŏI�����p�̃o�b�t�@���쐬�B
		{
			
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;	//UAV�Ƃ��ăo�C���h�\�B
			desc.ByteWidth = texMetadata.width * texMetadata.height * sizeof(SColor);		//�o�b�t�@�̃T�C�Y�B
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = sizeof(SColor);	//��v�f�̃T�C�Y�B
			m_finalTexture.Create(m_textureData.GetImage(0, 0, 0)->pixels, desc);
			m_finalTextureUAV.Create(m_finalTexture);

			//CPU����A�N�Z�X�ł���o�b�t�@���쐬�B
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;	//CPU����ǂݍ��݉\�ɐݒ肷��B
			m_finalTextureCPU.Create(m_textureData.GetImage(0, 0, 0)->pixels, desc);
		}

		//�V�F�[�_�[�����[�h�B
		m_csXBlurShader.Load("Assets/shader/GaussianBlurCS.fx", "XBlur", CShader::EnType::CS);
		m_csYBlurShader.Load("Assets/shader/GaussianBlurCS.fx", "YBlur", CShader::EnType::CS);
		m_csFinalShader.Load("Assets/shader/GaussianBlurCS.fx", "Final", CShader::EnType::CS);
		return true;
	}
	void Update() override
	{
	}
	//�萔�o�b�t�@�̍X�V�B
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
		// �K�i��
		for (int i = 0; i < 8; i++) {
			cb.weights[i] /= total;
		}
		renderContext.UpdateSubresource(m_weightCB, cb);
		//�e�N�X�`���̏���ݒ�B
		const DirectX::TexMetadata& texMetadata = m_textureData.GetMetadata();
		STexInfoCB texInfoCB;
		texInfoCB.texSize[0] = texMetadata.width;
		texInfoCB.texSize[1] = texMetadata.height;
		texInfoCB.xBlurTexSize[0] = texMetadata.width / 2;
		texInfoCB.xBlurTexSize[1] = texMetadata.height;
		texInfoCB.yBlurTexSize[0] = texMetadata.width / 2;
		texInfoCB.yBlurTexSize[1] = texMetadata.height/ 2;
		//�萔�o�b�t�@�̓��e��VRAM�ɃR�s�[�B
		renderContext.UpdateSubresource(m_texInfoCB, texInfoCB);

	}
	void Render(CRenderContext& renderContext) override
	{
		//�萔�o�b�t�@���X�V����B
		UpdateConstantBuffer(renderContext);

		const int NUM_THREAD = 4;
		//�萔�o�b�t�@�Ƃ����낢��ݒ肷��B
		renderContext.CSSetConstantBuffer(0, m_weightCB);
		renderContext.CSSetConstantBuffer(1, m_texInfoCB);

		const DirectX::TexMetadata& texMetadata = m_textureData.GetMetadata();
		//X�u���[�B
		{
			renderContext.CSSetShaderResource(0, m_textureSRV);
			renderContext.CSSetUnorderedAccessView(0, m_xblurTextureUAV);	//X�u���[�̌��ʂ̏o�͐��ݒ�B
			renderContext.CSSetShader(m_csXBlurShader);
			renderContext.Dispatch(texMetadata.width / NUM_THREAD / 2, texMetadata.height / NUM_THREAD, 1);	//���̉𑜓x�͔����B
		}
		//Y�u���[�B
		{
			renderContext.CSSetUnorderedAccessView(1, m_yblurTextureUAV);	//Y�u���[�̌��ʂ̏o�͐��ݒ�B
			renderContext.CSSetShader(m_csYBlurShader);
			renderContext.Dispatch(texMetadata.width / NUM_THREAD / 2, texMetadata.height / NUM_THREAD / 2, 1);	//�c�A���̉𑜓x�𔼕��ɁB
		}
		//�ŏI�����B
		{
			renderContext.CSSetUnorderedAccessView(2, m_finalTextureUAV);	//�ŏI�̏o�͐��ݒ�B
			renderContext.CSSetShader(m_csFinalShader);
			renderContext.Dispatch(texMetadata.width / NUM_THREAD, texMetadata.height / NUM_THREAD, 1);	
		}
		//VRAM���烁�C���������ɃR�s�[�B
		renderContext.CopyResource(m_finalTextureCPU, m_finalTexture);
		//�}�b�v�B
		CMapper<CStructuredBuffer> mapper(renderContext, m_finalTextureCPU);
		SColor* color = (SColor*)mapper.GetData();
		const DirectX::Image* image = m_textureData.GetImage(0, 0, 0);
		memcpy(image->pixels, color, sizeof(SColor)*image->width*image->height);

		//�ۑ��B
		m_textureData.Save(L"GenerateGPU.jpg", 0, 0, 0);

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
		NewGO<CTexConvertGPU>(0);
		//�������ɐ����B
		//�Q�[�����[�v�����s�B
		Engine().RunGameLoop();
	}
	//�G���W���̏I�������B
	Engine().Final();
	
    return 0;
}

