/*!
 *@brief	DirectX11�`���[�g���A��01
 *@details
 * GPGPU����B
 */
#include "stdafx.h"
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/timer/tkStopwatch.h"

using namespace tkEngine2;

class CComputeTest : public IGameObject {
	
	CShader m_csShader;						//!<�R���s���[�g�V�F�[�_�[�B
	CStructuredBuffer m_inputBuffer;		//!<���͗pGPU�o�b�t�@0�B
	CStructuredBuffer m_outputBuffer;		//!<�o�͗pGPU�o�b�t�@�B
	CStructuredBuffer m_outputBufferCPU;	//!<CPU�ŃR���s���[�g�̌��ʂ��󂯎�邽�߂̃o�b�t�@�B
	static const UINT NUM_STUDENT = 30;
	int m_score[NUM_STUDENT];				//!<�X�R�A�̔z��B
	CShaderResourceView m_inputSRV_0;		//!<����SRV�B
	CUnorderedAccessView m_outputUAV;		//!<�o��UAV�B
public:
	CComputeTest()
	{

	}
	~CComputeTest()
	{

	}
	bool Start() override
	{
		//�R���s���[�g�V�F�[�_�[�����[�h�B
		m_csShader.Load("Assets/shader/BasicCompute11.fx", "CSMain", CShader::EnType::CS);
		for (int i = 0; i < NUM_STUDENT; i++) {
			m_score[i] = i % 100;
		}
		//���͗p��StructuredBuffer���쐬�B
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			//SRV�Ƃ��ăo�C���h�\�B
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	//SRV�Ƃ��ăo�C���h�\�B
			desc.ByteWidth = NUM_STUDENT * sizeof(int);
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = sizeof(int);
			//StructuredBuffer���쐬�BVRAM��Ƀ��������m�ۂ��ē��̓f�[�^��]���B
			m_inputBuffer.Create(m_score, desc);
		}
		//�o�͗p��StructuredBuffer���쐬�B
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;	//UAV�Ƃ��ăo�C���h�\�B
			desc.ByteWidth = sizeof(int);
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = sizeof(int);
			//StructuredBuffer���쐬�BVRAM��ɏo�͗p�̃��������m�ۂ���B
			m_outputBuffer.Create(NULL, desc);
		}
		//�o�͌��ʂ�CPU�Ō��邽�߂̃o�b�t�@���쐬�B
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;	//CPU����ǂݍ��݉\�ɐݒ肷��B
			desc.Usage = D3D11_USAGE_STAGING;				//GPU����CPU�ւ̃f�[�^�R�s�[���T�|�[�g����B
			desc.BindFlags = 0;								//�ǂ��ɂ��o�C���h���Ȃ��B
			desc.MiscFlags = 0;
			desc.ByteWidth = sizeof(int);
			desc.StructureByteStride = sizeof(int);
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			//StructuredBuffer���쐬�B���C����������ɍ쐬����B
			m_outputBufferCPU.Create(NULL, desc);
		}
		//SRV���쐬�B
		m_inputSRV_0.Create(m_inputBuffer);
		//UAV���쐬�B
		m_outputUAV.Create(m_outputBuffer);
		return true;
	}
	void Update() override
	{

	}
	void Render(CRenderContext& renderContext)
	{
		//�R���s���[�g�V�F�[�_�[��ݒ�B
		renderContext.CSSetShader(m_csShader);
		//SRV��ݒ�B
		renderContext.CSSetShaderResource(0, m_inputSRV_0);
		//UAV��ݒ�B
		renderContext.CSSetUnorderedAccessView(0, m_outputUAV);
		//�R���s���[�g�V�F�[�_�[�����s�B
		renderContext.Dispatch(1, 1, 1);
		//CPU����A�N�Z�X�ł���o�b�t�@�ɃR�s�[�B
		renderContext.CopyResource(m_outputBufferCPU, m_outputBuffer);
		
		//�R���s���[�g�V�F�[�_�[�̌��ʂ��擾�B
		CMapper<CStructuredBuffer> mapper(renderContext, m_outputBufferCPU);
		int* p = (int*)mapper.GetData();
		if (p) {
			TK_LOG("���ϓ_ %d\n", *p);
			
		}
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
		NewGO<CComputeTest>(0);
		//�������ɐ����B
		//�Q�[�����[�v�����s�B
		Engine().RunGameLoop();
	}
	//�G���W���̏I�������B
	Engine().Final();
	
    return 0;
}

