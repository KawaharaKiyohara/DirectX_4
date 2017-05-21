/*!
 *@brief	�e�N�X�`���R���o�[�g
 */
#include "stdafx.h"
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/graphics/tkCamera.h"

using namespace tkEngine2;

class DeferredRender : public IGameObject {
	enum EnGBuffer {
		enGBuffer_DiffuseTexture,		//�f�B�t���[�Y�e�N�X�`��
		enGBuffer_Num,					//G�o�b�t�@�̐��B
	};
	CRenderTarget gbuffer[enGBuffer_Num];	//GBuffer
	CShader vsDeferredLightingShader;
	CShader psDeferredLightingShader;		//�f�B�t�@�[�h���C�e�B���O�p�̃s�N�Z���V�F�[�_�[�B
	CVertexBuffer vertexBuffer;		//���_�o�b�t�@�B
	CSkinModelData skinModelData;
	CSkinModel playerModel_0;
	CSkinModel playerModel_1;
	CCamera camera;
	//���_�B
	struct SSimpleVertex {
		CVector4 pos;
		CVector2 tex;
	};
public:
	bool Start() override
	{
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
		for (int i = 0; i < enGBuffer_Num; i++) {
			gbuffer[i].Create(
				Engine().GetFrameBufferWidth(),
				Engine().GetFrameBufferHeight(),
				1,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				i == 0 ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_UNKNOWN,	//i��0�Ȃ�f�v�X�X�e���V�������B
				multiSampleDesc
			);
		}
		skinModelData.Load(L"Assets/modelData/Thethief_H.cmo");
		
		playerModel_0.Init(skinModelData);
		playerModel_1.Init(skinModelData);
		//�J�������������B
		camera.SetPosition({ 0.0f, 20.0f, 30.0f });
		camera.SetTarget({ 0.0f, 20.0f, 0.0f });
		camera.SetUp({ 0.0f, 1.0f, 0.0f });
		camera.Update();
		return true;
	}
	void Update() override
	{
		//�v���C���[0�̃��[���h�s����X�V�B
		playerModel_0.Update({10.5f, 0.0f, 0.0f}, CQuaternion::Identity, CVector3::One);
		//�v���C���[1�̃��[���h�s����X�V�B
		playerModel_1.Update({ -10.5f, 0.0f, 0.0f }, CQuaternion::Identity, CVector3::One);
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
		//�v���C���[�O�ƃv���C���[�P��`��B
		playerModel_0.Draw(rc, camera.GetViewMatrix(), camera.GetProjectionMatrix());
		playerModel_1.Draw(rc, camera.GetViewMatrix(), camera.GetProjectionMatrix());
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
		for (int i = 0; i < enGBuffer_Num; i++) {
			rc.PSSetShaderResource(i, gbuffer[i].GetRenderTargetSRV());
		}
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

