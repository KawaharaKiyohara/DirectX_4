/*!
 * @brief	シェーダー。
 */
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/graphics/tkShader.h"
#include "tkEngine2/tkEngine.h"

namespace tkEngine2{
	namespace {
		void ReadFile(const char* filePath, char* readBuff, int& fileSize)
		{
			FILE* fp = fopen(filePath, "rb");
			fseek(fp, 0, SEEK_END);
			fpos_t fPos;
			fgetpos(fp, &fPos);
			fseek(fp, 0, SEEK_SET);
			fileSize = (int)fPos;
			fread(readBuff, fileSize, 1, fp);
			fclose(fp);
		}
	}
	CShader::CShader()
	{
	}
	CShader::~CShader()
	{
		Release();
	}
	
	void CShader::Release()
	{
		if(m_shader != nullptr){
			switch (m_shaderType) {
			case EnType::VS:
				((ID3D11VertexShader*)m_shader)->Release();
				break;
			case EnType::PS:
				((ID3D11PixelShader*)m_shader)->Release();
				break;
			case EnType::CS:
				((ID3D11ComputeShader*)m_shader)->Release();
				break;
			}
			m_shader = nullptr;
		}

		if (m_inputLayout != nullptr) {
			m_inputLayout->Release();
			m_inputLayout = nullptr;
		}
	}
	
	bool CShader::Load(const char* filePath, const char* entryFuncName, EnType shaderType)
	{
		Release();
		HRESULT hr = S_OK;

	    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	#if defined( DEBUG ) || defined( _DEBUG )
	    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	    // Setting this flag improves the shader debugging experience, but still allows 
	    // the shaders to be optimized and to run exactly the way they will run in 
	    // the release configuration of this program.
	    dwShaderFlags |= D3DCOMPILE_DEBUG;
	#endif
		TScopedResource<ID3DBlob> blobOut;
		TScopedResource<ID3DBlob> errorBlob;
		//シェーダーをロード。
		static char text[5 * 1024 * 1024];
		int fileSize = 0;
		ReadFile(filePath, text, fileSize);
		static const char* shaderModelNames[] = {
			"vs_5_0",
			"ps_5_0",
			"cs_5_0"
		};
		hr = D3DCompile(text, fileSize, nullptr, nullptr, nullptr, entryFuncName, 
			shaderModelNames[(int)shaderType], dwShaderFlags, 0, &blobOut.res, &errorBlob.res);
		
	    if( FAILED(hr) )
	    {
			if (errorBlob.res != nullptr) {
				OutputDebugStringA((char*)errorBlob.res->GetBufferPointer());
			}
	        return false;
	    }
		ID3D11Device* pD3DDevice = Engine().GetD3DDevice();
		switch(shaderType){
		case EnType::VS:{
			//頂点シェーダー。
			hr = pD3DDevice->CreateVertexShader(blobOut.res->GetBufferPointer(), blobOut.res->GetBufferSize(), nullptr, (ID3D11VertexShader**)&m_shader );
			if (FAILED(hr)) {
				return false;
			}
			//入力レイアウトを作成。
			//@todo入力レイアウトは頂点シェーダーごとに変更できるようにする必要があるが、今は決め打ち。
			// Define the input layout
			D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			UINT numElements = ARRAYSIZE(layout);
			hr = pD3DDevice->CreateInputLayout(layout, numElements, blobOut.res->GetBufferPointer(), blobOut.res->GetBufferSize(), &m_inputLayout);
			if (FAILED(hr)) {
				//入力レイアウトの作成に失敗した。
				return false;
			}
		}break;
		case EnType::PS:{
			//ピクセルシェーダー。
			hr = pD3DDevice->CreatePixelShader(blobOut.res->GetBufferPointer(), blobOut.res->GetBufferSize(), nullptr, (ID3D11PixelShader**)&m_shader);
			if (FAILED(hr)) {
				return false;
			}
		}break;
		case EnType::CS:{
			//コンピュートシェーダー。
			hr = pD3DDevice->CreateComputeShader(blobOut.res->GetBufferPointer(), blobOut.res->GetBufferSize(), nullptr, (ID3D11ComputeShader**)&m_shader);
			if (FAILED(hr)) {
				return false;
			}
		}break;
		}
	    return true;
	}
}