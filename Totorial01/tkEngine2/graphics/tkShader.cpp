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
		/*!
		 *@brief	頂点シェーダーから頂点レイアウトを生成。
		 */
		HRESULT CreateInputLayoutDescFromVertexShaderSignature(ID3DBlob* pShaderBlob, ID3D11Device* pD3DDevice, ID3D11InputLayout** pInputLayout)
		{
			// シェーダー情報からリフレクションを行う。
			ID3D11ShaderReflection* pVertexShaderReflection = NULL;
			if (FAILED(D3DReflect(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pVertexShaderReflection)))
			{
				return S_FALSE;
			}

			// シェーダー情報を取得。
			D3D11_SHADER_DESC shaderDesc;
			pVertexShaderReflection->GetDesc(&shaderDesc);

			// 入力情報定義を読み込み
			std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
			for (unsigned int i = 0; i< shaderDesc.InputParameters; i++)
			{
				D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
				pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc);

				// エレメント定義を設定。
				D3D11_INPUT_ELEMENT_DESC elementDesc;
				elementDesc.SemanticName = paramDesc.SemanticName;
				elementDesc.SemanticIndex = paramDesc.SemanticIndex;
				elementDesc.InputSlot = 0;
				elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
				elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				elementDesc.InstanceDataStepRate = 0;

				// determine DXGI format
				if (paramDesc.Mask == 1)
				{
					if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
					else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
					else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
				}
				else if (paramDesc.Mask <= 3)
				{
					if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
					else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
					else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
				}
				else if (paramDesc.Mask <= 7)
				{
					if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
					else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
					else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
				}
				else if (paramDesc.Mask <= 15)
				{
					if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
					else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
					else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				}

				//エレメントの定義を保存。
				inputLayoutDesc.push_back(elementDesc);
			}

			// 入力レイアウトを作成。
			HRESULT hr = pD3DDevice->CreateInputLayout(&inputLayoutDesc[0], inputLayoutDesc.size(), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pInputLayout);

			//リフレクション用に確保したメモリを解放。
			pVertexShaderReflection->Release();
			return hr;
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
				static char errorMessage[10 * 1024];
				sprintf(errorMessage, "filePath : %s, %s", filePath, errorBlob.res->GetBufferPointer());
				MessageBox(NULL, errorMessage, "シェーダーコンパイルエラー", MB_OK);
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
			hr = CreateInputLayoutDescFromVertexShaderSignature(blobOut.res, pD3DDevice, &m_inputLayout);
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