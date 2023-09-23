#include <DxLib.h>
#include "EffekseerForDxLib.h"

#include "game.h"
#include "Util/Sound.h"
#include "InputState.h"
#include "Scene/SceneManager.h"
#include "Scene/TitleScene.h"

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
	sceneManeger.ChangeScene(new TitleScene(sceneManeger, true));

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
#if true
	bool isTriggerWindouMode = false;//ALTとENTERを押しているか
	bool isWindouwMode = Game::kWindowMode;//ウィンドウモードを変更する
#endif

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
#if false
		if (CheckHitKey(KEY_INPUT_LALT))
		{
			if (CheckHitKey(KEY_INPUT_RETURN))
			{
				if (!isTriggerWindouMode)
				{
					isWindouwMode = !isWindouwMode;
					ChangeWindowMode(isWindouwMode);
					sceneManeger.SetIsWindouMode(isWindouwMode);
					SetDrawScreen(DX_SCREEN_BACK);//描画先を再定義
				}
				isTriggerWindouMode = true;
			}
			else
			{
				SetDrawScreen(DX_SCREEN_BACK);//描画先を再定義
				isTriggerWindouMode = false;
			}
		}
		if (CheckHitKey(KEY_INPUT_F4))
		{
			if (!isTriggerWindouMode)
			{
				isWindouwMode = !isWindouwMode;
				ChangeWindowMode(isWindouwMode);
				sceneManeger.SetIsWindouMode(isWindouwMode);
			}
			SetDrawScreen(DX_SCREEN_BACK);//描画先を再定義
		}
#endif

#if false
		// XYZ軸
		float lineSize = 100.0f;
		DrawLine3D(VGet(-lineSize, 0, 0), VGet(lineSize, 0, 0), GetColor(255, 0, 0));
		DrawLine3D(VGet(0, -lineSize, 0), VGet(0, lineSize, 0), GetColor(0, 255, 0));
		DrawLine3D(VGet(0, 0, -lineSize), VGet(0, 0, lineSize), GetColor(0, 0, 255));

		//// マップチップライン(マップチップをどう配置するか)
		//// プレイヤーの地面をY=0としたいので、その周りを配置し、大体の基準でカメラを決める
		//for (int i = 0; i < 16 + 2; i++)
		//{
		//	// X軸とかぶるところはとりあえず描画しない
		//	if (i != 1)
		//	{
		//		float y = 32 * (i - 1); // 一個下のラインからチップが始まる
		//		DrawLine3D(VGet(-lineSize, y, 0), VGet(lineSize, y, 0), GetColor(255, 255, 0));
		//	}
		//}
		//for (int i = 0; i < 15 + 1; i++)
		//{
		//	// X軸とかぶるところはとりあえず描画しない
		//	if (i != 1)
		//	{
		//		float x = 32 * (i - 1); // 一個下のラインからチップが始まる
		//		DrawLine3D(VGet(x, -100, 0), VGet(x, 100, 0), GetColor(255, 255, 255));
		//	}
		//}
#endif

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

	//エフェクシアを終了する
	Effkseer_End();
	//Dxライブラリを終了する
	DxLib_End();

	return 0;
}
