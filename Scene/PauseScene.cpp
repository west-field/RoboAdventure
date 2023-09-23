#include "PauseScene.h"
#include <DxLib.h>
#include "../game.h"
#include "../InputState.h"
#include "../Util/Sound.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "KeyConfigScene.h"
#include "SoundSettingScene.h"
#include "../Util/DrawFunctions.h"
#include "ConfirmationScene.h"

namespace
{
	//文字の大きさ
	constexpr int kFontSize = 20;
}

PauseScene::PauseScene(SceneManager& manager,int soundH) : Scene(manager), m_soundH(soundH)
{
	m_selectNum = static_cast<int>(Item::pauseBack);
	int i = 10;
	m_pauseMenu[static_cast<int>(Item::pauseKeyconfig)].x = PushSceneWindow::kPosX + 10;
	m_pauseMenu[static_cast<int>(Item::pauseKeyconfig)].y = PushSceneWindow::kPosY + kFontSize * 2 * (static_cast<int>(Item::pauseKeyconfig) + 1) + i;
	m_pauseMenu[static_cast<int>(Item::pauseKeyconfig)].name = L"キーせってい";
	i += 10;
	m_pauseMenu[static_cast<int>(Item::pauseSound)].x = PushSceneWindow::kPosX + 10;
	m_pauseMenu[static_cast<int>(Item::pauseSound)].y = PushSceneWindow::kPosY + kFontSize * 2 * (static_cast<int>(Item::pauseSound) + 1) + i;
	m_pauseMenu[static_cast<int>(Item::pauseSound)].name = L"音量せってい";
	i += 10;
	m_pauseMenu[static_cast<int>(Item::pauseFullscreen)].x = PushSceneWindow::kPosX + 10;
	m_pauseMenu[static_cast<int>(Item::pauseFullscreen)].y = PushSceneWindow::kPosY + kFontSize * 2 * (static_cast<int>(Item::pauseFullscreen) + 1) + i;
	m_pauseMenu[static_cast<int>(Item::pauseFullscreen)].name = L"スクリーンモード変更";
	i += 10;
	m_pauseMenu[static_cast<int>(Item::pauseBack)].x = PushSceneWindow::kPosX + 10;
	m_pauseMenu[static_cast<int>(Item::pauseBack)].y = PushSceneWindow::kPosY + kFontSize * 2 * (static_cast<int>(Item::pauseBack) + 1) + i;
	m_pauseMenu[static_cast<int>(Item::pauseBack)].name = L"閉じる";
	i += 10;
	m_pauseMenu[static_cast<int>(Item::pauseTitle)].x = PushSceneWindow::kPosX + 10;
	m_pauseMenu[static_cast<int>(Item::pauseTitle)].y = PushSceneWindow::kPosY + kFontSize * 2 * (static_cast<int>(Item::pauseTitle) + 1) + i;
	m_pauseMenu[static_cast<int>(Item::pauseTitle)].name = L"タイトル戻る";
	i += 10;
	m_pauseMenu[static_cast<int>(Item::pauseGameEnd)].x = PushSceneWindow::kPosX + 10;
	m_pauseMenu[static_cast<int>(Item::pauseGameEnd)].y = PushSceneWindow::kPosY + kFontSize * 2 * (static_cast<int>(Item::pauseGameEnd) + 1) + i;
	m_pauseMenu[static_cast<int>(Item::pauseGameEnd)].name = L"ゲーム終了";

	int num = static_cast<int>(Item::pauseMax);
	for (int i = 0; i < num; i++)
	{
		m_pauseMenu[i].fontSize = kFontSize;
		m_pauseMenu[i].color = PushSceneWindow::kNormalColor;
		m_pauseMenu[i].nameNum = static_cast<int>(wcslen(m_pauseMenu[i].name));
	}
	m_pauseMenu[m_selectNum].fontSize = kFontSize * 2;
	m_pauseMenu[m_selectNum].color = PushSceneWindow::kSelectColor;

	m_bg = my::MyLoadGraph(L"Data/Img/panel_Example3.png");
}

PauseScene::~PauseScene()
{
	DeleteGraph(m_bg);
}

void PauseScene::Update(const InputState& input)
{
	bool isPress = false;
	if (input.IsTriggered(InputType::down))
	{
		m_selectNum = (m_selectNum + 1) % pauseMax;
		isPress = true;
	}
	else if (input.IsTriggered(InputType::up))
	{
		m_selectNum = (m_selectNum + (pauseMax - 1)) % pauseMax;
		isPress = true;
	}

	if (isPress)
	{
		SoundManager::GetInstance().Play(SoundId::Cursor);
		for (int i = 0; i < pauseMax; i++)
		{
			if (i == m_selectNum)
			{
				m_pauseMenu[i].fontSize = kFontSize * 2;
				m_pauseMenu[i].color = PushSceneWindow::kSelectColor;
			}
			else
			{
				m_pauseMenu[i].fontSize = kFontSize;
				m_pauseMenu[i].color = PushSceneWindow::kNormalColor;
			}
		}
	}

	if (input.IsTriggered(InputType::next))
	{
		SoundManager::GetInstance().Play(SoundId::Determinant);
		switch (m_selectNum)
		{
		case pauseKeyconfig:
			SoundManager::GetInstance().Play(SoundId::Determinant);
			m_manager.PushScene(new KeyConfigScene(m_manager, input));
			return;
		case pauseSound:
			SoundManager::GetInstance().Play(SoundId::Determinant);
			m_manager.PushScene(new SoundSettingScene(m_manager, m_soundH));
			return;
		case static_cast<int>(Item::pauseFullscreen):
			SoundManager::GetInstance().Play(SoundId::Determinant);
			m_manager.PushScene(new ConfirmationScene(m_manager, L"スクリーンモード変更しますか", SelectType::SceneMode, m_soundH));
			return;
		case pauseBack:
			SoundManager::GetInstance().Play(SoundId::Back);
			m_manager.PopScene();
			return;
		case pauseTitle:
			SoundManager::GetInstance().Play(SoundId::Determinant);
			m_manager.PushScene(new ConfirmationScene(m_manager, L"タイトルに戻りますか", SelectType::BackTitle, m_soundH));
			return;
		case pauseGameEnd:
			SoundManager::GetInstance().Play(SoundId::Determinant);
			m_manager.PushScene(new ConfirmationScene(m_manager, L"ゲームを終了しますか", SelectType::End, m_soundH));
			return;
		default:
			break;
		}
	}
	if (input.IsTriggered(InputType::prev))
	{
		SoundManager::GetInstance().Play(SoundId::Determinant);
		m_selectNum = Item::pauseBack;
		m_manager.PopScene();
		return;
	}
}

void PauseScene::Draw()
{
	//ウィンドウ背景
	WindowDraw();
	//ポーズ中メッセージ
	SetFontSize(kFontSize * 2);

	DrawString(PushSceneWindow::pw_start_x + 10 + (kFontSize * 2) * 3, PushSceneWindow::pw_start_y + 10, L"ポーズ", 0x000000);
	for (auto& menu : m_pauseMenu)
	{
		SetFontSize(menu.fontSize);
		DrawString(menu.x, menu.y, menu.name, menu.color);
#ifdef _DEBUG
		int size = menu.nameNum * menu.fontSize;
		DrawBox(menu.x, menu.y, menu.x + size, menu.y + menu.fontSize, menu.color, false);
#endif
	}
	SetFontSize(0);
}

void PauseScene::WindowDraw()
{
	//角表示
	int x = PushSceneWindow::pw_start_x - 50 / 2;
	int y = PushSceneWindow::pw_start_y - 50 / 2;
	DrawRectGraph(x, y,
		0, 0, 50, 50, m_bg, true);//左上　50 y3,x9
	DrawRectGraph(x + PushSceneWindow::pw_width, y,
		50 * 8, 0, 50, 50, m_bg, true);//右上
	DrawRectGraph(x, y + PushSceneWindow::pw_height,
		0, 50 * 2, 50, 50, m_bg, true);//左下　50 y3,x9
	DrawRectGraph(x + PushSceneWindow::pw_width, y + PushSceneWindow::pw_height,
		50 * 8, 50 * 2, 50, 50, m_bg, true);//右下

	//画像の左上、右下、グラフィックの左上からXサイズ、Yサイズ、表示する画像、透明
	DrawRectExtendGraph(x + 50, y, x + PushSceneWindow::pw_width, y + 50,
		50 * 2, 0, 50, 50, m_bg, true);//上
	DrawRectExtendGraph(x, y + 50, x + 50, y + PushSceneWindow::pw_height,
		0, 50 * 1, 50, 50, m_bg, true);//左
	DrawRectExtendGraph(x + PushSceneWindow::pw_width, y + 50, x + PushSceneWindow::pw_width + 50, y + PushSceneWindow::pw_height,
		50 * 8, 50, 50, 50, m_bg, true);// 右
	DrawRectExtendGraph(x + 50, y + PushSceneWindow::pw_height, x + PushSceneWindow::pw_width, y + PushSceneWindow::pw_height + 50,
		50 * 2, 50 * 2, 50, 50, m_bg, true);	// 下
	DrawRectExtendGraph(x + 50, y + 50, x + PushSceneWindow::pw_width, y + PushSceneWindow::pw_height,
		50 * 3, 50 * 1, 50, 50, m_bg, true);	// ウインドウ内部
}
