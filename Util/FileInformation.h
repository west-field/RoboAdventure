#pragma once

//ステージ項目
enum class StageSelect
{
	First,		//1-1
	Second,		//1-2
	Third,		//1-3

	stageNum			//ステージの数
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
	/// 読み込む
	/// </summary>
	void Load();

	/// <summary>
	/// 書き込む
	/// </summary>
	void Save();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	//情報を取得する
	Header GetHeader(int stage);

	/// <summary>
	/// クリアしたときに呼び出す
	/// </summary>
	/// <param name="stage">クリアしたステージ</param>
	void Clear(int stage, int score,bool coin1,bool coin2,bool coin3);

	/// <summary>
	/// スコアの比較
	/// </summary>
	/// <param name="stage">クリアしたステージ</param>
	/// <param name="score">スコア</param>
	void ScoreComparison(int stage,int score);

	/// <summary>
	/// ハイスコアを更新したかどうか
	/// </summary>
	/// <returns>true:更新した false:更新していない</returns>
	const bool IsHighScore()const { return m_isHighScore; }
private:
	Header m_header[static_cast<int>(StageSelect::stageNum)];

	bool m_isSave = false;//情報を更新したかどうか

	bool m_isHighScore;//ハイスコアを更新したかどうか
};

