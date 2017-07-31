/*!
 *@brief	エンジン。
 */
#pragma once

#include "tkEngine2/graphics/tkRenderTarget.h"
#include "tkEngine2/timer/tkStopwatch.h"
namespace tkEngine2{
	/*!
	 *@brief	初期化用のパラメータ。
	 */
	struct SInitParam{
		SInitParam(){
			memset(this, 0, sizeof(SInitParam));
			gameObjectPrioMax = 32;
		}
		HINSTANCE hInstance;		//!<アプリケーションインスタンス。
		int	screenWidth;			//!<スクリーンの幅。
		int	screenHeight;			//!<スクリーンの高さ。
		int frameBufferHeight;		//!<フレームバッファの幅。これが内部解像度。
		int frameBufferWidth;		//!<フレームバッファの高さ。これが内部解像度。
		int nCmdShow;				//!<
		unsigned char gameObjectPrioMax;		//!<ゲームオブジェクトの優先度の最大値(32まで)
	};
	/*!
	 *@brief	エンジン。
	 */
	class CEngine : Noncopyable {
	private:
		/*!
		 *@brief	コンストラクタ。
		 */
		CEngine();
		/*!
		 *@brief	デストラクタ。
		 */
		~CEngine();
	public:
		/*!
		 *@brief	初期化。
		 *@param[in]	initParam		初期化パラメータ。
		 */
		bool Init( const SInitParam& initParam );
		/*!
		 *@brief	終了処理。
		 */
		void Final();
		/*!
		 *@brief	ゲームループを実行。
		 */
		void RunGameLoop();
		/*!
		 *@brief	インスタンスを取得。
		 */
		static CEngine& GetInstance()
		{
			static CEngine* instance = nullptr;
			if(instance == nullptr){
				instance = new CEngine;
			}
			return *instance;
		}
		/*!
		*@brief	Direct3DDeviceの取得。
		*/
		ID3D11Device* GetD3DDevice() const
		{
			return m_pd3dDevice;
		}
		/*!
		*@brief	ID3D11DeviceContextの取得。
		*/
		ID3D11DeviceContext* GetD3DDeviceContext() const
		{
			return m_pImmediateContext;
		}
		/*!
		*@brief		フレームバッファの幅を取得。
		*/
		int GetFrameBufferWidth() const
		{
			return m_frameBufferWidth;
		}
		/*!
		*@brief		フレームバッファの高さを取得。
		*/
		int GetFrameBufferHeight() const
		{
			return m_frameBufferHeight;
		}
		/*!
		* @brief	メインレンダリングターゲットを切り替え。
		*/
		void ToggleMainRenderTarget()
		{
			m_currentMainRenderTarget ^= 1;
		}
		/*!
		* @brief	メインレンダリングターゲットの取得。
		*/
		CRenderTarget& GetMainRenderTarget()
		{
			return m_mainRenderTarget[m_currentMainRenderTarget];
		}
		/*!
		* @brief	ゲームパッドの取得。
		*@param[in]	padNo	パッド番号
		*/
		CPad& GetPad(int padNo)
		{
			TK_ASSERT(padNo < CPad::CONNECT_PAD_MAX && padNo >= 0, "padNo is invalid");
			return m_pad[padNo];
		}
	private:
		/*!
		 *@brief	ウィンドウ初期化。
		 */
		bool InitWindow( const SInitParam& initParam );
		/*!
		 *@brief	DirectXを初期化。
		 */
		bool InitDirectX( const SInitParam& initParam );
		/*!
		* @brief	ウィンドウプロシージャ。
		*/
		static LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		/*!
		* @brief	更新。
		*/
		void Update();
		/*!
		* @brief	メインレンダリングターゲットの内容をバックバッファにコピー。
		*/
		void CopyMainRenderTargetToBackBuffer();
	private:
		HINSTANCE				m_hInst = nullptr;	//!<アプリケーションのインスタンス。
		HWND					m_hWnd = nullptr;	//!<ウィンドウハンドル。
		D3D_DRIVER_TYPE			m_driverType = D3D_DRIVER_TYPE_NULL; //!<ドライバの種類のオプション。
		D3D_FEATURE_LEVEL		m_featureLevel = D3D_FEATURE_LEVEL_11_0;	//!<Direct3D デバイスのターゲットとなる機能セット。
		ID3D11Device*			m_pd3dDevice = nullptr;						//!<D3D11デバイス。
		CRenderContext			m_renderContext;							//!<レンダリングコンテキスト。
		ID3D11DeviceContext*	m_pImmediateContext = nullptr;				//!<D3D11即時デバイスコンテキスト。
		IDXGISwapChain*			m_pSwapChain = nullptr;						//!<SwapChain。
		int						m_currentMainRenderTarget = 0;				//!<現在のメインレンダリングターゲット。
		CRenderTarget			m_mainRenderTarget[2];						//!<メインレンダリングターゲット。
		
		int						m_screenWidth = 0;							//!<スクリーンの幅。
		int						m_screenHeight = 0;							//!<スクリーンの高さ。
		int						m_frameBufferWidth = 0;						//!<フレームバッファの幅。これが内部解像度。
		int						m_frameBufferHeight = 0;					//!<フレームバッファの高さ。これが内部解像度。
		CPad					m_pad[CPad::CONNECT_PAD_MAX];				//!<ゲームパッド。
	public:
		CStopwatch				m_sw;
	};

	/*!
	* @brief	Engineのインスタンスを取得。。
	*/
	static inline CEngine& Engine()
	{
		return CEngine::GetInstance();
	}
	/*!
	* @brief	CPadのインスタンスを取得。。
	*/
	static inline CPad& Pad(int padNo)
	{
		return Engine().GetPad(padNo);
	}
}