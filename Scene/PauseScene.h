#pragma once
#include "Secne.h"
#include "../game.h"
#include "../Util/Info.h"

class InputState;
class SceneManager;
/// <summary>
/// ポーズシーン
/// </summary>
class PauseScene : public Scene
{
public:
	PauseScene(SceneManager& manager, int soundH);
	virtual ~PauseScene();

	virtual void Update(const InputState& input) override;
	void Draw();
private:
	void WindowDraw();

	enum Item
	{
		pauseKeyconfig,//キー設定
		pauseSound,//音量設定
		//pauseFullscreen,//画面表示を切り替える
		pauseBack,//閉じる
		pauseTitle,//タイトルに戻る
		pauseGameEnd,//ゲームを終わる

		pauseMax
	};

	Element m_pauseMenu[Item::pauseMax];
	int m_selectNum;

	int m_soundH;

	int m_bg;
};

