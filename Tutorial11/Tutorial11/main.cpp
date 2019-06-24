#include "stdafx.h"
#include "Effect.h"
#include "Stopwatch.h"
#include <thread>

///////////////////////////////////////////////////////////////////
// グローバル変数。
///////////////////////////////////////////////////////////////////

HWND					g_hWnd = NULL;	
ID3D11Device*			g_pd3dDevice = NULL;
IDXGISwapChain*			g_pSwapChain = NULL;	
ID3D11DeviceContext* g_pd3dImmidiateContext = nullptr;					//①　即時コンテキスト。
ID3D11DeviceContext* g_pd3dDiferredContext[NUM_THREAD] = { nullptr };	//②　これがDiferredContextコンテキスト。

ID3D11RenderTargetView* g_backBuffer = NULL;
ID3D11RasterizerState*	g_rasterizerState = NULL;

///////////////////////////////////////////////////////////////////
//ここから三角形ポリゴン描画関係の変数など。
///////////////////////////////////////////////////////////////////
ID3D11Buffer* g_vertexBuffer = NULL;		//頂点バッファ。
Effect g_effect;							//エフェクト。気にしなくてよい。

//頂点構造体。
struct SVertex {
	float position[4];	
};

///////////////////////////////////////////////////////////////////
// DirectXの初期化。
///////////////////////////////////////////////////////////////////
void InitDirectX()
{
	//スワップチェインを作成するための情報を設定する。
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;									//スワップチェインのバッファ数。通常は１。
	sd.BufferDesc.Width = 500;							//フレームバッファの幅。
	sd.BufferDesc.Height = 500;							//フレームバッファの高さ。
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//フレームバッファのフォーマット。R8G8B8A8の32bit。
	sd.BufferDesc.RefreshRate.Numerator = 60;			//モニタのリフレッシュレート。(バックバッファとフロントバッファを入れ替えるタイミングとなる。)
	sd.BufferDesc.RefreshRate.Denominator = 1;			//２にしたら30fpsになる。1でいい。
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//サーフェスまたはリソースを出力レンダー ターゲットとして使用します。
	sd.OutputWindow = g_hWnd;							//出力先のウィンドウハンドル。
	sd.SampleDesc.Count = 1;							//1でいい。
	sd.SampleDesc.Quality = 0;							//MSAAなし。0でいい。
	sd.Windowed = TRUE;									//ウィンドウモード。TRUEでよい。

	//利用するDirectXの機能セット。DirectX10以上に対応しているGPUを利用可能とする。
	//この配列はD3D11CreateDeviceAndSwapChainの引数として使う。
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	
	D3D_FEATURE_LEVEL featureLevel;
	//D3Dデバイスとスワップチェインを作成する。
	D3D11CreateDeviceAndSwapChain(
		NULL,											//NULLでいい。
		D3D_DRIVER_TYPE_HARDWARE,						//D3Dデバイスがアクセスするドライバーの種類。
														//基本的にD3D_DRIVER_TYPE_HARDWAREを指定すればよい。
		NULL,											//NULLでいい。
		0,												//０でいい。
		featureLevels,									//D3Dデバイスのターゲットとなる機能セットを指定する。
														//今回のサンプルはDirectX10以上をサポートするので、
														//それらを含むD3D_FEATURE_LEVELの配列を渡す。
		sizeof(featureLevels)/sizeof(featureLevels[0]),	//機能セットの数。
		D3D11_SDK_VERSION,								//使用するDirectXのバージョン。
														//D3D11_SDK_VERSIONを指定すればよい。
		&sd,											//スワップチェインを作成するための情報。
		&g_pSwapChain,									//作成したスワップチェインのアドレスの格納先。
		&g_pd3dDevice,									//作成したD3Dデバイスのアドレスの格納先。
		&featureLevel,									//使用される機能セットの格納先。
		&g_pd3dImmidiateContext							//③　即時コンテキストを作成している。
	);
	D3D11_FEATURE_DATA_THREADING fd;
	//デバイスのマルチスレッドサポートの機能を調べる。
	g_pd3dDevice->CheckFeatureSupport(
		D3D11_FEATURE_THREADING,
		&fd,
		sizeof(fd)
	);
	//④　ディファードコンテキストの作成。
	//デバイスが複数のコンテキストを作成できるか調べる。
	if (fd.DriverCommandLists == TRUE) {
		//NVIDIAのグラフィックボードのみディファードコンテキストに対応している。
		//ディファードコンテキストを作成。
		for (auto& deviceContext : g_pd3dDiferredContext) {
			g_pd3dDevice->CreateDeferredContext(0, &deviceContext);
		}
	}

	//書き込み先になるレンダリングターゲットを作成。
	ID3D11Texture2D* pBackBuffer = nullptr;
	g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (pBackBuffer != nullptr) {
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_backBuffer);
		pBackBuffer->Release();
	}
	D3D11_RASTERIZER_DESC desc = {};
	desc.CullMode = D3D11_CULL_FRONT;
	desc.FillMode = D3D11_FILL_SOLID;
	desc.DepthClipEnable = true;
	desc.MultisampleEnable = true;

	//ラスタライザとビューポートを初期化。
	g_pd3dDevice->CreateRasterizerState(&desc, &g_rasterizerState);

	
}
///////////////////////////////////////////////////////////////////
// DirectXの終了処理。
///////////////////////////////////////////////////////////////////
void ReleaseDirectX()
{
	if (g_vertexBuffer != NULL) {
		g_vertexBuffer->Release();
	}
	if (g_rasterizerState != NULL) {
		g_rasterizerState->Release();
	}
	if (g_backBuffer != NULL) {
		g_backBuffer->Release();
	}
	if (g_pSwapChain != NULL) {
		g_pSwapChain->Release();
	}
	for (auto& context : g_pd3dDiferredContext) {
		if (context != NULL) {
			context->Release();
		}
	}
	if (g_pd3dImmidiateContext != NULL) {
		g_pd3dImmidiateContext->Release();
	}
	if (g_pd3dDevice != NULL) {
		g_pd3dDevice->Release();
	}
	
}
///////////////////////////////////////////////////////////////////
//メッセージプロシージャ。
//hWndがメッセージを送ってきたウィンドウのハンドル。
//msgがメッセージの種類。
//wParamとlParamは引数。今は気にしなくてよい。
///////////////////////////////////////////////////////////////////
LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//送られてきたメッセージで処理を分岐させる。
	switch (msg)
	{
	case WM_DESTROY:
		ReleaseDirectX();
		PostQuitMessage(0);
		break;	
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

///////////////////////////////////////////////////////////////////
// ウィンドウの初期化。
///////////////////////////////////////////////////////////////////
void InitWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//ウィンドウクラスのパラメータを設定(単なる構造体の変数の初期化です。)
	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX),		//構造体のサイズ。
		CS_CLASSDC,				//ウィンドウのスタイル。
								//ここの指定でスクロールバーをつけたりできるが、ゲームでは不要なのでCS_CLASSDCでよい。
		MsgProc,				//メッセージプロシージャ(後述)
		0,						//0でいい。
		0,						//0でいい。
		GetModuleHandle(NULL),	//このクラスのためのウインドウプロシージャがあるインスタンスハンドル。
								//何も気にしなくてよい。
		NULL,					//アイコンのハンドル。アイコンを変えたい場合ここを変更する。とりあえずこれでいい。
		NULL,					//マウスカーソルのハンドル。NULLの場合はデフォルト。
		NULL,					//ウィンドウの背景色。NULLの場合はデフォルト。
		NULL,					//メニュー名。NULLでいい。
		TEXT("Sample_01"),		//ウィンドウクラスに付ける名前。
		NULL					//NULLでいい。
	};
	//ウィンドウクラスの登録。
	RegisterClassEx(&wc);

	// ウィンドウの作成。
	g_hWnd = CreateWindow(
		TEXT("Sample_01"),		//使用するウィンドウクラスの名前。
								//先ほど作成したウィンドウクラスと同じ名前にする。
		TEXT("Sample_01"),		//ウィンドウの名前。ウィンドウクラスの名前と別名でもよい。
		WS_OVERLAPPEDWINDOW,	//ウィンドウスタイル。ゲームでは基本的にWS_OVERLAPPEDWINDOWでいい、
		0,						//ウィンドウの初期X座標。
		0,						//ウィンドウの初期Y座標。
		500,					//ウィンドウの幅。
		500,					//ウィンドウの高さ。
		NULL,					//親ウィンドウ。ゲームでは基本的にNULLでいい。
		NULL,					//メニュー。今はNULLでいい。
		hInstance,				//アプリケーションのインスタンス。
		NULL
	);

	ShowWindow(g_hWnd, nCmdShow);

}
///////////////////////////////////////////////////////////////////
// 三角形ポリゴンの初期化。
///////////////////////////////////////////////////////////////////
void InitTrianglePolygon()
{
	//3頂点を定義する。これが頂点座標
	SVertex vertex[3] = {
		{ 0.5f,  -0.5f, 0.0f, 1.0f },	//頂点１
		{  0.0f,  0.5f, 0.0f, 1.0f },	//頂点２
		{ -0.5f, -0.5f, 0.0f, 1.0f },	//頂点３
	};
	//上で定義した頂点を使用して頂点バッファを作成する。
	//頂点バッファを作成するためにはD3D11_BUFFER_DESCとD3D11_SUBRESOURCE_DATAを設定する必要がある。
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));				//構造体を0で初期化する。
	bd.Usage = D3D11_USAGE_DEFAULT;				//バッファーで想定されている読み込みおよび書き込みの方法。
												//取りあえずはD3D11_USAGE_DEFAULTでよい。
	bd.ByteWidth = sizeof(vertex);				//頂点バッファのサイズ。頂点のサイズ×頂点数となる。
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	//これから作成するバッファが頂点バッファであることを指定する。

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertex;

	//頂点バッファの作成。
	g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_vertexBuffer);

	//シェーダーをロード。
	g_effect.Load("Assets/shader/primitive.fx");

}

//描画方法。０は即時コンテキスト、１はディファードコンテキスト、２はディファードコンテキストを使った並列描画。
#define DRAE_TYPE	2
//100万のドローコール( ´艸｀)
const int NUM_DRAW_CALL = 1000000;
///////////////////////////////////////////////////////////////////
// 描画処理
///////////////////////////////////////////////////////////////////
void GameRender()
{
	CStopwatch sw;
	sw.Start();

	D3D11_VIEWPORT viewport;
	viewport.Width = 500;
	viewport.Height = 500;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	static bool isCreateCommandList = false;
	
#if DRAE_TYPE == 0
	g_pd3dImmidiateContext->RSSetViewports(1, &viewport);
	g_pd3dImmidiateContext->RSSetState(g_rasterizerState);

	float ClearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; //red,green,blue,alpha
	//描き込み先をバックバッファにする。
	g_pd3dImmidiateContext->OMSetRenderTargets(1, &g_backBuffer, NULL);
	//バックバッファを灰色で塗りつぶす。
	g_pd3dImmidiateContext->ClearRenderTargetView(g_backBuffer, ClearColor);

	/////////////////////////////////////////////////
	// ここに3Dモデルなどを描画するコードを書いていく。
	/////////////////////////////////////////////////
	unsigned int vertexSize = sizeof(SVertex);	//頂点のサイズ。
	unsigned int offset = 0;

	//描画開始。

	//０番目のコンテキスト。
	g_effect.BeginRender(g_pd3dImmidiateContext);
	for (int i = 0; i < NUM_DRAW_CALL; i++) {

		g_pd3dImmidiateContext->IASetVertexBuffers(	//頂点バッファを設定。
			0,					//StartSlot番号。今は0でいい。
			1,					//バッファの数。今は1でいい。
			&g_vertexBuffer,	//頂点バッファ。
			&vertexSize,		//頂点のサイズ。
			&offset				//気にしなくてよい。
		);
		//プリミティブのトポロジーを設定する。(詳しくはLesson_3で解説を行う。)
		g_pd3dImmidiateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		g_pd3dImmidiateContext->Draw(	//描画命令。
			3,						//頂点数。
			0						//開始頂点番号。
		);
	}
#elif DRAE_TYPE == 1 //ディファードコンテキスト
	
	g_pd3dDiferredContext[0]->RSSetViewports(1, &viewport);
	g_pd3dDiferredContext[0]->RSSetState(g_rasterizerState);

	float ClearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; //red,green,blue,alpha
	//描き込み先をバックバッファにする。
	g_pd3dDiferredContext[0]->OMSetRenderTargets(1, &g_backBuffer, NULL);
	//バックバッファを灰色で塗りつぶす。
	g_pd3dDiferredContext[0]->ClearRenderTargetView(g_backBuffer, ClearColor);

	/////////////////////////////////////////////////
	// ここに3Dモデルなどを描画するコードを書いていく。
	/////////////////////////////////////////////////
	unsigned int vertexSize = sizeof(SVertex);	//頂点のサイズ。
	unsigned int offset = 0;

	int numPolyOneContext = NUM_DRAW_CALL / NUM_THREAD;
	//NUM_THREAD個のディファードコンテキストにドローコール。
	for (auto& deviceContext : g_pd3dDiferredContext) {
		g_effect.BeginRender(deviceContext);
		for (int i = 0; i < numPolyOneContext; i++) {

			deviceContext->IASetVertexBuffers(	//頂点バッファを設定。
				0,					//StartSlot番号。今は0でいい。
				1,					//バッファの数。今は1でいい。
				&g_vertexBuffer,	//頂点バッファ。
				&vertexSize,		//頂点のサイズ。
				&offset				//気にしなくてよい。
			);
			//プリミティブのトポロジーを設定する。(詳しくはLesson_3で解説を行う。)
			deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			deviceContext->Draw(	//描画命令。
				3,						//頂点数。
				0						//開始頂点番号。
			);
		}
	}
		
	//⑨　各コンテキストからコマンドリストを作成して、それを描画していく。
	ID3D11CommandList* cc = nullptr;
	for (auto& deviceContext : g_pd3dDiferredContext) {
		deviceContext->FinishCommandList(false, &cc);
		if (cc != nullptr) {
			g_pd3dImmidiateContext->ExecuteCommandList(cc, false);
			cc->Release();
		}
	}
#elif DRAE_TYPE == 2

	unsigned int vertexSize = sizeof(SVertex);	//頂点のサイズ。
	unsigned int offset = 0;

	g_pd3dDiferredContext[0]->RSSetViewports(1, &viewport);
	g_pd3dDiferredContext[0]->RSSetState(g_rasterizerState);

	float ClearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; //red,green,blue,alpha
	//描き込み先をバックバッファにする。
	g_pd3dDiferredContext[0]->OMSetRenderTargets(1, &g_backBuffer, NULL);
	//バックバッファを灰色で塗りつぶす。
	g_pd3dDiferredContext[0]->ClearRenderTargetView(g_backBuffer, ClearColor);
	
	std::thread th[NUM_THREAD];
	for (int threadNo = 0; threadNo < NUM_THREAD; threadNo++) {
		//⑤　０番目のコンテキストに対してドローコールを行う。
		th[threadNo] = std::thread([&, threadNo] {
			g_effect.BeginRender(g_pd3dDiferredContext[threadNo]);
			for (int i = 0; i < 250000; i++) {

				g_pd3dDiferredContext[threadNo]->IASetVertexBuffers(	//頂点バッファを設定。
					0,					//StartSlot番号。今は0でいい。
					1,					//バッファの数。今は1でいい。
					&g_vertexBuffer,	//頂点バッファ。
					&vertexSize,		//頂点のサイズ。
					&offset				//気にしなくてよい。
				);
				//プリミティブのトポロジーを設定する。(詳しくはLesson_3で解説を行う。)
				g_pd3dDiferredContext[threadNo]->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				g_pd3dDiferredContext[threadNo]->Draw(	//描画命令。
					3,						//頂点数。
					0						//開始頂点番号。
				);
			}});
	}
	//じょいーん
	for (auto& t : th) {
		t.join();
	}
	//⑨　各コンテキストからコマンドリストを作成して、それを描画していく。
	ID3D11CommandList* cc = nullptr;
	for (auto& deviceContext : g_pd3dDiferredContext) {
		deviceContext->FinishCommandList(false, &cc);
		if (cc != nullptr) {
			g_pd3dImmidiateContext->ExecuteCommandList(cc, false);
			cc->Release();
		}
	}

#endif
		
	sw.Stop();
	
	printf("GameRender Time = %f\n", sw.GetElapsedMillisecond());

	//バックバッファとフロントバッファを入れ替える。
	g_pSwapChain->Present(1, 0);

}
///////////////////////////////////////////////////////////////////
// ウィンドウプログラムのメイン関数。
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	AllocConsole();
	freopen("CON", "r", stdin);     // 標準入力の割り当て
	freopen("CON", "w", stdout);    // 標準出力の割り当て
	//ウィンドウの初期化。
	InitWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	//DirectXの初期化。
	InitDirectX();
	//３角形ポリゴンの初期化。
	InitTrianglePolygon();
	//メッセージ構造体の変数msgを初期化。
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)	//メッセージループ
	{
		//ウィンドウからのメッセージを受け取る。
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			//
			GameRender();
		}
	}
	FreeConsole();
}