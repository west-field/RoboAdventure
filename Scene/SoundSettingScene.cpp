#include "SoundSettingScene.h"
#include <Dxlib.h>
#include "../InputState.h"
#include "../Util/Sound.h"
#include "../Util/DrawFunctions.h"
#include "SceneManager.h"
#include "../Util/Info.h"

namespace
{
	constexpr int kFontSize = 20;

	constexpr int pw_width = 450;
	constexpr int pw_height = 350;
	constexpr int pw_start_x = (Game::kScreenWidth - pw_width) / 2;
	constexpr int pw_start_y = (Game::kScreenHeight - pw_height) / 2;
	constexpr int kPosX = pw_start_x + 10;
	constexpr int kPosY = pw_start_y + 30;
}

SoundSettingScene::SoundSettingScene(SceneManager& manager,int soundH) :Scene(manager), m_soundH(soundH), m_updateFunc(&SoundSettingScene::NormalUpdate)
{
	m_selectNum = static_cast<int>(SoundType::soundTypeBack);

	m_soundChange[static_cast<int>(SoundType::soundTypeBGM)].y = kPosY + kFontSize*2 * (static_cast<int>(SoundType::soundTypeBGM) + 1);
	m_soundChange[static_cast<int>(SoundType::soundTypeBGM)].name = L"BGM Volume = %d";

	m_soundChange[static_cast<int>(SoundType::soundTypeSE)].y = kPosY + kFontSize*2 * (static_cast<int>(SoundType::soundTypeSE) + 2);
	m_soundChange[static_cast<int>(SoundType::soundTypeSE)].name = L"SE Volume = %d";

	m_soundChange[static_cast<int>(SoundType::soundTypeBack)].y = kPosY + kFontSize*2 * (static_cast<int>(SoundType::soundTypeBack) + 3);
	m_soundChange[static_cast<int>(SoundType::soundTypeBack)].name = L"閉じる";

	for (auto& sound : m_soundChange)
	{
		sound.x = kPosX + kFontSize;
		sound.color = PushSceneWindow::kNormalColor;
		sound.nameNum = static_cast<int>(sound.name.size());
		sound.fontSize = kFontSize;
	}
	m_soundChange[m_selectNum].color = PushSceneWindow::kSelectColor;
	m_soundChange[m_selectNum].fontSize = kFontSize * 2;
	//ウィンドウ背景
	m_bg = my::MyLoadGraph(L"Data/Img/panel_Example1.png");
}

SoundSettingScene::~SoundSettingScene()
{
	DeleteGraph(m_bg);
}

void SoundSettingScene::Update(const InputState& input)
{
	(this->*m_updateFunc)(input);
}

void SoundSettingScene::Draw()
{
	WindowDraw();
	//サウンドセッティング中メッセージ
	SetFontSize(kFontSize * 2);
	DrawString(pw_start_x + kFontSize, pw_start_y + kFontSize, L"SoundSetting...", 0x000000);
	//BGMの設定
	auto& soundMgr = SoundManager::GetInstance();
	SetFontSize(m_soundChange[soundTypeBGM].fontSize);
	DrawFormatString(m_soundChange[soundTypeBGM].x, m_soundChange[soundTypeBGM].y, m_soundChange[soundTypeBGM].color,
		m_soundChange[soundTypeBGM].name.c_str(), soundMgr.GetBGMVolume());
	//SEの設定
	SetFontSize(m_soundChange[soundTypeSE].fontSize);
	DrawFormatString(m_soundChange[soundTypeSE].x, m_soundChange[soundTypeSE].y, m_soundChange[soundTypeSE].color,
		m_soundChange[soundTypeSE].name.c_str(), soundMgr.GetSEVolume());
	SetFontSize(m_soundChange[soundTypeBack].fontSize);
	//閉じる
	DrawFormatString(m_soundChange[soundTypeBack].x, m_soundChange[soundTypeBack].y, m_soundChange[soundTypeBack].color,
		m_soundChange[soundTypeBack].name.c_str());
	SetFontSize(0);
}

void SoundSettingScene::WindowDraw()
{
	//角表示
	int x = pw_start_x - 50 / 2;
	int y = pw_start_y - 50 / 2;
	DrawRectGraph(x, y,
		0, 0, 50, 50, m_bg, true);//左上　50 y3,x9
	DrawRectGraph(x + pw_width, y,
		50 * 8, 0, 50, 50, m_bg, true);//右上
	DrawRectGraph(x, y + pw_height,
		0, 50 * 2, 50, 50, m_bg, true);//左下　50 y3,x9
	DrawRectGraph(x + pw_width, y + pw_height,
		50 * 8, 50 * 2, 50, 50, m_bg, true);//右下

	//画像の左上、右下、グラフィックの左上からXサイズ、Yサイズ、表示する画像、透明
	DrawRectExtendGraph(x + 50, y, x + pw_width, y + 50,
		50 * 2, 0, 50, 50, m_bg, true);//上
	DrawRectExtendGraph(x, y + 50, x + 50, y + pw_height,
		0, 50 * 1, 50, 50, m_bg, true);//左
	DrawRectExtendGraph(x + pw_width, y + 50, x + pw_width + 50, y + pw_height,
		50 * 8, 50, 50, 50, m_bg, true);// 右
	DrawRectExtendGraph(x + 50, y + pw_height, x + pw_width, y + pw_height + 50,
		50 * 2, 50 * 2, 50, 50, m_bg, true);	// 下
	DrawRectExtendGraph(x + 50, y + 50, x + pw_width, y + pw_height,
		50 * 3, 50 * 1, 50, 50, m_bg, true);	// ウインドウ内部
}

void SoundSettingScene::NormalUpdate(const InputState& input)
{
	bool isPress = false;
	if (input.IsTriggered(InputType::down))
	{
		m_selectNum = (m_selectNum + 1) % soundTypeMax;
		isPress = true;
	}
	else if (input.IsTriggered(InputType::up))
	{
		m_selectNum = (m_selectNum + (soundTypeMax - 1)) % soundTypeMax;
		isPress = true;
	}

	if (isPress)
	{
		SoundManager::GetInstance().Play(SoundId::Cursor);
		for (int i = 0; i < soundTypeMax; i++)
		{
			if (i == m_selectNum)
			{
				m_soundChange[i].fontSize = kFontSize * 2;
				m_soundChange[i].color = PushSceneWindow::kSelectColor;
			}
			else
			{
				m_soundChange[i].fontSize = kFontSize;
				m_soundChange[i].color = PushSceneWindow::kNormalColor;
			}
		}
	}
	auto& soundMgr = SoundManager::GetInstance();

	if (input.IsTriggered(InputType::next))
	{
		SoundManager::GetInstance().Play(SoundId::Determinant);
		switch (m_selectNum)
		{
		case soundTypeBGM:
			m_soundChange[soundTypeBGM].color = 0xffa000;
			m_updateFunc = &SoundSettingScene::BGMVolumeChange;
			return;
		case soundTypeSE:
			m_soundChange[soundTypeSE].color = 0xffa000;
			m_updateFunc = &SoundSettingScene::SEVolumeChange;
			return;
		case soundTypeBack:
			soundMgr.SaveSoundConfig();
			m_manager.PopScene();
			return;
		default:
			break;
		}
	}
	if (input.IsTriggered(InputType::prev))
	{
		soundMgr.SaveSoundConfig();
		SoundManager::GetInstance().Play(SoundId::Determinant);
		m_selectNum = soundTypeBack;
		m_manager.PopScene();
		return;
	}
}

void SoundSettingScene::BGMVolumeChange(const InputState& input)
{
	auto& soundMgr = SoundManager::GetInstance();
	int soundVolume = 0;
	//BGM
	if (input.IsPressed(InputType::up))
	{
		if (input.IsTriggered(InputType::up))
		{
			m_puressTime = 0;
			m_waitInterval = 60;
		}
		if (m_puressTime % m_waitInterval == 0)
		{
			soundVolume = soundMgr.GetBGMVolume() + 1;
			if (soundVolume >= 255)
			{
				soundVolume = 255;
			}
			soundMgr.SetBGMVolume(soundVolume, m_soundH);
		}
		if (m_puressTime % 10 == 0)
		{
			if (m_waitInterval-- <= 3)
			{
				m_waitInterval = 3;
			}
		}
		m_puressTime++;
	}
	if (input.IsPressed(InputType::down))
	{
		if (input.IsTriggered(InputType::down))
		{
			m_puressTime = 0;
			m_waitInterval = 60;
		}
		if (m_puressTime % m_waitInterval == 0)
		{
			soundVolume = soundMgr.GetBGMVolume() - 1;
			if (soundVolume <= 0)
			{
				soundVolume = 0;
			}
			soundMgr.SetBGMVolume(soundVolume, m_soundH);
		}
		if (m_puressTime % 10 == 0)
		{
			if (m_waitInterval-- <= 3)
			{
				m_waitInterval = 3;
			}
		}
		m_puressTime++;
	}
	if (input.IsTriggered(InputType::next) || input.IsTriggered(InputType::prev))
	{
		SoundManager::GetInstance().Play(SoundId::Determinant);
		m_soundChange[soundTypeBGM].color = PushSceneWindow::kSelectColor;
		m_updateFunc = &SoundSettingScene::NormalUpdate;
		return;
	}
}

void SoundSettingScene::SEVolumeChange(const InputState& input)
{
	auto& soundMgr = SoundManager::GetInstance();
	int soundVolume = 0;

	//SE
	if (input.IsPressed(InputType::up))
	{
		if (input.IsTriggered(InputType::up))
		{
			m_puressTime = 0;
			m_waitInterval = 60;
		}
		if (m_puressTime % m_waitInterval == 0)
		{
			soundVolume = soundMgr.GetSEVolume() + 1;
			if (soundVolume >= 255)
			{
				soundVolume = 255;
			}
			SoundManager::GetInstance().Play(SoundId::Cursor);
			soundMgr.SetSEVolume(soundVolume);
		}
		if (m_puressTime % 10 == 0)
		{
			if (m_waitInterval-- <= 3)
			{
				m_waitInterval = 3;
			}
		}
		m_puressTime++;
	}
	if (input.IsPressed(InputType::down))
	{
		if (input.IsTriggered(InputType::down))
		{
			m_puressTime = 0;
			m_waitInterval = 60;
		}
		if (m_puressTime % m_waitInterval == 0)
		{
			soundVolume = soundMgr.GetSEVolume() - 1;
			if (soundVolume <= 0)
			{
				soundVolume = 0;
			}
			SoundManager::GetInstance().Play(SoundId::Cursor);
			soundMgr.SetSEVolume(soundVolume);
		}
		if (m_puressTime % 10 == 0)
		{
			if (m_waitInterval-- <= 3)
			{
				m_waitInterval = 3;
			}
		}
		m_puressTime++;
	}
	if (input.IsTriggered(InputType::next) || input.IsTriggered(InputType::prev))
	{
		SoundManager::GetInstance().Play(SoundId::Determinant);
		m_soundChange[soundTypeSE].color = PushSceneWindow::kSelectColor;
		m_updateFunc = &SoundSettingScene::NormalUpdate;
		return;
	}
}

