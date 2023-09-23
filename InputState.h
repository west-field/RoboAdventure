#pragma once
#include <map>
#include <vector>
#include <string>
#include "KeyInfo.h"

/// <summary>
/// ���͏��
/// </summary>
struct InputInfo
{
	InputCategory cat;//
	int id;
};

//�t�����h�p�Ƀv���g�^�C�v�錾
class KeyConfigScene;

/// <summary>
/// ���͏�Ԃ��Ǘ�����
/// </summary>
class InputState
{
	friend KeyConfigScene;//KeyConfigScene�ɂ����A���ׂĂ�������B
public:
	InputState();

	/// <summary>
	/// ���͏����X�V����
	/// �i���Ӂj���t���[��Update���Ă΂Ȃ��Ɠ��͏󋵂͍X�V����Ȃ�
	/// </summary>
	void Update();

	/// <summary>
	/// ������Ă���true�ɂȂ�
	/// </summary>
	/// <param name="type">InputType</param>
	/// <returns>������Ă�����true,������ĂȂ��Ȃ�false</returns>
	bool IsPressed(InputType type) const;

	/// <summary>
	/// �������u�Ԃ�true�ɂȂ�
	/// </summary>
	/// <param name="type">InputType</param>
	/// <returns>�������u�ԂȂ�true�A����ȊO�Ȃ�false</returns>
	bool IsTriggered(InputType type) const;

	/// <summary>
	/// ���͏����X�V����
	/// </summary>
	/// <param name="type">�Q�[���ɂ�������͎��</param>
	/// <param name="cat">���̓J�e�S��</param>
	/// <param name="id">���ۂ̓���</param>
	void RewriteInputInfo(InputType type, InputCategory cat, int id);

	/// <summary>
	/// ���ݕҏW���̃L�[�R���t�B�O���m�肷��
	/// </summary>
	void CommitChangedInputInfo();

	/// <summary>
	/// ���ݕҏW���̃L�[�R���t�B�O�ݒ���Ȃ��������Ƃɂ���
	/// </summary>
	void RollbackChangedInputInfo();

	/// <summary>
	/// �L�[�}�b�v���f�t�H���g�Ƀ��Z�b�g����
	/// </summary>
	void ResetInputInfo();

	/// <summary>
	/// �L�[�}�b�v���t�@�C���ɕۑ�����
	/// </summary>
	void SaveKeyInfo()const;

	/// <summary>
	/// �L�[�}�b�v�����[�h����
	/// </summary>
	void LoadKeyInfo();
private:
	//���z���͏��ƁA���ۂ̓��̓e�[�u�������
	//�L�[(first)=InputType
	//�l(second)=std::vector<InputInfo>
	using InputMap_t = std::map<InputType, std::vector<InputInfo>>;
	InputMap_t m_inputMapTable;//���ۂ̓��͂ƃQ�[���{�^���̑Ή��e�[�u��

	InputMap_t m_tempMapTable;//���������p�̈ꎞ�I��inputMapTable_�̃R�s�[ �����Ȃ肩���Ă��܂�Ȃ��悤��
	InputMap_t m_defaultMapTable;//���Z�b�g�p�L�[�}�b�v�e�[�u��

	//���̓^�C�v�Ƃ��̖��O�̑Ή��e�[�u��
	std::map<InputType, std::wstring> m_inputNameTable;

	std::vector<bool> m_currentInput;//���݂̓��͏��(�����Ă��邩�����Ă��Ȃ���)
	std::vector<bool> m_lastInput;//���O�̓��͏��(���O�����Ă��邩�����Ă��Ȃ���)
};
