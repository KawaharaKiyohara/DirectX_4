/*!
 * @brief	�e�N�X�`���f�[�^�B
 */

#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/graphics/tkTextureData.h"


namespace tkEngine2{
	/*!
	 *@brief	�e�N�X�`����ǂݍ��݁B
	 */
	bool CTextureData::Load(const LPCWSTR fileName)
	{
		const wchar_t* ext = wcsstr(fileName, L".");
		HRESULT hr = S_OK;
		if (wcscmp(ext, L".dds") == 0){
			//DDS�B
			 hr = DirectX::LoadFromDDSFile(fileName, 0, &m_texMetadata, m_scratchImage);
		}
		else if (wcscmp(ext, L".tga") == 0) {
			//TGA�B
			hr = DirectX::LoadFromTGAFile(fileName, &m_texMetadata, m_scratchImage);
		}
		else {
			//����ȊO�B
			hr = DirectX::LoadFromWICFile(fileName, 0, &m_texMetadata, m_scratchImage);
		}
		
		if (FAILED(hr)) {
			//�ǂݍ��ݎ��s�B
			char mFileName[256];
			wcstombs(mFileName, fileName, sizeof(mFileName)-1);
			static char errorMessage[1024];
			sprintf(errorMessage, "�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂����B%s\n", mFileName);
			TK_LOG("errorMessage");
			return false;
		}

		return true;
	}
}