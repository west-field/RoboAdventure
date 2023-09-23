#include "InputState.h"
#include "DxLib.h"
#include <assert.h>

//PAD_INPUT_Z 右ショルダー
//PAD_INPUT_1 A
//PAD_INPUT_2 B
//PAD_INPUT_3 X
//PAD_INPUT_4 Y

InputState::InputState()
{
	m_defaultMapTable[InputType::next] = { {InputCategory::keybd,KEY_INPUT_RETURN},
										//{InputCategory::pad,PAD_INPUT_R},//スタートボタン
										{InputCategory::pad,PAD_INPUT_1},//A
										{InputCategory::mouse,MOUSE_INPUT_LEFT} };

	m_defaultMapTable[InputType::prev] = { {InputCategory::keybd,KEY_INPUT_ESCAPE},
										{InputCategory::pad,PAD_INPUT_2} };//B
	
	m_defaultMapTable[InputType::pause] = { {InputCategory::keybd,KEY_INPUT_P},
										{InputCategory::pad,PAD_INPUT_R} };//start

	m_defaultMapTable[InputType::up] = { {InputCategory::keybd,KEY_INPUT_UP},
											{InputCategory::pad,PAD_INPUT_UP} };//上

	m_defaultMapTable[InputType::down] = { {InputCategory::keybd,KEY_INPUT_DOWN},
											{InputCategory::pad,PAD_INPUT_DOWN} };//下

	m_defaultMapTable[InputType::left] = { {InputCategory::keybd , KEY_INPUT_LEFT},
											{InputCategory::pad , PAD_INPUT_LEFT} };//左

	m_defaultMapTable[InputType::right] = { {InputCategory::keybd , KEY_INPUT_RIGHT},
											{InputCategory::pad , PAD_INPUT_RIGHT} };//右

	m_defaultMapTable[InputType::junp] = {{InputCategory::keybd , KEY_INPUT_SPACE},
										{InputCategory::pad , PAD_INPUT_1} };//A

	m_inputMapTable = m_defaultMapTable;
	//SaveKeyInfo();
	LoadKeyInfo();
	//一時マップテーブルにコピー
	m_tempMapTable = m_inputMapTable;

	//入力タイプの名前テーブルを作る
	m_inputNameTable[InputType::next] = L"次へ";
	m_inputNameTable[InputType::prev] = L"戻る";
	m_inputNameTable[InputType::pause] = L"ポーズ";
	m_inputNameTable[InputType::up] = L"上";
	m_inputNameTable[InputType::down] = L"下";
	m_inputNameTable[InputType::left] = L"左";
	m_inputNameTable[InputType::right] = L"右";
	m_inputNameTable[InputType::junp] = L"ジャンプ";

	m_currentInput.resize(static_cast<int>(InputType::max));
	m_lastInput.resize(static_cast<int>(InputType::max));
}

void
InputState::Update()
{
	m_lastInput = m_currentInput;//直前の入力情報を記憶しておく

	char keystate[256];
	GetHitKeyStateAll(keystate);//全キー情報取得
	int padState = GetJoypadInputState(DX_INPUT_PAD1);//パッド1コンの情報を取得する
	int mouseState = GetMouseInput();
	//このinputの中身は、keybd,KEY_INPUT_RETURNなどのセット(InputInfo)が入っている
	//keymap.secondには、この入力情報セットが入っている
	//keymap.firstには、対応するゲーム入力名の"InputType::next"などが入っている
	for (const auto& keymap : m_inputMapTable)
	{
		for (const auto& input : keymap.second)
		{
			if (input.cat == InputCategory::keybd)
			{
				m_currentInput[static_cast<int>(keymap.first)] = keystate[input.id];
			}
			else if (input.cat == InputCategory::pad)
			{
				m_currentInput[static_cast<int>(keymap.first)] = padState & input.id;
			}
			else if (input.cat == InputCategory::mouse)
			{
				m_currentInput[static_cast<int>(keymap.first)] = mouseState & input.id;
			}
			//3つの入力のうちのどれかがtrueだったらもう「入力されている」とみなして、breakする。
			if (m_currentInput[static_cast<int>(keymap.first)])
			{
				break;
			}
		}
	}
}

bool
InputState::IsPressed(InputType type) const
{
	return m_currentInput[static_cast<int>(type)];
}

bool
InputState::IsTriggered(InputType type) const
{
	return IsPressed(type) && !m_lastInput[static_cast<int>(type)];
}

void
InputState::RewriteInputInfo(InputType type, InputCategory cat, int id)
{
	//入力種別(割り当て先)がなければ、無効なので無視する。
	if (m_tempMapTable.count(type) == 0)
	{
		return;
	}
	bool isRewrite = false;
	for (auto& inputInfo : m_tempMapTable[type])
	{
		if (inputInfo.cat == cat)//カテゴリがヒットしたら
		{
			//IDを上書きする
			inputInfo.id = id;
			isRewrite = true;
			break;
		}
	}
	if (!isRewrite)
	{
		//もしカテゴリが存在しなかったら、ここで追加しておく
		m_tempMapTable[type].push_back({ cat,id });
	}
}

void
InputState::CommitChangedInputInfo()
{
	m_inputMapTable = m_tempMapTable;
}

void
InputState::RollbackChangedInputInfo()
{
	m_tempMapTable = m_inputMapTable;
}

void InputState::ResetInputInfo()
{
	m_inputMapTable = m_defaultMapTable;
	m_tempMapTable = m_defaultMapTable;
}

void InputState::SaveKeyInfo()const
{
	FILE* fp = nullptr;

	auto err = fopen_s(&fp, "Data/key.info", "wb");
	if (fp == nullptr)
	{
		assert(0);
		return;
	}

	//仮想キータイプの数を書き込む
	int keytypeNum = static_cast<int>(m_inputMapTable.size());
	fwrite(&keytypeNum, sizeof(keytypeNum), 1, fp);
	//仮想キータイプ(next,prevなど)のループ
	for (const auto& key : m_inputMapTable)
	{
		int keytype = static_cast<int> (key.first);
		//仮想キー番号
		fwrite(&keytype, sizeof(keytype), 1, fp);
		int dataSize = static_cast<int>(key.second.size());
		//いくつ実入力データがあるのか
		fwrite(&dataSize, sizeof(dataSize), 1, fp);

		//vector型のdata()は先頭のアドレスを返す　一気にその入力のデータを書き込む
		fwrite(key.second.data(), dataSize * sizeof(InputInfo), 1, fp);
	}
	fclose(fp);
}

void InputState::LoadKeyInfo()
{
	int handle = FileRead_open(L"Data/key.info");
	if (handle == 0)
	{
		return;
	}
	int keyTypeNum = 0;
	FileRead_read(&keyTypeNum, sizeof(keyTypeNum), handle);
	m_inputMapTable.clear();
	for (int i = 0; i < keyTypeNum; i++)
	{
		int inputType = 0;
		FileRead_read(&inputType, sizeof(inputType), handle);

		int dataSize = 0;
		FileRead_read(&dataSize, sizeof(dataSize), handle);

		std::vector<InputInfo> inputInfoes(dataSize);
		FileRead_read(inputInfoes.data(), sizeof(InputInfo) * dataSize, handle);
		m_inputMapTable[static_cast<InputType>(inputType)] = inputInfoes;
	}
	m_tempMapTable = m_inputMapTable;
	FileRead_close(handle);
}
