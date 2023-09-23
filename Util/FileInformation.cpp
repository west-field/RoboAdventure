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
	fopen_s(&fp, path, "rb");//ファイルを開く
	assert(fp);//ファイルを開けなかったら止める
	fread(&m_header, sizeof(m_header), 1, fp);//取得する

	fclose(fp);//ファイルを閉じる
}

void FileInformation::Save()
{
	FILE* fp = nullptr;
	fopen_s(&fp, path, "wb");
	assert(fp);//ファイルを開けなかったら止める

	fwrite(&m_header, sizeof(m_header), 1, fp);//情報を書き込み

	fclose(fp);
}

void FileInformation::Init()
{
	FILE* fp = nullptr;
	fopen_s(&fp, path, "wb");
	assert(fp);//ファイルを開けなかったら止める

	//初期化
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
	//最後のステージの時は何もしない
	if (stage == static_cast<int>(StageSelect::stageNum) - 1)
	{
		return;
	}
	//クリアしたステージの次のステージに行けるようにする
	m_header[stage + 1].select = true;

	//コインを取得したかどうか
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

	//ハイスコアと比較
	//今回のスコアがハイスコアよりも大きいとき
	if (score > m_header[stage].highScore)
	{
		m_isHighScore = true;
		m_header[stage].highScore = score;
	}
}

