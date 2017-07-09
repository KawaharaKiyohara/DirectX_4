/*!
 *@brief	PBR�T���v���B
 */
#include "stdafx.h"
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/graphics/tkCamera.h"
#include "tkEngine2/timer/tkStopwatch.h"
#include <time.h>
using namespace tkEngine2;



class PBRSample : public IGameObject {
	
	//���_�B
	struct SSimpleVertex {
		CVector4 pos;
		CVector2 tex;
	};

	//���C�g�\���́B
	struct SLight {
		CVector4 diffuseLightDir;		//�f�B�t���[�Y���C�g�̕����B
		CVector4 diffuseLightColor;		//�f�B�t���[�Y���C�g�̐F�B
		CVector4 ambientLight;			//�A���r�G���g���C�g�B
		CVector4 eyePos;				//�����̈ʒu�B
	};
	/*!
	 * @brief	�}�e���A���p�����[�^�B
	 */
	struct MaterialParam{
		float roughness;		//!<�e��
		float metallic;			//!<���^���b�N�B
	};
	
	SLight m_light;								//���C�g�B
	CConstantBuffer m_lightCB;					//���C�g�p�̒萔�o�b�t�@�B
	MaterialParam m_materialParam;				//�}�e���A���p�����[�^�B
	CConstantBuffer m_materialParamCB;			//�}�e���A���p�����[�^�p�̒萔�o�b�t�@�B
	CSkinModelData skinModelData;
	CSkinModel bgModel;
	CCamera camera;
	std::unique_ptr<DirectX::SpriteFont>	m_font;
	std::unique_ptr<DirectX::SpriteBatch>	m_bach;
public:

	bool Start() override
	{
		

		skinModelData.Load(L"Assets/modelData/background.cmo");
		bgModel.Init(skinModelData);
		//�J�������������B
		camera.SetPosition({ 0.0f, 40.0f, 50.0f });
		camera.SetTarget({ 0.0f, 0.0f, 0.0f });
		camera.SetUp({ 0.0f, 1.0f, 0.0f });
		camera.Update();
		

		//���C�g�̒萔�o�b�t�@���쐬�B
		m_light.diffuseLightDir.Set({ 1.0f, 0.0f, 0.0f, 0.0f });
		m_light.diffuseLightColor.Set({ 1.0f, 1.0f, 1.0f, 1.0f });
		m_light.eyePos = camera.GetPosition();
		m_lightCB.Create(&m_light, sizeof(m_light));
		
		//�}�e���A���p�����[�^���������B
		m_materialParam.roughness = 0.5f;
		m_materialParam.metallic = 0.5f;
		m_materialParamCB.Create(&m_materialParam, sizeof(m_materialParam));
			
		//�t�H���g���������B
		m_font.reset(new DirectX::SpriteFont(Engine().GetD3DDevice(), L"Assets/font/myfile.spritefont"));
		m_bach.reset(new DirectX::SpriteBatch(Engine().GetD3DDeviceContext()));
		return true;
	}
	void Update() override
	{
		bgModel.Update({0.5f, 0.0f, 0.0f}, CQuaternion::Identity, CVector3::One);
		//�}�e���A���p���[���[�^���X�V�B
		if (GetAsyncKeyState(VK_LEFT)) {
			m_materialParam.roughness += 0.001f;
		}
		else if (GetAsyncKeyState(VK_RIGHT)) {
			m_materialParam.roughness -= 0.001f;
		}
		if (GetAsyncKeyState('A')) {
			m_materialParam.metallic += 0.001f;
		}
		else if (GetAsyncKeyState('D')) {
			m_materialParam.metallic -= 0.001f;
		}
		m_materialParam.roughness = max(m_materialParam.roughness, 0.0f);
		m_materialParam.roughness = min(m_materialParam.roughness, 1.0f);
		m_materialParam.metallic = max(m_materialParam.metallic, 0.0f);
		m_materialParam.metallic = min(m_materialParam.metallic, 1.0f);
	}
	/*!------------------------------------------------------------------
	* @brief	�V�[���̕`��B
	------------------------------------------------------------------*/
	void RenderScene(CRenderContext& rc)
	{	
		rc.UpdateSubresource(m_materialParamCB, m_materialParam);
		rc.PSSetConstantBuffer(1, m_lightCB);
		rc.PSSetConstantBuffer(2, m_materialParamCB);
		bgModel.Draw(rc, camera.GetViewMatrix(), camera.GetProjectionMatrix());
	}
	
	/*!------------------------------------------------------------------
	* @brief	�}�e���A���p�����[�^��\���B
	------------------------------------------------------------------*/
	void RenderMaterialParam(CRenderContext& rc)
	{
		wchar_t fps[256];
		swprintf(fps, L"roughness %f\nmetallic %f\n", m_materialParam.roughness, m_materialParam.metallic);
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
		//�}�e���A���p���[���[�^��\���B
		RenderMaterialParam(rc);
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
		NewGO<PBRSample>(0);
		//�Q�[�����[�v�����s�B
		Engine().RunGameLoop();
	}
	//�G���W���̏I�������B
	Engine().Final();
	
    return 0;
}

