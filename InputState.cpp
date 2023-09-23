#include "InputState.h"
#include "DxLib.h"
#include <assert.h>

//PAD_INPUT_Z �E�V�����_�[
//PAD_INPUT_1 A
//PAD_INPUT_2 B
//PAD_INPUT_3 X
//PAD_INPUT_4 Y

InputState::InputState()
{
	m_defaultMapTable[InputType::next] = { {InputCategory::keybd,KEY_INPUT_RETURN},
										//{InputCategory::pad,PAD_INPUT_R},//�X�^�[�g�{�^��
										{InputCategory::pad,PAD_INPUT_1},//A
										{InputCategory::mouse,MOUSE_INPUT_LEFT} };

	m_defaultMapTable[InputType::prev] = { {InputCategory::keybd,KEY_INPUT_ESCAPE},
										{InputCategory::pad,PAD_INPUT_2} };//B
	
	m_defaultMapTable[InputType::pause] = { {InputCategory::keybd,KEY_INPUT_P},
										{InputCategory::pad,PAD_INPUT_R} };//start

	m_defaultMapTable[InputType::up] = { {InputCategory::keybd,KEY_INPUT_UP},
											{InputCategory::pad,PAD_INPUT_UP} };//��

	m_defaultMapTable[InputType::down] = { {InputCategory::keybd,KEY_INPUT_DOWN},
											{InputCategory::pad,PAD_INPUT_DOWN} };//��

	m_defaultMapTable[InputType::left] = { {InputCategory::keybd , KEY_INPUT_LEFT},
											{InputCategory::pad , PAD_INPUT_LEFT} };//��

	m_defaultMapTable[InputType::right] = { {InputCategory::keybd , KEY_INPUT_RIGHT},
											{InputCategory::pad , PAD_INPUT_RIGHT} };//�E

	m_defaultMapTable[InputType::junp] = {{InputCategory::keybd , KEY_INPUT_SPACE},
										{InputCategory::pad , PAD_INPUT_1} };//A

	m_inputMapTable = m_defaultMapTable;
	//SaveKeyInfo();
	LoadKeyInfo();
	//�ꎞ�}�b�v�e�[�u���ɃR�s�[
	m_tempMapTable = m_inputMapTable;

	//���̓^�C�v�̖��O�e�[�u�������
	m_inputNameTable[InputType::next] = L"����";
	m_inputNameTable[InputType::prev] = L"�߂�";
	m_inputNameTable[InputType::pause] = L"�|�[�Y";
	m_inputNameTable[InputType::up] = L"��";
	m_inputNameTable[InputType::down] = L"��";
	m_inputNameTable[InputType::left] = L"��";
	m_inputNameTable[InputType::right] = L"�E";
	m_inputNameTable[InputType::junp] = L"�W�����v";

	m_currentInput.resize(static_cast<int>(InputType::max));
	m_lastInput.resize(static_cast<int>(InputType::max));
}

void
InputState::Update()
{
	m_lastInput = m_currentInput;//���O�̓��͏����L�����Ă���

	char keystate[256];
	GetHitKeyStateAll(keystate);//�S�L�[���擾
	int padState = GetJoypadInputState(DX_INPUT_PAD1);//�p�b�h1�R���̏����擾����
	int mouseState = GetMouseInput();
	//����input�̒��g�́Akeybd,KEY_INPUT_RETURN�Ȃǂ̃Z�b�g(InputInfo)�������Ă���
	//keymap.second�ɂ́A���̓��͏��Z�b�g�������Ă���
	//keymap.first�ɂ́A�Ή�����Q�[�����͖���"InputType::next"�Ȃǂ������Ă���
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
			//3�̓��͂̂����̂ǂꂩ��true������������u���͂���Ă���v�Ƃ݂Ȃ��āAbreak����B
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
	//���͎��(���蓖�Đ�)���Ȃ���΁A�����Ȃ̂Ŗ�������B
	if (m_tempMapTable.count(type) == 0)
	{
		return;
	}
	bool isRewrite = false;
	for (auto& inputInfo : m_tempMapTable[type])
	{
		if (inputInfo.cat == cat)//�J�e�S�����q�b�g������
		{
			//ID���㏑������
			inputInfo.id = id;
			isRewrite = true;
			break;
		}
	}
	if (!isRewrite)
	{
		//�����J�e�S�������݂��Ȃ�������A�����Œǉ����Ă���
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

	//���z�L�[�^�C�v�̐�����������
	int keytypeNum = static_cast<int>(m_inputMapTable.size());
	fwrite(&keytypeNum, sizeof(keytypeNum), 1, fp);
	//���z�L�[�^�C�v(next,prev�Ȃ�)�̃��[�v
	for (const auto& key : m_inputMapTable)
	{
		int keytype = static_cast<int> (key.first);
		//���z�L�[�ԍ�
		fwrite(&keytype, sizeof(keytype), 1, fp);
		int dataSize = static_cast<int>(key.second.size());
		//���������̓f�[�^������̂�
		fwrite(&dataSize, sizeof(dataSize), 1, fp);

		//vector�^��data()�͐擪�̃A�h���X��Ԃ��@��C�ɂ��̓��͂̃f�[�^����������
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
