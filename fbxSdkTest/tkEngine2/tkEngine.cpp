/*!
 *@brief	エンジン
 */
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/gameObject/tkGameObjectManager.h"
#include "tkEngine2/timer/tkStopwatch.h"

namespace tkEngine2 {
	CEngine::CEngine()
	{

	}
	CEngine::~CEngine()
	{
		Final();
	}
	bool CEngine::Init( const SInitParam& initParam )
	{
		//ウィンドウ初期化。
		if(!InitWindow(initParam)){
			return false;
		}
		//DirectX初期化。
		if(!InitDirectX(initParam)){
			return false;
		}
		//GameObjectManagerの初期化。
		GameObjectManager().Init(initParam.gameObjectPrioMax);
		return true;
	}
	bool CEngine::InitWindow( const SInitParam& initParam )
	{
		TK_ASSERT(initParam.screenHeight != 0, "screenHeight is zero");
		TK_ASSERT(initParam.screenWidth != 0, "screenWidth is zero");
		
		m_screenHeight = initParam.screenHeight;
		m_screenWidth = initParam.screenWidth;
		WNDCLASSEX wc =
		{
			sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
			GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr,
			TEXT("GameDemo"), nullptr
		};
		RegisterClassEx(&wc);
		// Create the application's window
		m_hWnd = CreateWindow(TEXT("GameDemo"), TEXT("GameDemo"),
			WS_OVERLAPPEDWINDOW, 0, 0, m_screenWidth, m_screenHeight,
			nullptr, nullptr, wc.hInstance, nullptr);

		ShowWindow(m_hWnd, initParam.nCmdShow);
		return m_hWnd != nullptr;
	}
	bool CEngine::InitDirectX( const SInitParam& initParam )
	{
		UINT createDeviceFlags = 0;
#ifdef _DEBUG
   		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_DRIVER_TYPE driverTypes[] =
	    {
	        D3D_DRIVER_TYPE_HARDWARE,
	        D3D_DRIVER_TYPE_WARP,
	        D3D_DRIVER_TYPE_REFERENCE,
	    };

	    D3D_FEATURE_LEVEL featureLevels[] =
	    {
	        D3D_FEATURE_LEVEL_11_0,
	        D3D_FEATURE_LEVEL_10_1,
	        D3D_FEATURE_LEVEL_10_0,
	    };
		UINT numFeatureLevels = ARRAYSIZE( featureLevels );
		
		m_frameBufferWidth = initParam.frameBufferWidth;
		m_frameBufferHeight = initParam.frameBufferHeight;
		//スワップチェインを作成。
		DXGI_SWAP_CHAIN_DESC sd;
	    ZeroMemory( &sd, sizeof( sd ) );
	    sd.BufferCount = 1;							//スワップチェインのバッファ数。通常は１。
	    sd.BufferDesc.Width = m_frameBufferWidth;	//フレームバッファの幅。
	    sd.BufferDesc.Height = m_frameBufferHeight;	//フレームバッファの高さ。
	    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//フレームバッファのフォーマット。R8G8B8A8の32bit。
	    sd.BufferDesc.RefreshRate.Numerator = 60;			//モニタのリフレッシュレート。(バックバッファとフロントバッファを入れ替えるタイミングとなる。)
	    sd.BufferDesc.RefreshRate.Denominator = 1;			//@todo なんやろこれ？たぶん２にしたら30fpsになる。あとで試す。
	    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//サーフェスまたはリソースを出力レンダー ターゲットとして使用します。
	    sd.OutputWindow = m_hWnd;
	    sd.SampleDesc.Count = 1;							//ピクセル単位のマルチサンプリングの数。MSAAはなし。
		sd.SampleDesc.Quality = 0;							//MSAAなし。
		sd.Windowed =  TRUE;
		
		//すべてのドライバタイプでスワップチェインの作成を試す。
		HRESULT hr = E_FAIL;
		for (auto driverType : driverTypes) {
			m_driverType = driverType;
			hr = D3D11CreateDeviceAndSwapChain(NULL, m_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
				D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice, &m_featureLevel, &m_pImmediateContext);
			if (SUCCEEDED(hr)) {
				//スワップチェインを作成できたのでループを抜ける。
				break;
			}
		}
		if (FAILED(hr)) {
			//スワップチェインを作成できなかった。
			return false;
		}
		
		//書き込み先になるレンダリングターゲットを作成。
		ID3D11Texture2D* pBackBuffer = NULL;
		hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (FAILED(hr)) {
			return false;
		}
		DXGI_SAMPLE_DESC multiSampleDesc;
		multiSampleDesc.Count = 1;
		multiSampleDesc.Quality = 0;
		bool ret = m_mainRenderTarget[0].Create(
			m_frameBufferWidth,
			m_frameBufferHeight,
			1,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			multiSampleDesc
		);
		ret = m_mainRenderTarget[1].Create(
			m_frameBufferWidth,
			m_frameBufferHeight,
			1,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			multiSampleDesc
		);
		if (!ret) {
			return false;
		}

		//レンダリングコンテキストの初期化。
		m_renderContext.Init(m_pImmediateContext);

		m_renderContext.OMSetRenderTargets(1, &m_mainRenderTarget[0]);

		//ビューポートを設定。
		m_renderContext.RSSetViewport(0.0f, 0.0f, (FLOAT)m_frameBufferWidth, (FLOAT)m_frameBufferHeight);

		return true;
	}
	void CEngine::CopyMainRenderTargetToBackBuffer()
	{
		ID3D11Texture2D* pBackBuffer = NULL;
		m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		m_pImmediateContext->CopyResource(
			pBackBuffer,
			m_mainRenderTarget[m_currentMainRenderTarget].GetRenderTarget()
		);
		pBackBuffer->Release();
	}
	void CEngine::Final()
	{
		if( m_pImmediateContext ){
			m_pImmediateContext->ClearState();
			m_pImmediateContext = nullptr;
		}
		m_mainRenderTarget[0].Release();
		m_mainRenderTarget[1].Release();
	    if( m_pSwapChain ){
			m_pSwapChain->Release();
			m_pSwapChain = nullptr;
		}
	    if( m_pImmediateContext ){
			m_pImmediateContext->Release();
			m_pImmediateContext = nullptr;
		}
	    if( m_pd3dDevice ){
			m_pd3dDevice->Release();
			m_pd3dDevice = nullptr;
		}
	}
	void CEngine::RunGameLoop()
	{
		MSG msg = {0};
	    while( WM_QUIT != msg.message )
	    {
	        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
	        {
	            TranslateMessage( &msg );
	            DispatchMessage( &msg );
	        }
	        else
	        {
				//更新。
				Update();
	        }
	    }
	}
	void CEngine::Update()
	{
		CStopwatch sw;
		sw.Start();
		CGameObjectManager& goMgr = GameObjectManager();
		goMgr.Execute(m_renderContext);
		//メインレンダリングターゲットの内容をバックバッファにコピー。
		CopyMainRenderTargetToBackBuffer();
		
		m_pSwapChain->Present(0, 0);
		sw.Stop();
		Log("1Frame time = %lf\n", sw.GetElapsed());
	}
	LRESULT CALLBACK CEngine::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		PAINTSTRUCT ps;
		HDC hdc;

		switch (msg)
		{
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}

		return 0;
	}
}