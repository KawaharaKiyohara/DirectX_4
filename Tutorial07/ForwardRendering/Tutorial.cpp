/*!
 *@brief	TBFR�T���v���B
 */
#include "stdafx.h"
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/graphics/tkCamera.h"
#include "tkEngine2/timer/tkStopwatch.h"
#include <time.h>
using namespace tkEngine2;
namespace{
	const int NUM_POINT_LIGHT = 512;				//�|�C���g���C�g�̐��B
}


class ForwardRender : public IGameObject {
	
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

	SLight m_light;										//���C�g�B
	CConstantBuffer m_lightCB;							//���C�g�p�̒萔�o�b�t�@�B
	SCameraParam m_cameraParam;							//�J�����p�����[�^�B
	SPointLight	m_pointLightList[NUM_POINT_LIGHT];		//�|�C���g���C�g�̃��X�g�B
	CStructuredBuffer m_pointLightSB;					//�|�C���g���C�g�̃X�g���N�`���[�o�b�t�@�B
	CShaderResourceView m_pointLightSRV;				//�|�C���g���C�g��SRV�B
	CStructuredBuffer m_pointLightListInTile;			//�e�^�C���Ɋ܂܂��|�C���g���C�g�̃��X�g�̃X�g���N�`���[�h�o�b�t�@�B
	CShaderResourceView m_pointLightListInTileSRV;		//�e�^�C���Ɋ܂܂��|�C���g���C�g�̃��X�g��SRV�B
	CUnorderedAccessView m_pointLightListInTileUAV;		//�e�^�C���Ɋ܂܂��|�C���g���C�g�̃��X�g��UAV�B
	CConstantBuffer m_cameraParamCB;					//�J�����p�̒萔�o�b�t�@�B
	CSkinModelData skinModelData;
	CSkinModel bgModel;
	const int TILE_WIDTH = 16;
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

		
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		//�|�C���g���C�g�p��StructuredBuffer���������B
		{
			//SRV�Ƃ��ăo�C���h�\�B
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	//SRV�Ƃ��ăo�C���h�\�B
			desc.ByteWidth = sizeof(m_pointLightList);
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = sizeof(m_pointLightList[0]);

			m_pointLightSB.Create(m_pointLightList, desc);
			//SRV���쐬�B
			m_pointLightSRV.Create(m_pointLightSB);
		}
		//�^�C�����Ƃ̃|�C���g���C�g�̃��X�g�p��StructuredBuffer���������B
		{
			//�^�C���̐�
			int numTile = (Engine().GetFrameBufferWidth() / TILE_WIDTH)
				* (Engine().GetFrameBufferHeight() / TILE_WIDTH);
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;	//SRV��UAV�Ƀo�C���h�\�B
			desc.ByteWidth = sizeof(unsigned int) * NUM_POINT_LIGHT * numTile; 
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = sizeof(unsigned int);
			m_pointLightListInTile.Create(NULL, desc);
			m_pointLightListInTileSRV.Create(m_pointLightListInTile);
			m_pointLightListInTileUAV.Create(m_pointLightListInTile);
		}
		
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
	* @brief	�V�[���̕`��B
	------------------------------------------------------------------*/
	void RenderScene(CRenderContext& rc)
	{	
		//�|�C���g���C�g�̃��X�g��t1�ɐݒ�B
		rc.PSSetShaderResource(1, m_pointLightSRV);
		bgModel.Draw(rc, camera.GetViewMatrix(), camera.GetProjectionMatrix());
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
		//�V�[���̕`��B
		RenderScene(rc);
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
		NewGO<ForwardRender>(0);
		//�Q�[�����[�v�����s�B
		Engine().RunGameLoop();
	}
	//�G���W���̏I�������B
	Engine().Final();
	
    return 0;
}

