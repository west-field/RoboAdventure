#pragma once
#include "Secne.h"
#include <string>

class InputState;

class SoundSettingScene : public Scene
{
public:
	SoundSettingScene(SceneManager& manager,int soundH);
	~SoundSettingScene();

	void Update(const InputState& input);
	void Draw();
private:

	void WindowDraw();

	void NormalUpdate(const InputState& input);
	void BGMVolumeChange(const InputState& input);
	void SEVolumeChange(const InputState& input);

	void (SoundSettingScene::* m_updateFunc)(const InputState& input);

	static constexpr int pw_width = 450;
	static constexpr int pw_height = 350;
	static constexpr int pw_start_x = (640 - pw_width) / 2 + 50;
	static constexpr int pw_start_y = (480 - pw_height) / 2 + 50;

	static constexpr int kPosX = pw_start_x + 10;
	static constexpr int kPosY = pw_start_y + 30;
	enum SoundType
	{
		soundTypeBGM,//BGM
		soundTypeSE,//SE

		soundTypeBack,//閉じる

		soundTypeMax,
	};
	struct SoundVolume
	{
		int x, y;//座標
		int color;//色
		std::wstring name;//名前
		int nameNum;//文字数
		int fontSize;//フォントのサイズ
	};

	SoundVolume m_soundChange[soundTypeMax];

	int m_selectNum = 0;

	int m_waitInterval = 60;//値が変化するインターバル
	int m_puressTime = 0;//押しっぱなしの時間
	
	int m_soundH;//サウンドハンドル

	int m_bg;
};

