#pragma once
#include "Secne.h"
#include "../game.h"
#include "../Util/Info.h"

class InputState;
class SceneManager;
/// <summary>
/// 設定シーン
/// </summary>
class SettingScene : public Scene
{
public:
	SettingScene(SceneManager& manager, int soundH);
	virtual ~SettingScene();

	virtual void Update(const InputState& input) override;
	void Draw();
private:
	void WindowDraw();

	enum Item
	{
		pauseKeyconfig,//キー設定
		pauseSound,//音量設定
		pauseFullScene,//スクリーンモード変更
		pauseBack,//閉じる
		pauseGameEnd,//ゲームを終わる

		pauseMax
	};

	Element m_pauseMenu[pauseMax];

	int m_selectNum = 0;

	int m_soundH;

	int m_bg;
};

