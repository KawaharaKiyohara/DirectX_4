/*!
 * @brief	�萔�o�b�t�@
 */

#pragma once

namespace tkEngine2{
	/*!
	 * @brief	�萔�o�b�t�@�B
	 */
	class CConstantBuffer{
	public:
		CConstantBuffer();
		~CConstantBuffer();
		/*!
		*@brief ID3D11Buffer*���擾�B
		*/
		ID3D11Buffer* GetBody()
		{
			return m_constantBuffer;
		}
	public:
		ID3D11Buffer*	m_constantBuffer;		//!<�萔�o�b�t�@�B
	};
}