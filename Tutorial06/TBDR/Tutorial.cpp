/*!
 *@brief	TBDR�T���v���B
 */
#include "stdafx.h"
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/graphics/tkCamera.h"
#include "tkEngine2/timer/tkStopwatch.h"
#include <time.h>


using namespace tkEngine2;
namespace{
	const int NUM_POINT_LIGHT = 64;				//�|�C���g���C�g�̐��B
}

class DeferredRender : public IGameObject {
	enum EnGBuffer {
		enGBuffer_DiffuseTexture,		//�f�B�t���[�Y�e�N�X�`��
		enGBuffer_Normal,				//�@���B
		enGBuffer_Depth,				//�[�x�p��G-Buffer
		enGBuffer_Num,					//G�o�b�t�@�̐��B
	};
	//���C�g�\���́B
	struct SLight {
		CVector4 diffuseLightDir;		//�f�B�t���[�Y���C�g�̕����B
		CVector4 diffuseLightColor;		//�f�B�t���[�Y���C�g�̐F�B
		CVector4 ambientLight;			//�A���r�G���g���C�g�B
		int numPointLight;				//�|�C���g���C�g�̐��B
	};
	
	//�J�����p�����[�^�B
	struct SCameraParam {
		CMatrix mProj;			//�v���W�F�N�V�����s��B
		CMatrix mProjInv;		//�v���W�F�N�V�����s��̋t�s��B
		CMatrix mViewRot;		//�J�����̉�]�s��̋t�s��B
		CVector4 screenParam;	//�X�N���[���p�����[�^�B
		
	};
	//�|�C���g���C�g�B
	struct SPointLight {
		CVector3	position;		//�ʒu�B
		CVector3	positionInView;	//�J�������W�n�ł̃|�C���g���C�g�̈ʒu�B
		CVector4	color;			//�J���[�B
		CVector4	attn;			//�����萔�B(x������������ƃ��C�g�̖��邳�������Ay��z������������ƃ��C�g�������܂œ͂��悤�ɂȂ�B)
	};
	
	CRenderTarget gbuffer[enGBuffer_Num];				//GBuffer
	CShader csTBDR;										//TBDR(TileBasedDifferedRendering)�p�̃R���s���[�g�V�F�[�_�[�B
	CVertexBuffer vertexBuffer;							//���_�o�b�t�@�B
	SLight m_light;										//���C�g�B
	CConstantBuffer m_lightCB;							//���C�g�p�̒萔�o�b�t�@�B
	SCameraParam m_cameraParam;							//�J�����p�����[�^�B
	SPointLight	m_pointLightList[NUM_POINT_LIGHT];		//�|�C���g���C�g�̃��X�g�B
	CStructuredBuffer m_pointLightSB;					//�|�C���g���C�g�̃X�g���N�`���[�o�b�t�@�B
	CShaderResourceView m_pointLightSRV;				//�|�C���g���C�g��SRV�B
	CConstantBuffer m_cameraParamCB;					//�J�����p�̒萔�o�b�t�@�B
	CSkinModelData skinModelData;
	CSkinModel bgModel;
	
	CCamera camera;
	std::unique_ptr<DirectX::SpriteFont>	m_font;
	std::unique_ptr<DirectX::SpriteBatch>	m_bach;
	//���_�B
	struct SSimpleVertex {
		CVector4 pos;
		CVector2 tex;
	};
public:
	//�|�C���g���C�g���������B
	void InitPointLightList()
	{
		static const int QuantizationSize = 1000;	//�ʎq���T�C�Y�B
		for (int i = 0; i < NUM_POINT_LIGHT; i++) {
			int ix = rand() % QuantizationSize;
			int iy = rand() % QuantizationSize;
			int iz = rand() % QuantizationSize;

			//0�`999�܂ł̐�����0.0�`1.0�͈̔͂ɕϊ�����B
			float fnx = (float)ix / QuantizationSize;
			float fny = (float)iy / QuantizationSize;
			float fnz = (float)iz / QuantizationSize;
			//x��y��-1.0�`1.0�͈̔͂ɕϊ�����B
			fnx = (fnx - 0.5f) * 2.0f;
			fnz = (fnz - 0.5f) * 2.0f;
			//�|�C���g���C�g�̈ʒu�������_���Ɍ���B
			m_pointLightList[i].position.x = 50.0f * fnx;
			m_pointLightList[i].position.y = 5.0f * fny;
			m_pointLightList[i].position.z = 50.0f * fnz;


			int ir = rand() % QuantizationSize;
			int ig = rand() % QuantizationSize;
			int ib = rand() % QuantizationSize;

			//0�`999�܂ł̐�����0.0�`1.0�͈̔͂ɐ��K�����āA�|�C���g���C�g�̃J���[�������_���Ɍ���B
			m_pointLightList[i].color.x = (float)ir / QuantizationSize;
			m_pointLightList[i].color.y = (float)ig / QuantizationSize;
			m_pointLightList[i].color.z = (float)ib / QuantizationSize;

			m_pointLightList[i].attn.x = 5.0f;
			m_pointLightList[i].attn.y = 0.01f;
			m_pointLightList[i].attn.z = 0.01f;
		}
		m_pointLightList[0].position.x = 0.0f;
		m_pointLightList[0].position.y = 10.0f;
		m_pointLightList[0].position.z = 0.0f;

		//StructuredBuffer���������B
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		//SRV�Ƃ��ăo�C���h�\�B
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	//SRV�Ƃ��ăo�C���h�\�B
		desc.ByteWidth = sizeof(m_pointLightList);
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(m_pointLightList[0]);

		m_pointLightSB.Create(m_pointLightList, desc);
		//SRV���쐬�B
		m_pointLightSRV.Create(m_pointLightSB);
	}
	bool Start() override
	{
		//�|�C���g���C�g���������B
		InitPointLightList();
		//���C�g�̒萔�o�b�t�@���쐬�B
		m_light.diffuseLightDir.Set({ 1.0f, 0.0f, 0.0f, 0.0f });
		m_light.diffuseLightColor.Set({ 0.0f, 0.0f, 0.0f, 1.0f });
		m_light.numPointLight = NUM_POINT_LIGHT;
		//m_light.ambientLight.Set({0.2f, 0.2f, 0.2f, 1.0f});
		m_lightCB.Create(&m_light, sizeof(m_light));
		//�J�����p�̒萔�o�b�t�@���쐬�B
		m_cameraParamCB.Create(NULL, sizeof(m_cameraParam));
		
		//TBDR�p�̃R���s���[�g�V�F�[�_�[�B
		csTBDR.Load("Assets/shader/TBDR.fx", "CSMain", CShader::EnType::CS);
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
		//�t�H���g���������B
		m_font.reset(new DirectX::SpriteFont(Engine().GetD3DDevice(), L"Assets/font/myfile.spritefont"));
		m_bach.reset(new DirectX::SpriteBatch(Engine().GetD3DDeviceContext()));
		return true;
	}
	void Update() override
	{
		bgModel.Update({0.5f, 0.0f, 0.0f}, CQuaternion::Identity, CVector3::One);
		//�_�������񂵂Ă݂�B
		CQuaternion qRot;
		qRot.SetRotationDeg(CVector3::AxisY, 0.2f);
		qRot.Multiply(m_light.diffuseLightDir);
		CMatrix mView = camera.GetViewMatrix();
		for (auto& ptLight : m_pointLightList) {
			qRot.Multiply(ptLight.position);
			ptLight.positionInView = ptLight.position;
			mView.Mul(ptLight.positionInView);
		}
		
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
	* @brief	TBDR
	------------------------------------------------------------------*/
	void TileBaseDefferdRendring(CRenderContext& rc)
	{
		//�����_�����O�^�[�Q�b�g���O���B
		rc.OMSetRenderTargets(enGBuffer_Num, NULL);

		rc.CSSetShader(csTBDR);
		const int TILE_WIDTH = 16;
		//G-Buffer��ݒ�B
		for (int i = 0; i < enGBuffer_Num; i++) {
			rc.CSSetShaderResource(i, gbuffer[i].GetRenderTargetSRV());
		}
		//�|�C���g���C�g�̃��X�g��ݒ�B
		rc.UpdateSubresource(m_pointLightSB, m_pointLightList);
		rc.CSSetShaderResource(enGBuffer_Num, m_pointLightSRV);
		//�J�����萔�o�b�t�@�̐ݒ�B
		m_cameraParam.screenParam.x = camera.GetNear();
		m_cameraParam.screenParam.y = camera.GetFar();
		m_cameraParam.screenParam.z = Engine().GetFrameBufferWidth();
		m_cameraParam.screenParam.w = Engine().GetFrameBufferHeight();
		m_cameraParam.mProj = camera.GetProjectionMatrix();
		m_cameraParam.mProjInv.Inverse(m_cameraParam.mProj);
		m_cameraParam.mViewRot.Inverse(camera.GetViewMatrix());
		m_cameraParam.mViewRot.m[3][0] = 0.0f;
		m_cameraParam.mViewRot.m[3][1] = 0.0f;
		m_cameraParam.mViewRot.m[3][2] = 0.0f;
		m_cameraParam.mViewRot.Transpose();

		rc.UpdateSubresource(m_cameraParamCB, m_cameraParam);
		rc.CSSetConstantBuffer(0, m_cameraParamCB);
		//�������ݐ���w��B
		rc.CSSetUnorderedAccessView(0, Engine().GetMainRenderTarget().GetRenderTargetUAV());
		rc.Dispatch(Engine().GetFrameBufferWidth() / TILE_WIDTH, Engine().GetFrameBufferHeight() / TILE_WIDTH, 1);
	}
	/*!------------------------------------------------------------------
	* @brief	FPS��\���B
	------------------------------------------------------------------*/
	void RenderFPS(CRenderContext& rc)
	{
		wchar_t fps[256];
		swprintf(fps, L"fps %f", 1.0f / Engine().m_sw.GetElapsed());
		rc.OMSetRenderTargets(1, &Engine().GetMainRenderTarget());
		m_bach->Begin();

		m_font->DrawString(
			m_bach.get(),
			fps,
			DirectX::XMFLOAT2(0, 0),
			DirectX::Colors::White,
			0,
			DirectX::XMFLOAT2(0, 0),
			3.0f
		);
		m_bach->End();
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
		//TBDR�����s�B
		TileBaseDefferdRendring(rc);
		//�v���p��FPS��`��B
		RenderFPS(rc);
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

