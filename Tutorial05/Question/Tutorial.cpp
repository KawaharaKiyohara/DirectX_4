/*!
 *@brief	�e�N�X�`���R���o�[�g
 */
#include "stdafx.h"
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/graphics/tkCamera.h"
#include <time.h>

using namespace tkEngine2;


class DeferredRender : public IGameObject {
	enum EnGBuffer {
		enGBuffer_DiffuseTexture,		//�f�B�t���[�Y�e�N�X�`��
		enGBuffer_Normal,				//�@���B
		enGBuffer_Num,					//G�o�b�t�@�̐��B
	};
	//���C�g�\���́B
	struct SLight {
		CVector4 diffuseLightDir;		//�f�B�t���[�Y���C�g�̕����B
		CVector4 diffuseLightColor;		//�f�B�t���[�Y���C�g�̐F�B
		CVector4 ambientLight;			//�A���r�G���g���C�g�B
	};
	//�s��p�����[�^�B
	struct SMatrixParam {
		CMatrix mViewProjInv;			//�r���[�v���W�F�N�V�����s��̋t�s��B
	};

	static const int NUM_POINT_LIGHT = 32;	//�|�C���g���C�g�̐��B
	CRenderTarget gbuffer[enGBuffer_Num];	//GBuffer
	CShader vsDeferredLightingShader;		//�f�B�t�@�[�h���C�e�B���O�p�̒��_�V�F�[�_�[�B
	CShader psDeferredLightingShader;		//�f�B�t�@�[�h���C�e�B���O�p�̃s�N�Z���V�F�[�_�[�B
	CVertexBuffer vertexBuffer;				//���_�o�b�t�@�B
	SLight m_light;							//���C�g�B
	CConstantBuffer m_lightCB;				//���C�g�p�̒萔�o�b�t�@�B
	CSkinModelData skinModelData;
	CSkinModel bgModel;
	
	CCamera camera;
	//���_�B
	struct SSimpleVertex {
		CVector4 pos;
		CVector2 tex;
	};
public:
	bool Start() override
	{
		//���C�g�̒萔�o�b�t�@���쐬�B
		m_light.diffuseLightDir.Set({ 1.0f, 0.0f, 0.0f, 0.0f });
		m_light.diffuseLightColor.Set({ 0.3f, 0.3f, 0.3f, 1.0f });
		m_light.ambientLight.Set({0.1f, 0.1f, 0.1f, 1.0f});
		m_lightCB.Create(&m_light, sizeof(m_light));
		
		//�f�B�t�@�[�h���C�e�B���O�p�̃s�N�Z���V�F�[�_�[�B
		psDeferredLightingShader.Load("Assets/shader/DeferredLighting.fx", "PSMain", CShader::EnType::PS);
		vsDeferredLightingShader.Load("Assets/shader/DeferredLighting.fx", "VSMain", CShader::EnType::VS);
		//���_�o�b�t�@�̃\�[�X�f�[�^�B
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
		//���_�o�b�t�@���쐬�B
		vertexBuffer.Create(4, sizeof(SSimpleVertex), vertices);
		//GBuffer���쐬����B
		DXGI_SAMPLE_DESC multiSampleDesc;
		multiSampleDesc.Count = 1;
		multiSampleDesc.Quality = 0;
		DXGI_FORMAT colorFormatTbl[] = {
			DXGI_FORMAT_R16G16B16A16_FLOAT,	//�f�B�t���[�Y�e�N�X�`���B
			DXGI_FORMAT_R16G16B16A16_FLOAT,	//�@���B
			DXGI_FORMAT_R32_FLOAT,			//�[�x(�[�x�͐��x���ق����̂�32bit��R�����݂̂̕��������_�t�H�[�}�b�g)�B
		};
		for (int i = 0; i < enGBuffer_Num; i++) {
			gbuffer[i].Create(
				Engine().GetFrameBufferWidth(),
				Engine().GetFrameBufferHeight(),
				1,
				colorFormatTbl[i],
				i == 0 ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_UNKNOWN,	//i��0�Ȃ�f�v�X�X�e���V�������B
				multiSampleDesc
			);
		}
		skinModelData.Load(L"Assets/modelData/background.cmo");
		bgModel.Init(skinModelData);
		//�J�������������B
		camera.SetPosition({ 0.0f, 40.0f, 50.0f });
		camera.SetTarget({ 0.0f, 0.0f, 0.0f });
		camera.SetUp({ 0.0f, 1.0f, 0.0f });
		camera.Update();
		return true;
	}
	void Update() override
	{
		bgModel.Update({0.5f, 0.0f, 0.0f}, CQuaternion::Identity, CVector3::One);
		//���C�g���񂵂Ă݂�B
		CQuaternion qRot;
		qRot.SetRotationDeg(CVector3::AxisY, 0.2f);
		qRot.Multiply(m_light.diffuseLightDir);
	}
	
	/*!------------------------------------------------------------------
	* @brief	�V�[���̕`����J�n�B
	------------------------------------------------------------------*/
	void BeginRenderScene(CRenderContext& rc)
	{
		//G-Buffer��ݒ肷��B
		rc.OMSetRenderTargets(enGBuffer_Num, gbuffer);
		//G-Buffer���N���A����B
		for (int i = 0; i < enGBuffer_Num; i++) {
			float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			rc.ClearRenderTargetView(i, color);
		}
	}
	/*!------------------------------------------------------------------
	* @brief	�V�[����`��B
	------------------------------------------------------------------*/
	void RenderScene(CRenderContext& rc)
	{
		//�w�i��`��B
		bgModel.Draw(rc, camera.GetViewMatrix(), camera.GetProjectionMatrix());
	}
	/*!------------------------------------------------------------------
	* @brief	G-Buffer���g���ă|�X�g�G�t�F�N�g�I�Ƀ��C�e�B���O���s���B
	------------------------------------------------------------------*/
	void RenderMainRenderTargetFromGBuffer(CRenderContext& rc)
	{
		//�����_�����O�^�[�Q�b�g�����C���ɖ߂��B
		rc.OMSetRenderTargets(1, &Engine().GetMainRenderTarget());
		float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		rc.ClearRenderTargetView(0, color);
		//G-Buffer���Z�b�g���Ă����B
		int srvNo = 0;
		for (int i = 0; i < enGBuffer_Num; i++) {
			rc.PSSetShaderResource(srvNo++, gbuffer[i].GetRenderTargetSRV());
		}
		//���C�g�p�̒萔�o�b�t�@��ݒ�B
		rc.UpdateSubresource(m_lightCB, m_light);
		rc.PSSetConstantBuffer(0, m_lightCB);
		
		//���_�o�b�t�@��ݒ�B
		rc.IASetVertexBuffer(vertexBuffer);
		//�v���~�e�B�u�̃g�|���W�[��ݒ�B
		rc.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		//���_�V�F�[�_�[��ݒ�B
		rc.VSSetShader(vsDeferredLightingShader);
		//�s�N�Z���V�F�[�_�[��ݒ�B
		rc.PSSetShader(psDeferredLightingShader);
		//���̓��C�A�E�g��ݒ�B
		rc.IASetInputLayout(vsDeferredLightingShader.GetInputLayout());
		
		//�`��B
		rc.Draw(4, 0);
	}
	/*!------------------------------------------------------------------
	* @brief	�`��B
	------------------------------------------------------------------*/
	void Render(CRenderContext& rc) override
	{
		//�V�[���̕`��J�n�B
		BeginRenderScene(rc);
		
		//�V�[���̕`��J�n�B
		RenderScene(rc);

		//�V�[���̕`�悪���������̂�G-Buffer���g�p���ă��C�e�B���O���s���āA
		//���C�������_�����O�^�[�Q�b�g�ɕ`�悵�Ă����B
		RenderMainRenderTargetFromGBuffer(rc);
	}
};

/*!
 *@brief	���C���֐��B
 */
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );
	srand((unsigned)time(NULL));
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
		//�������ɐ����B
		NewGO<DeferredRender>(0);
		//�Q�[�����[�v�����s�B
		Engine().RunGameLoop();
	}
	//�G���W���̏I�������B
	Engine().Final();
	
    return 0;
}

