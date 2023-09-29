#include <DxLib.h>
#include "EffekseerForDxLib.h"

#include "game.h"
#include "Util/Sound.h"
#include "InputState.h"
#include "Scene/SceneManager.h"
#include "Scene/TitleScene.h"
#include "Scene/DebugScene.h"

//int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	SetUseDirectDrawDeviceIndex(1);
	// windowモード設定
	ChangeWindowMode(Game::kWindowMode);
	// ウインドウ名設定
	SetMainWindowText(Game::kTitleText);
	// 画面サイズの設定
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorDepth);

	//DxLibの初期化
	if (DxLib_Init() == -1)
	{
		return -1;
	}
	//エフェクシアの初期化
	if (Effekseer_Init(8000) == -1)
	{
		return -1;
	}
	// フルスクリーンウインドウの切り替えでリソースが消えるのを防ぐ。
	SetChangeScreenModeGraphicsSystemResetFlag(false);//(Dxlib
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();//(Effekseer
	//描画先を裏画面に変更する。
	SetDrawScreen(DX_SCREEN_BACK);

	//Zバッファを有効にする
	SetUseZBuffer3D(true);//前後表示を有効にする
	//Zバッファに書き込みを有効にする
	SetWriteZBuffer3D(true);
	//裏面ポリゴンを表示しない
	SetUseBackCulling(true);

	auto& soundManager = SoundManager::GetInstance();
	InputState input;
	SceneManager sceneManeger;
#if _DEBUG
	sceneManeger.ChangeScene(new DebugScene(sceneManeger));
#else
	sceneManeger.ChangeScene(new TitleScene(sceneManeger, true));
#endif
	sceneManeger.SetIsWindouMode(Game::kWindowMode);

	//奥行0.1～1000までをカメラの描画範囲とする
	SetCameraNearFar(1.0f, 1000.0f);
	// カメラの位置
	SetCameraPositionAndTarget_UpVecY(VGet(10.4f, 3.0f, -10.0f), VGet(10.4f, 3.0f, -9.0f));
	// 標準ライトをポイントライトにする
	ChangeLightTypePoint(VGet(0.0f, 0.0f, 0.0f), 2000.0f, 0.0f, 0.002f, 0.0f);
	// 標準ライトの方向をz軸のプラス方向にする
	SetLightDirection(VGet(0.0f, -10.0f, 1.0f));
	//ライトの位置
	SetLightPosition(VAdd(VGet(10.0f, 3.0f, -10.0f), VGet(0.0f, 200.0f, -100.0f)));

	while (ProcessMessage() != -1)
	{
		LONGLONG  time = GetNowHiPerformanceCount();

		// DXライブラリの設定とEffekseerの設定を同期する。(基本カメラ)(Effekseer)
		Effekseer_Sync3DSetting();

		input.Update();
		sceneManeger.Update(input);

		UpdateEffekseer3D();

		// 画面を初期化する
		ClearDrawScreen();

		sceneManeger.Draw();

		// Effekseerにより再生中のエフェクトを描画する。
		DrawEffekseer3D();
		//裏画面を表画面を入れ替える
		ScreenFlip();

		// escキーを押したら終了する
		//if (CheckHitKey(KEY_INPUT_ESCAPE))	break;
		//isEndがtrueのとき終了する
		if (sceneManeger.GetIsEnd())	break;
		// fpsを60に固定
		while (GetNowHiPerformanceCount() - time < 16667){}
	}

	soundManager.DeleteSound();

	//エフェクシアを終了する
	Effkseer_End();
	//Dxライブラリを終了する
	DxLib_End();

	return 0;
}
