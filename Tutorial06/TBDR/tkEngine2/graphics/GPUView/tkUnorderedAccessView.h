/*!
 *@brief	UnorderedAccessView
 */

#pragma once

namespace tkEngine2{
	class CStructuredBuffer;
	/*!
	 *@brief	UnorderedAccessView
	 *@details
	 * コンピュートシェーダーとピクセルシェーダーの出力に使用するビュー。
	 * 出力バッファではこれを使用する。
	 */
	class CUnorderedAccessView{
	public:
		CUnorderedAccessView();
		~CUnorderedAccessView();
		/*!
		 *@brief	StructuredBuffer用のUAVを作成。
		 *@param[in]	structuredBuffer	structuredBuffer
		 */
		bool Create(CStructuredBuffer& structuredBuffer);
		/*!
		*@brief	Texture用のUAVを作成。
		*@param[in]	texture		テクスチャ。
		*/
		bool Create(ID3D11Texture2D* texture);
		/*!
		 *@brief	明示的な開放。
		 *@details
		 * 明示的に開放する必要があるときに使用してください。
		 */
		void Release();
		/*!
		*@brief	ID3D11UnorderedAccessViewを取得。
		*/
		ID3D11UnorderedAccessView*& GetBody()
		{
			return m_uav;
		}
	private:
		ID3D11UnorderedAccessView*	m_uav = nullptr;
	};
}