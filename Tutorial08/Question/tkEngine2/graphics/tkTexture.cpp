#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/graphics/tkTexture.h"

namespace tkEngine2 {
	CTexture::CTexture()
	{
	}
	CTexture::~CTexture()
	{
		Release();
	}
	void CTexture::Load(const wchar_t* filePath)
	{
		wchar_t ext[_MAX_EXT];
		_wsplitpath_s(filePath, nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

		if (_wcsicmp(ext, L".dds") == 0) {
			//ddsƒtƒ@ƒCƒ‹
		/*	HRESULT hr = DirectX::CreateDDSTextureFromFileEx(
				Engine().GetD3DDevice(), filePath, 0,
				D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
				false, nullptr, m_srv);*/
			
		}
	}
	void CTexture::Release()
	{
	}
}