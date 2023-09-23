#include "FileInformation.h"
#include <DxLib.h>
#include <cassert>

namespace
{
	const char* path = "Data/stage.info";
}

FileInformation::FileInformation() :m_header()
{
	m_isSave = false;
	m_isHighScore = false;
}

FileInformation::~FileInformation()
{
}

void FileInformation::Load()
{
	FILE* fp = nullptr;
	fopen_s(&fp, path, "rb");//�t�@�C�����J��
	assert(fp);//�t�@�C�����J���Ȃ�������~�߂�
	fread(&m_header, sizeof(m_header), 1, fp);//�擾����

	fclose(fp);//�t�@�C�������
}

void FileInformation::Save()
{
	FILE* fp = nullptr;
	fopen_s(&fp, path, "wb");
	assert(fp);//�t�@�C�����J���Ȃ�������~�߂�

	fwrite(&m_header, sizeof(m_header), 1, fp);//������������

	fclose(fp);
}

void FileInformation::Init()
{
	FILE* fp = nullptr;
	fopen_s(&fp, path, "wb");
	assert(fp);//�t�@�C�����J���Ȃ�������~�߂�

	//������
	Header header[static_cast<int>(StageSelect::stageNum)] = {};
	header[static_cast<int>(StageSelect::First)].stage = StageSelect::First;
	header[static_cast<int>(StageSelect::Second)].stage = StageSelect::Second;
	header[static_cast<int>(StageSelect::Third)].stage = StageSelect::Third;

	for (auto& hea : header)
	{
		hea.select = false;

		hea.highScore = 3500;

		for (auto& coin : hea.isCoinGet)
		{
			coin = false;
		}
	}

	header[static_cast<int>(StageSelect::First)].select = true;

	fwrite(&header, sizeof(header), 1, fp);
	fclose(fp);
}

Header FileInformation::GetHeader(int stage)
{
	return m_header[stage];
}

void FileInformation::Clear(int stage, int score, bool coin1, bool coin2, bool coin3)
{
	//�Ō�̃X�e�[�W�̎��͉������Ȃ�
	if (stage == static_cast<int>(StageSelect::stageNum) - 1)
	{
		return;
	}
	//�N���A�����X�e�[�W�̎��̃X�e�[�W�ɍs����悤�ɂ���
	m_header[stage + 1].select = true;

	//�R�C�����擾�������ǂ���
	if (coin1)
	{
		m_header[stage].isCoinGet[0] = coin1;
	}
	if (coin2)
	{
		m_header[stage].isCoinGet[1] = coin2;
	}
	if (coin3)
	{
		m_header[stage].isCoinGet[2] = coin3;
	}

	//�n�C�X�R�A�Ɣ�r
	//����̃X�R�A���n�C�X�R�A�����傫���Ƃ�
	if (score > m_header[stage].highScore)
	{
		m_isHighScore = true;
		m_header[stage].highScore = score;
	}
}

