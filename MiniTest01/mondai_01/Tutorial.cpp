/*!
 *@brief	小テスト01_00
 */
#include "stdafx.h"
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/timer/tkStopwatch.h"
#include <time.h>

using namespace tkEngine2;

class Mondai_01 : public IGameObject {
	//入力パラメータ。
	struct SInputParam {
		int score;			//!<得点。
	};
	static const int NUM_STUDENT = 30;		//生徒数。
public:
	Mondai_01()
	{

	}
	~Mondai_01()
	{

	}
	//初期化処理。
	bool Start() override
	{
		//あるクラスの点数をランダムに決定。
		SInputParam score[NUM_STUDENT];
		for (int i = 0; i < NUM_STUDENT; i++) {
			score[i].score = rand() % 100;
			TK_LOG("Score = %d\n", score[i].score);
		}
		return true;
	}
	void Update() override
	{
		//何も書かなくていいよ。
	}
	void Render(CRenderContext& renderContext)
	{
		//ここでコンピュートシェーダーをディスパッチして、結果をメッセージボックスで出すプログラムを記述する。。
		
		DeleteGO(this);	//終わり。
	}
};
/*!
 *@brief	メイン関数。
 */
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );
    //tkEngine2の初期化パラメータを設定する。
	SInitParam initParam;
	initParam.nCmdShow = nCmdShow;
	initParam.hInstance = hInstance;
	initParam.screenWidth = 1280;
	initParam.screenHeight = 720;
	initParam.frameBufferWidth = 1280;
	initParam.frameBufferHeight = 720;
	srand(time(NULL));
	//エンジンを初期化。
	if (Engine().Init(initParam) == true) {
		NewGO<Mondai_01>(0);
		//初期化に成功。
		//ゲームループを実行。
		Engine().RunGameLoop();
	}
	//エンジンの終了処理。
	Engine().Final();
	
    return 0;
}

