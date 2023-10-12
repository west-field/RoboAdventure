#include "DebugScene.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "GameplayingScene.h"
#include "GameclearScene.h"
#include "GameoverScene.h"

#include "../game.h"

#include <DxLib.h>

#include "../Camera.h"
#include "../Model.h"
#include "../InputState.h"

#include <memory>

namespace
{
	constexpr int kStartX = 100;
	constexpr int kStartY = 100;
	constexpr int kyajirusi = 80;

	constexpr int kFontSize = 60;
}

DebugScene::DebugScene(SceneManager& manager):Scene(manager), m_updateFunc(&DebugScene::FadeInUpdat)
{
	m_stage = 0;
	m_select = 0;
	m_drawY = kStartY;

	for (auto& color : m_color)
	{
		color = 0xffffff;
	}

	SetFontSize(kFontSize);
}

DebugScene::~DebugScene()
{
	SetFontSize(0);
}

void DebugScene::Update(const InputState& input)
{
	(this->*m_updateFunc)(input);
}

void DebugScene::Draw()
{
	DrawString(kyajirusi, m_drawY, L"▶", 0xff0000);

	DrawString(kStartX, kStartY, L"タイトルへ", m_color[0]);
	DrawString(kStartX, kStartY + kFontSize, L"ゲームシーンへ", m_color[1]);
	DrawFormatString(kStartX + 10, kStartY + kFontSize * 2, m_color[2], L"ステージ選択%d", m_stage + 1);
	DrawString(kStartX, kStartY + kFontSize * 3, L"クリアシーンへ", m_color[3]);
	DrawString(kStartX, kStartY + kFontSize * 4, L"ゲームオーバーシーンへ", m_color[4]);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeValue);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void DebugScene::FadeInUpdat(const InputState& input)
{
	m_fadeValue = 255 * m_fadeTimer / kFadeInterval;
	if (--m_fadeTimer == 0)
	{
		m_updateFunc = &DebugScene::NormalUpdat;
		m_fadeValue = 0;
		return;
	}
}

void DebugScene::FadeOutUpdat(const InputState& input)
{
	m_fadeValue = 255 * m_fadeTimer / kFadeInterval;

	if (++m_fadeTimer == kFadeInterval)
	{
		std::shared_ptr<Camera> camera = std::make_shared<Camera>();
		std::shared_ptr<Model> model = std::make_shared<Model>(L"Data/Model/Robot.mv1", 0.002f, true);
		const wchar_t* name = L"Data/Img/Map1.fmf";
		switch (m_select)
		{
		case static_cast<int>(SelectScene::Title):
			//タイトル
			m_manager.ChangeScene(new TitleScene(m_manager, true));
			return;
		case static_cast<int>(SelectScene::Gameplaying):
			//ゲーム画面
			switch (m_stage)
			{
			case 0:
				name = L"Data/Img/Map1.fmf";
				break;
			case 1:
				name = L"Data/Img/Map2.fmf";
				break;
			case 2:
				name = L"Data/Img/Map3.fmf";
				break;
			default:
				break;
			}
			m_manager.ChangeScene(new GameplayingScene(m_manager, name, m_stage, camera));
			return;
		case static_cast<int>(SelectScene::ClearScene):
			m_manager.ChangeScene(new GameclearScene(m_manager, m_stage, 300, camera, model, false, false, false));
			return;
		case static_cast<int>(SelectScene::gameoverScene):
			m_manager.ChangeScene(new GameoverScene(m_manager, m_stage, 300, camera, model, false, false, false));
			return;
		default:
			break;
		}
	}
}

void DebugScene::NormalUpdat(const InputState& input)
{
	bool isPress = false;
	if (input.IsTriggered(InputType::down))
	{
		m_select = (m_select + 1) % static_cast<int>(SelectScene::SelectMax);
		isPress = true;
	}
	else if (input.IsTriggered(InputType::up))
	{
		m_select = (m_select + (static_cast<int>(SelectScene::SelectMax) - 1)) % static_cast<int>(SelectScene::SelectMax);
		isPress = true;
	}

	if (isPress)
	{
		m_drawY = kStartY + m_select * kFontSize;
	}

	for (int i = 0; i < static_cast<int>(SelectScene::SelectMax); i++)
	{
		if (i == m_select)
		{
			m_color[i] = 0xffff00;
		}
		else
		{
			m_color[i] = 0xffffff;
		}
	}

	if (input.IsTriggered(InputType::next))
	{
		m_color[m_select] = 0xf00f00;
		if (m_select == static_cast<int>(SelectScene::SelectStage))
		{
			m_updateFunc = &DebugScene::StageSelect;
			return;
		}
		else
		{
			m_updateFunc = &DebugScene::FadeOutUpdat;
			m_fadeColor = 0x000000;
			return;
		}
	}
}

void DebugScene::StageSelect(const InputState& input)
{
	bool isPress = false;
	if (input.IsTriggered(InputType::down) || input.IsTriggered(InputType::left))
	{
		m_stage = (m_stage + 1) % 3;
		isPress = true;
	}
	else if (input.IsTriggered(InputType::up) || input.IsTriggered(InputType::right))
	{
		m_stage = (m_stage + (3 - 1)) % 3;
		isPress = true;
	}

	if (input.IsTriggered(InputType::next))
	{
		m_updateFunc = &DebugScene::NormalUpdat;
		return;
	}
}
