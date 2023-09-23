#include "ConfirmationScene.h"
#include <DxLib.h>
#include "../game.h"
#include "../InputState.h"
#include "../Util/Sound.h"
#include "../Util/DrawFunctions.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "SoundSettingScene.h"

namespace
{
	//�E�B���h�E�̑傫��
	static constexpr int kWindowWidth = 350;
	static constexpr int kWindowHeight = 250;
	//�E�B���h�E�̃X�^�[�g�ʒu
	static constexpr int kWindowStartX = (Game::kScreenWidth - kWindowWidth) / 2;
	static constexpr int kWindowStartY = (Game::kScreenHeight - kWindowHeight) / 2;

	//���ʒu
	static constexpr int kDrawPosY = (kWindowStartY + kWindowHeight) - (kWindowHeight / 2);

	//�����̑傫��
	static constexpr int kFontSize = 20;
}

ConfirmationScene::ConfirmationScene(SceneManager& manager, const wchar_t* conf, SelectType type,int soundH) :
	Scene(manager), m_type(type), m_soundH(soundH),m_conf(conf)
{
	m_stringNum = static_cast<int>(wcslen(m_conf));

	m_pauseMenu[static_cast<int>(Item::yes)].x = (kWindowStartX + kWindowWidth) - (kWindowWidth / 2);
	m_pauseMenu[static_cast<int>(Item::yes)].y = kDrawPosY;
	m_pauseMenu[static_cast<int>(Item::yes)].name = L"�͂�";
		
	m_pauseMenu[static_cast<int>(Item::no)].x = kWindowStartX + 20; 
	m_pauseMenu[static_cast<int>(Item::no)].y = kDrawPosY ;
	m_pauseMenu[static_cast<int>(Item::no)].name = L"������";

	m_selectNum = static_cast<int>(Item::no);

	for (int i = 0; i < static_cast<int>(Item::Max); i++)
	{
		m_pauseMenu[i].fontSize = kFontSize;
		m_pauseMenu[i].color = PushSceneWindow::kNormalColor;
		m_pauseMenu[i].nameNum = static_cast<int>(wcslen(m_pauseMenu[i].name));
	}
	m_pauseMenu[m_selectNum].fontSize = kFontSize * 2;
	m_pauseMenu[m_selectNum].color = PushSceneWindow::kSelectColor;

	m_isWindouwMode = m_manager.GetIsWindouMode();
	m_bg = my::MyLoadGraph(L"Data/Img/panel_Example2.png");
}

ConfirmationScene::~ConfirmationScene()
{
	DeleteGraph(m_bg);
}

void ConfirmationScene::Update(const InputState& input)
{
	bool isSelect = false;
	int pauseMax = static_cast<int>(Item::Max);

	bool isPress = false;
	if (input.IsTriggered(InputType::left) || input.IsTriggered(InputType::down))
	{
		m_selectNum = (m_selectNum + 1) % pauseMax;
		isPress = true;
	}
	else if (input.IsTriggered(InputType::right) || input.IsTriggered(InputType::up))
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
		switch (m_selectNum)
		{
		case static_cast<int>(Item::yes):
			StopSoundMem(m_soundH);
			SoundManager::GetInstance().Play(SoundId::Determinant);
			if (m_type == SelectType::End)
			{
				m_manager.SetIsEnd();
			}
			else if(m_type == SelectType::BackTitle)
			{
				m_manager.ChangeScene(new TitleScene(m_manager, false));
			}
			else if(m_type == SelectType::SceneMode)
			{
				FullSceneChange();
				m_manager.PopScene();
			}
			return;
		case static_cast<int>(Item::no):
			SoundManager::GetInstance().Play(SoundId::Back);
			m_manager.PopScene();
			return;
		default:
			break;
		}
	}
	if (input.IsTriggered(InputType::prev))
	{
		SoundManager::GetInstance().Play(SoundId::Back);
		m_selectNum = static_cast<int>(Item::no);
		m_manager.PopScene();
		return;
	}
}

void ConfirmationScene::Draw()
{
	//�E�B���h�E�w�i
	WindowDraw();
	//���b�Z�[�W
	SetFontSize(kFontSize);
	//DrawString(kWindowStartX + 30, kWindowStartY + 10, m_conf, 0xf0f088);
	int startString = (kWindowWidth - (m_stringNum * kFontSize)) / 2;
	DrawString(kWindowStartX + startString, kWindowStartY + kFontSize*2, m_conf,0xff0000 );//0xf0f088
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

void ConfirmationScene::WindowDraw()
{
	//�p�\��
	int x = kWindowStartX - 50 / 2;
	int y = kWindowStartY - 50 / 2;
	DrawRectGraph(x, y,
		0, 0, 50, 50, m_bg, true);//����@50 y3,x9
	DrawRectGraph(x + kWindowWidth, y,
		50 * 8, 0, 50, 50, m_bg, true);//�E��
	DrawRectGraph(x, y + kWindowHeight,
		0, 50 * 2, 50, 50, m_bg, true);//�����@50 y3,x9
	DrawRectGraph(x + kWindowWidth, y + kWindowHeight,
		50 * 8, 50 * 2, 50, 50, m_bg, true);//�E��

	//�摜�̍���A�E���A�O���t�B�b�N�̍��ォ��X�T�C�Y�AY�T�C�Y�A�\������摜�A����
	DrawRectExtendGraph(x + 50, y, x + kWindowWidth, y + 50,
		50 * 2, 0, 50, 50, m_bg, true);//��
	DrawRectExtendGraph(x, y + 50, x + 50, y + kWindowHeight,
		0, 50 * 1, 50, 50, m_bg, true);//��
	DrawRectExtendGraph(x + kWindowWidth, y + 50, x + kWindowWidth + 50, y + kWindowHeight,
		50 * 8, 50, 50, 50, m_bg, true);// �E
	DrawRectExtendGraph(x + 50, y + kWindowHeight, x + kWindowWidth, y + kWindowHeight + 50,
		50 * 2, 50 * 2, 50, 50, m_bg, true);	// ��
	DrawRectExtendGraph(x + 50, y + 50, x + kWindowWidth, y + kWindowHeight,
		50 * 3, 50 * 1, 50, 50, m_bg, true);	// �E�C���h�E����
}

void ConfirmationScene::FullSceneChange()
{
	m_isWindouwMode = !m_isWindouwMode;
	ChangeWindowMode(m_isWindouwMode);
	m_manager.SetIsWindouMode(m_isWindouwMode);
	SetDrawScreen(DX_SCREEN_BACK);//�`�����Ē�`

	ChangeVolumeSoundMem(SoundManager::GetInstance().GetBGMVolume(), m_soundH);
	PlaySoundMem(m_soundH, DX_PLAYTYPE_LOOP, true);
}
