#pragma once
#include "Secne.h"
#include <memory>

class InputState;
class Map;
class FileInformation;
class Camera;

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene : public Scene
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="manager"></param>
	/// <param name="start">true:タイトル画面から始める false:ステージ選択画面から始める</param>
	TitleScene(SceneManager& manager,bool start);
	virtual ~TitleScene();

	void Update(const InputState& input);
	virtual void Draw() override;

private:
	//フェードインの時のUpdate関数
	void FadeInUpdat(const InputState& input);
	//フェードアウトの時のUpdate関数
	void FadeOutUpdat(const InputState& input);

	//通常状態の時のUpdate関数
	void NormalUpdat(const InputState& input);
	//カメラを下に移動する
	void MoveDropUpdate(const InputState& input);
	//カメラを上に移動する
	void MoveUpUpdate(const InputState& input);
	//ステージ選択画面
	void StageSelectUpdate(const InputState& input);

	//タイトルロゴを表示
	void TitleRogoDraw();
	//何も表示しない
	void NoneDraw();
	//ステージ選択のための画像を表示
	void StageSelectDraw();

	void (TitleScene::* m_updateFunc)(const InputState& input);//Update用メンバ関数ポインタ
	void (TitleScene::* m_drawFunc)();//Draw用メンバ関数ポインタ

	//ステージ項目
	enum StageSelect
	{
		First,		//1-1
		Second,		//1-2
		Third,		//1-3

		stageNum			//ステージの数
	};
	//スター
	struct Star
	{
		int x, y;//位置
		bool isGet;//取得しているか
	};
	//ステージ要素
	struct StageElement
	{
		int x,y;//座標
		int handle;//表示する画像
		float scale;//拡大率
		bool isSelect;//選択できるかどうか
		int fade;//暗くする

		Star star[3];//スター
		int highScore;//ハイスコア
	};
	
	int m_titleRogoHandle;//タイトルロゴ

	int m_flashingTitleText;//文字を点滅させる
	int m_flashingTitleTextTime;//文字を点滅させる時間

	std::shared_ptr<Map> m_map;//マップ
	std::shared_ptr<Camera> m_camera;//カメラ
	std::shared_ptr<FileInformation> m_file;//ファイル

	StageElement m_selectStage[StageSelect::stageNum];//ステージ情報

	int m_starHandle;//スター画像
	int m_starOutlineHandle;//スターアウトライン画像

	int  m_selectNum;//今選択しているステージ

	bool m_isStart;//ゲーム開始して初めてタイトルを表示したとき
};
