#pragma once
#include <unordered_map>

enum class SoundId
{
	Gameclear,		//�Q�[���N���A��
	Gameover,		//�Q�[���I�[�o�[��

	Cursor,			//�J�[�\���ړ���
	Determinant,	//����{�^��
	Back,			//�߂�{�^��

	BlockMove,		//�u���b�N�ڐG��
	MenuOpen,		//���j���[���J�����̉�
	PlayerJump,		//�v���C���[�W�����v��

	PlayeyHit,		//�v���C���[���U�����󂯂�

	EnemyHit,	//�G���U�����󂯂��Ƃ��̉�
	EnemyBurst,	//�G�����񂾂Ƃ��̉�

	StarGet,//�����擾�����Ƃ��̉�

	SoundId_Max
};

class SoundManager
{
private:
	SoundManager();
	
	//�R�s�[��������֎~����
	SoundManager(const SoundManager&) = delete;
	void operator= (const SoundManager&) = delete;

	/// <summary>
	/// �T�E���h�n���h�����擾
	/// </summary>
	/// <param name="id">�T�E���h��</param>
	/// <param name="fileName">�T�E���h�t�@�C����</param>
	/// <returns>�T�E���h�n���h��</returns>
	int LoadSoundFile(SoundId id,const wchar_t* fileName);
	
	/// <summary>
	/// �T�E���h���̃��[�h
	/// </summary>
	void LoadSoundConfig();

	std::unordered_map<SoundId, int> nameAndHandleTable_;//�T�E���h�n���h��

	//�ύX�����T�E���h�����t�@�C���ɏ�������
	struct SoundConfigInfo
	{
		unsigned short volumeSE;//0�`255
		unsigned short volumeBGM;//0�`255
	};

	int m_volumeSE = 255;//����SE����
	int m_volumeBGM = 255;//����BGM����
public:
	~SoundManager();
	/// <summary>
	/// SoundManager�g�p�҂�GetInstance()��ʂ����Q�Ƃ��炵�����p�ł��Ȃ�
	/// </summary>
	/// <returns>���̂̎Q�Ƃ�Ԃ�</returns>
	static SoundManager& GetInstance()
	{
		static SoundManager instance;//�B��̎���
		return instance;//����̎Q�Ƃ�Ԃ�
	}

	/// <summary>
	/// �T�E���h���̃Z�[�u
	/// </summary>
	void SaveSoundConfig();

	/// <summary>
	/// �w��̃T�E���h��炷
	/// </summary>
	/// <param name="name">�T�E���h��</param>
	void Play(SoundId id, int volume = 200);
	/// <summary>
	/// BGM��炷
	/// </summary>
	/// <param name="soundH">�T�E���h�n���h��</param>
	void PlayBGM(int soundH);

	/// <summary>
	/// SE�̃{�����[����ݒ肷��
	/// </summary>
	/// <param name="volume">����</param>
	void SetSEVolume(int volume);
	int GetSEVolume()const;
	/// <summary>
	/// BGM�̃{�����[����ݒ肷��
	/// </summary>
	/// <param name="volume">����</param>
	/// <param name="soundH">�T�E���h�n���h��</param>
	void SetBGMVolume(int volume,int soundH);
	int GetBGMVolume()const;

	/// <summary>
	/// �T�E���h���~�߂�
	/// </summary>
	/// <param name="id">�T�E���h��</param>
	void StopBgm(SoundId id);
};


