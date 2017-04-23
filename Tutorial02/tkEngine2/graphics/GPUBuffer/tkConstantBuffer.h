/*!
 * @brief	定数バッファ
 */

#pragma once

namespace tkEngine2{
	/*!
	 * @brief	定数バッファ。
	 */
	class CConstantBuffer{
	public:
		CConstantBuffer();
		~CConstantBuffer();
		/*!
		*@brief ID3D11Buffer*を取得。
		*/
		ID3D11Buffer* GetBody()
		{
			return m_constantBuffer;
		}
	public:
		ID3D11Buffer*	m_constantBuffer;		//!<定数バッファ。
	};
}