/*!
 *@brief	DirectX11�e�N�X�`���\��t���`���[�g���A���B
 */
#include "stdafx.h"
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"

using namespace tkEngine2;

class CTriangleDraw : public IGameObject {
	//�萔�o�b�t�@�B
	struct SConstantBuffer {
		CVector4 color;
	};
	CShader m_vsShader;					//VS�V�F�[�_�[�B
	CShader m_psShader;					//PS�V�F�[�_�[�B
	CVertexBuffer m_vertexBuffer;		//���_�o�b�t�@�B
	CConstantBuffer	m_constantBuffer;	//�萔�o�b�t�@�B
	CTextureData m_textureData;			//�e�N�X�`���f�[�^�B
	CShaderResourceView m_textureSRV;	//�e�N�X�`��SRV�B
	CSamplerState m_samplerState;		//�T���v���X�e�[�g�B

	struct SSimpleVertex {
		CVector4 pos;
		float uv[2];
	};
	public:
		bool Start() override
		{
			//���_�V�F�[�_�[�����[�h�B
			m_vsShader.Load("Assets/shader/Tutorial.fx", "VS", CShader::EnType::VS);
			//�s�N�Z���V�F�[�_�[�����[�h�B
			m_psShader.Load("Assets/shader/Tutorial.fx", "PS", CShader::EnType::PS);
			//�e�N�X�`�����t�@�C������쐬�B
			m_textureData.Load(L"Assets/seafloor.dds");
			//�e�N�X�`��SRV���쐬�B
			m_textureSRV.Create(m_textureData);

			D3D11_SAMPLER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			desc.MinLOD = 0;
			desc.MaxLOD = D3D11_FLOAT32_MAX;
			m_samplerState.Create(desc);

			//���_�o�b�t�@�̃\�[�X�f�[�^�B
			SSimpleVertex vertices[] =
			{
				{
					CVector4(0.0f, 0.5f, 0.5f, 1.0f),
					0.0f, 0.0f
				},
				{
					CVector4(0.5f, -0.5f, 0.5f, 1.0f),
					1.0f, 0.0f
				},
				{
					CVector4(-0.5f, -0.5f, 0.5f, 1.0f),
					1.0f, 1.0f
				}
			};
			//���_�o�b�t�@���쐬�B
			m_vertexBuffer.Create(3, sizeof(SSimpleVertex), vertices);
			//�萔�o�b�t�@�����B
			m_constantBuffer.Create(NULL, sizeof(SConstantBuffer));
			return true;
		}
		void Update() override
		{
		
		}
		void Render(CRenderContext& renderContext) override
		 {
			//���_�o�b�t�@��ݒ�B
			renderContext.IASetVertexBuffer(m_vertexBuffer);
			//�v���~�e�B�u�̃g�|���W�[��ݒ�B
			renderContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			//���_�V�F�[�_�[��ݒ�B
			renderContext.VSSetShader(m_vsShader);
			//�s�N�Z���V�F�[�_�[��ݒ�B
			renderContext.PSSetShader(m_psShader);
			//���̓��C�A�E�g��ݒ�B
			renderContext.IASetInputLayout(m_vsShader.GetInputLayout());
			//�萔�o�b�t�@�̐ݒ�B
			SConstantBuffer cb;
			cb.color.x = 1.0f;
			cb.color.y = 0.0f;
			cb.color.z = 0.0f;
			cb.color.w = 1.0f;
			//VRAM��̒萔�o�b�t�@�̓��e���X�V�B
			renderContext.UpdateSubresource(m_constantBuffer, cb);
			//�萔�o�b�t�@��PS�X�e�[�W�ɐݒ�B
			renderContext.PSSetShaderResource(0, m_textureSRV);
			renderContext.PSSetConstantBuffer(0, m_constantBuffer);
			renderContext.PSSetSampler(0, m_samplerState);
			//�`��B
			renderContext.Draw(3, 0);
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
		NewGO<CTriangleDraw>(0);
		//�������ɐ����B
		//�Q�[�����[�v�����s�B
		Engine().RunGameLoop();
	}
	//�G���W���̏I�������B
	Engine().Final();
	
    return 0;
}

