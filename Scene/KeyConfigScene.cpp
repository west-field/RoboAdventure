#include "KeyConfigScene.h"
#include <DxLib.h>
#include "../game.h"
#include "../Util/Info.h"
#include "../InputState.h"
#include "../Util/Sound.h"
#include "../Util/DrawFunctions.h"
#include "SceneManager.h"

namespace
{
	//文字の大きさ
	static constexpr int kFontSize = 20;
	static constexpr int kMaxFontSize = static_cast<int>(20 * 1.5f);
	//ウィンドウの大きさ
	static constexpr int kWindowWidth = 700;
	static constexpr int kWindowHeight = 550;
	//ウィンドウのスタート位置
	static constexpr int kWindowStartX = (Game::kScreenWidth - kWindowWidth) / 2;
	static constexpr int kWindowStartY = (Game::kScreenHeight - kWindowHeight) / 2;
}

KeyConfigScene::KeyConfigScene(SceneManager& manager, const InputState& input) :
	m_inputState(input), Scene(manager), m_currentInputIndex(static_cast<int>(InputType::max))
{
	int idx = 0;//インデックス
	for (auto& conf : m_keyConfing)
	{
		m_keyConfing[idx].x = kWindowStartX + 20;
		m_keyConfing[idx].y = kWindowStartY + kFontSize + kMaxFontSize + 50 * idx;
		m_keyConfing[idx].color = PushSceneWindow::kNormalColor;
		m_keyConfing[idx].fontSize = kFontSize;
		m_keyConfing[idx].type = InputType::max;
		idx++;
	}

	idx = 0;
	int nameNum = 0;//文字の数
	//インプットステート(入力情報)にある入力タイプと名前の対応テーブル
	for (const auto& name : m_inputState.m_inputNameTable)
	{
		m_keyConfing[idx].type = name.first;
		m_keyConfing[idx].name = name.second.c_str();

		m_keyConfing[idx].nameNum = static_cast<int>(m_keyConfing[idx].name.size());
		if (nameNum < m_keyConfing[idx].nameNum)
		{
			nameNum = m_keyConfing[idx].nameNum;
		}

		idx++;
	}

	int x = 0;
	int i = static_cast<int>(InputType::max);
	m_keyConfing[i].name = L"決定(閉じる)";
	m_keyConfing[i].nameNum = static_cast<int>(m_keyConfing[i].name.size());
	x = (kWindowWidth - m_keyConfing[i].nameNum) / 2 - m_keyConfing[i].nameNum / 2;
	m_keyConfing[i].x = kWindowStartX + x;
	i++;
	m_keyConfing[i].name = L"キーリセット";
	m_keyConfing[i].nameNum = static_cast<int>(m_keyConfing[i].name.size());
	x = (kWindowWidth - m_keyConfing[i].nameNum) / 2 - m_keyConfing[i].nameNum / 2;
	m_keyConfing[i].x = kWindowStartX + x;

	m_keyConfing[m_currentInputIndex].color = PushSceneWindow::kSelectColor;
	m_keyConfing[m_currentInputIndex].fontSize = kMaxFontSize;

	//同じ位置に:をつける
	for (int i = 0; i < idx; i++)
	{
		int num = nameNum - m_keyConfing[i].nameNum;
		std::wstring name;
		for (int j = 0; j < num; j++)
		{
			name += L"　";
		}
		name += L":";
		m_keyConfing[i].name += name;
	}

	m_bg = my::MyLoadGraph(L"Data/Img/panel_Example1.png");
}

KeyConfigScene::~KeyConfigScene()
{
	m_inputState.SaveKeyInfo();
	DeleteGraph(m_bg);
}

void KeyConfigScene::Update(const InputState& input)
{
	//どの入力タイプを変更するかどうか
	auto& configInput = const_cast<InputState&>(input);
	//編集中でないとき
	if (!m_isEditing)
	{
		//決定、キーリセットを含めた入力タイプの数
		const int nameCount = static_cast<int>(InputType::max) + 2;
		bool isPress = false;
		//上下移動させる
		if (input.IsTriggered(InputType::up))
		{
			SoundManager::GetInstance().Play(SoundId::Cursor);
			m_currentInputIndex = ((m_currentInputIndex - 1) + nameCount) % nameCount;
			isPress = true;
		}
		else if (input.IsTriggered(InputType::down))
		{
			SoundManager::GetInstance().Play(SoundId::Cursor);
			m_currentInputIndex = (m_currentInputIndex + 1) % nameCount;
			isPress = true;
		}

		if (isPress)
		{
			SoundManager::GetInstance().Play(SoundId::Cursor);
			for (int i = 0; i < nameCount; i++)
			{
				if (i == m_currentInputIndex)
				{
					m_keyConfing[i].fontSize = kMaxFontSize;
					m_keyConfing[i].color = PushSceneWindow::kSelectColor;
				}
				else
				{
					m_keyConfing[i].fontSize = kFontSize;
					m_keyConfing[i].color = PushSceneWindow::kNormalColor;
				}
			}
		}

		if (input.IsTriggered(InputType::prev))
		{
			SoundManager::GetInstance().Play(SoundId::Back);
			configInput.CommitChangedInputInfo();
			m_manager.PopScene();
			return;
		}
	}

	//この時もう、「決定」を選択している
	//決定を選択している時
	if (m_currentInputIndex == input.m_inputNameTable.size())
	{
#ifdef _DEBUG
		m_numSize = static_cast<int>(input.m_inputMapTable.size());
#endif
		if (input.IsTriggered(InputType::next))
		{
			SoundManager::GetInstance().Play(SoundId::Determinant);
			configInput.CommitChangedInputInfo();
			m_manager.PopScene();
			return;
		}
	}
	//キーリセットを選択している時
	if (m_currentInputIndex == input.m_inputMapTable.size() + 1)
	{
#ifdef _DEBUG
		m_numSize = static_cast<int>(input.m_inputMapTable.size() + 1);
#endif
		if (input.IsTriggered(InputType::next))
		{
			SoundManager::GetInstance().Play(SoundId::Determinant);
			configInput.ResetInputInfo();
			return;
		}
	}
	//nextボタンでエディット中かそうじゃないかを決定する
	if (input.IsTriggered(InputType::next))
	{
		SoundManager::GetInstance().Play(SoundId::Determinant);
		m_isEditing = !m_isEditing;
		if (m_isEditing)
		{
			m_keyConfing[m_currentInputIndex].color = 0xffa000;//入力中の色
		}
		else
		{
			m_keyConfing[m_currentInputIndex].color = PushSceneWindow::kSelectColor;//選択している時の色
		}
		return;
	}

	//編集中の時
	if (m_isEditing)
	{
		char keystate[256];
		GetHitKeyStateAll(keystate);
		auto padState = GetJoypadInputState(DX_INPUT_PAD1);
		auto mouseState = GetMouseInput();

		int idx = 0;//インデックス
		InputType currentType = InputType::max;//入力タイプを取得するため

		//インプットステート(入力情報)にある入力タイプと名前の対応テーブル
		for (const auto& name : m_inputState.m_inputNameTable)
		{
			//選択中のインデックスと回しているインデックスが同じとき
			if (m_currentInputIndex == idx)
			{
				//その場所の入力タイプを取得する
				currentType = name.first;
				break;
			}
			++idx;
		}

		//入力情報を変更する
		for (int i = 0; i < 256; i++)
		{
			if (keystate[i])
			{
				configInput.RewriteInputInfo(currentType, InputCategory::keybd, i);
				break;
			}
		}

		if (padState != 0)
		{
			configInput.RewriteInputInfo(currentType, InputCategory::pad, padState);
		}
		if (mouseState != 0)
		{
			configInput.RewriteInputInfo(currentType, InputCategory::mouse, mouseState);
		}
	}
}

void KeyConfigScene::Draw()
{
	WindowDraw();

	SetFontSize(kMaxFontSize);
	DrawString(kWindowStartX + kFontSize, kWindowStartY + kFontSize, L"キー設定...", 0x000000);

	int idx = 0;//選択中のインデックスと同じかどうかを比較するため

	//インプットステート(入力情報)にある入力タイプと名前の対応テーブル
	for (const auto& name : m_inputState.m_inputNameTable)
	{
		std::wstring drawInput = m_keyConfing[idx].name;//表示する入力情報

		SetFontSize(m_keyConfing[idx].fontSize);

		//入力タイプを取得する
		auto type = name.first;
		//取得した入力タイプのキー情報をすべて取得する({InputCategory::keybd,KEY_INPUT_}等)
		auto it = m_inputState.m_tempMapTable.find(type);
		//取得したキー情報の分だけ表示させる
		for (auto& elem : it->second)
		{
			//キーボード
			if (elem.cat == InputCategory::keybd)
			{
				drawInput += KeyName(elem.id);
			}
			//パッド
			else if (elem.cat == InputCategory::pad)
			{
				drawInput += PadName(elem.id);
			}
			//マウス
			else if (elem.cat == InputCategory::mouse)
			{
				drawInput += MouseName(elem.id);
			}
			drawInput += L" ";
		}
		DrawString(m_keyConfing[idx].x, m_keyConfing[idx].y, drawInput.c_str(), m_keyConfing[idx].color);
		//インデックスを進める
		idx++;
	}
	int i = static_cast<int>(InputType::max);
	SetFontSize(m_keyConfing[i].fontSize);
	DrawString(m_keyConfing[i].x, m_keyConfing[i].y, m_keyConfing[i].name.c_str(), m_keyConfing[i].color);
	i++;
	SetFontSize(m_keyConfing[i].fontSize);
	DrawString(m_keyConfing[i].x, m_keyConfing[i].y, m_keyConfing[i].name.c_str(), m_keyConfing[i].color);

	SetFontSize(0);
#ifdef _DEBUG
	DrawFormatString(0, 100, 0x000000, L"選択  %d", m_currentInputIndex);
	DrawFormatString(0, 120, 0x000000, L"現在地%d", m_numSize);
#endif
}

void KeyConfigScene::WindowDraw()
{
	//角表示
	int x = kWindowStartX - 50 / 2;
	int y = kWindowStartY - 50 / 2;
	DrawRectGraph(x, y,
		0, 0, 50, 50, m_bg, true);//左上　50 y3,x9
	DrawRectGraph(x + kWindowWidth, y,
		50 * 8, 0, 50, 50, m_bg, true);//右上
	DrawRectGraph(x, y + kWindowHeight,
		0, 50 * 2, 50, 50, m_bg, true);//左下　50 y3,x9
	DrawRectGraph(x + kWindowWidth, y + kWindowHeight,
		50 * 8, 50 * 2, 50, 50, m_bg, true);//右下

	//画像の左上、右下、グラフィックの左上からXサイズ、Yサイズ、表示する画像、透明
	DrawRectExtendGraph(x + 50, y, x + kWindowWidth, y + 50,
		50 * 2, 0, 50, 50, m_bg, true);//上
	DrawRectExtendGraph(x, y + 50, x + 50, y + kWindowHeight,
		0, 50 * 1, 50, 50, m_bg, true);//左
	DrawRectExtendGraph(x + kWindowWidth, y + 50, x + kWindowWidth + 50, y + kWindowHeight,
		50 * 8, 50, 50, 50, m_bg, true);// 右
	DrawRectExtendGraph(x + 50, y + kWindowHeight, x + kWindowWidth, y + kWindowHeight + 50,
		50 * 2, 50 * 2, 50, 50, m_bg, true);	// 下
	DrawRectExtendGraph(x + 50, y + 50, x + kWindowWidth, y + kWindowHeight,
		50 * 3, 50 * 1, 50, 50, m_bg, true);	// ウインドウ内部
}

std::wstring KeyConfigScene::PadName(int id)
{
	std::wstring path = L"pad=";

	switch (id)
	{
	case static_cast<int>(PAD_INPUT_1):
		path += L"A";
		break;
	case static_cast<int>(PAD_INPUT_2):
		path += L"B";
		break;
	case static_cast<int>(PAD_INPUT_3):
		path += L"X";
		break;
	case static_cast<int>(PAD_INPUT_4):
		path += L"Y";
		break;
	case static_cast<int>(PAD_INPUT_LEFT):
		path += L"←";
		break;
	case static_cast<int>(PAD_INPUT_RIGHT):
		path += L"→";
		break;
	case static_cast<int>(PAD_INPUT_UP):
		path += L"↑";
		break;
	case static_cast<int>(PAD_INPUT_DOWN):
		path += L"↓";
		break;
	case static_cast<int>(PAD_INPUT_L):
		path += L"BACK";
		break;
	case static_cast<int>(PAD_INPUT_R):
		path += L"START";
		break;

	default:
		path += id;
		break;
	}

	return path;
}

std::wstring KeyConfigScene::KeyName(int id)
{
	std::wstring path = L"key=";

	switch (id)
	{
	case static_cast<int>(KEY_INPUT_A):
		path += L"A";
		break;// Ａキー
	case static_cast<int>(KEY_INPUT_B):
		path += L"B";
		break;// Ｂキー
	case static_cast<int>(KEY_INPUT_C):
		path += L"C";
		break;	// Ｃキー
	case static_cast<int>(KEY_INPUT_D):
		path += L"D";
		break;// Ｄキー
	case static_cast<int>(KEY_INPUT_E):
		path += L"E";
		break;// Ｅキー
	case static_cast<int>(KEY_INPUT_F):
		path += L"F";
		break;// Ｆキー
	case static_cast<int>(KEY_INPUT_G):
		path += L"G";
		break;// Ｇキー
	case static_cast<int>(KEY_INPUT_H):
		path += L"H";
		break;// Ｈキー
	case static_cast<int>(KEY_INPUT_I):
		path += L"I";
		break;// Ｉキー
	case static_cast<int>(KEY_INPUT_J):
		path += L"J";
		break;// Ｊキー
	case static_cast<int>(KEY_INPUT_K):
		path += L"K";
		break;// Ｋキー
	case static_cast<int>(KEY_INPUT_L):
		path += L"L";
		break;// Ｌキー
	case static_cast<int>(KEY_INPUT_M):
		path += L"M";
		break;// Ｍキー
	case static_cast<int>(KEY_INPUT_N):
		path += L"N";
		break;// Ｎキー
	case static_cast<int>(KEY_INPUT_O):
		path += L"O";
		break;// Ｏキー
	case static_cast<int>(KEY_INPUT_P):
		path += L"P";
		break;	// Ｐキー
	case static_cast<int>(KEY_INPUT_Q):
		path += L"Q";
		break;// Ｑキー
	case static_cast<int>(KEY_INPUT_R):
		path += L"R";
		break;// Ｒキー
	case static_cast<int>(KEY_INPUT_S):
		path += L"S";
		break;	// Ｓキー
	case static_cast<int>(KEY_INPUT_T):
		path += L"T";
		break;// Ｔキー
	case static_cast<int>(KEY_INPUT_U):
		path += L"U";
		break;// Ｕキー
	case static_cast<int>(KEY_INPUT_V):
		path += L"V";
		break;// Ｖキー
	case static_cast<int>(KEY_INPUT_W):
		path += L"W";
		break;// Ｗキー
	case static_cast<int>(KEY_INPUT_X):	
		path += L"X";
		break;// Ｘキー
	case static_cast<int>(KEY_INPUT_Y):
		path += L"Y";
		break;// Ｙキー
	case static_cast<int>(KEY_INPUT_Z):	
		path += L"Z";
		break;// Ｚキー
	case static_cast<int>(KEY_INPUT_ESCAPE):
		path += L"ESC";
		break;
	case static_cast<int>(KEY_INPUT_LEFT):
		path += L"←";
		break;
	case static_cast<int>(KEY_INPUT_RIGHT):
		path += L"→";
		break;
	case static_cast<int>(KEY_INPUT_UP):
		path += L"↑";
		break;
	case static_cast<int>(KEY_INPUT_DOWN):
		path += L"↓";
		break;
	case static_cast<int>(KEY_INPUT_SPACE):
		path += L"SPACE";
		break;
	case static_cast<int>(KEY_INPUT_RETURN):
		path += L"ENTER";
		break;

	default:
		path += id;
		break;
	}

	return path;
}

std::wstring KeyConfigScene::MouseName(int id)
{
	std::wstring path = L"mouse=";

	switch (id)
	{
	case static_cast<int>(MOUSE_INPUT_LEFT):
		path += L"左クリック";
		break;
	case static_cast<int>(MOUSE_INPUT_RIGHT):
		path += L"右クリック";
		break;
	default:
		path += id;
		break;
	}

	return path;
}


