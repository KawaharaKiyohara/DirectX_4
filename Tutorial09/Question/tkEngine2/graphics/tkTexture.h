/*!
 * @brief	テクスチャ。
 */

#pragma once

namespace tkEngine2{
	class CTexture{
	public:
		CTexture();
		~CTexture();
		void Load( const wchar_t* filePath );
		void Release();
		CShaderResourceView& GetSRV()
		{
			return m_srv;
		}
	private:
		CShaderResourceView			m_srv;
		ID3D11ShaderResourceView*	m_srvRaw = nullptr;		//シェーダーリソースびゅ。
	};
}