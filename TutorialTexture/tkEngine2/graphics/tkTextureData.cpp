/*!
 * @brief	テクスチャデータ。
 */

#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/graphics/tkTextureData.h"


namespace tkEngine2{
	/*!
	 *@brief	テクスチャを読み込み。
	 */
	bool CTextureData::Load(const LPCWSTR fileName)
	{
		const wchar_t* ext = wcsstr(fileName, L".");
		HRESULT hr = S_OK;
		if (wcscmp(ext, L".dds") == 0){
			//DDS。
			 hr = DirectX::LoadFromDDSFile(fileName, 0, &m_texMetadata, m_scratchImage);
		}
		else if (wcscmp(ext, L".tga") == 0) {
			//TGA。
			hr = DirectX::LoadFromTGAFile(fileName, &m_texMetadata, m_scratchImage);
		}
		else {
			//それ以外。
			hr = DirectX::LoadFromWICFile(fileName, 0, &m_texMetadata, m_scratchImage);
		}
		
		if (FAILED(hr)) {
			//読み込み失敗。
			char mFileName[256];
			wcstombs(mFileName, fileName, sizeof(mFileName)-1);
			static char errorMessage[1024];
			sprintf(errorMessage, "テクスチャの読み込みに失敗しました。%s\n", mFileName);
			TK_LOG("errorMessage");
			return false;
		}

		return true;
	}
}