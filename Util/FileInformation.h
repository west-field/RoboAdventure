#pragma once

//�X�e�[�W����
enum class StageSelect
{
	First,		//1-1
	Second,		//1-2
	Third,		//1-3

	stageNum			//�X�e�[�W�̐�
};

struct Header
{
	StageSelect stage;
	bool select;
	int highScore;
	bool isCoinGet[3];
};

class FileInformation
{
public:
	FileInformation();
	virtual ~FileInformation();

	/// <summary>
	/// �ǂݍ���
	/// </summary>
	void Load();

	/// <summary>
	/// ��������
	/// </summary>
	void Save();

	/// <summary>
	/// ������
	/// </summary>
	void Init();

	//�����擾����
	Header GetHeader(int stage);

	/// <summary>
	/// �N���A�����Ƃ��ɌĂяo��
	/// </summary>
	/// <param name="stage">�N���A�����X�e�[�W</param>
	void Clear(int stage, int score,bool coin1,bool coin2,bool coin3);

	/// <summary>
	/// �X�R�A�̔�r
	/// </summary>
	/// <param name="stage">�N���A�����X�e�[�W</param>
	/// <param name="score">�X�R�A</param>
	void ScoreComparison(int stage,int score);

	/// <summary>
	/// �n�C�X�R�A���X�V�������ǂ���
	/// </summary>
	/// <returns>true:�X�V���� false:�X�V���Ă��Ȃ�</returns>
	const bool IsHighScore()const { return m_isHighScore; }
private:
	Header m_header[static_cast<int>(StageSelect::stageNum)];

	bool m_isSave = false;//�����X�V�������ǂ���

	bool m_isHighScore;//�n�C�X�R�A���X�V�������ǂ���
};

