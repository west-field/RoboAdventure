#include "GameclearScene.h"
#include <DxLib.h>
#include "../game.h"
#include "../Camera.h"
#include "../Model.h"
#include "../Util/Sound.h"
#include "../InputState.h"
#include "../Util/DrawFunctions.h"
#include "../Util/FileInformation.h"
#include "../Game/Map.h"
#include "../Game/Item/ItemCoin.h"
#include "SceneManager.h"
#include "TitleScene.h"


namespace
{
	constexpr float kMoveNum = 2.0f;//プレイヤー移動スピード

	const wchar_t* const kFileName = L"Data/Img/Scene.fmf";//使用するマップ

	//文字の大きさ
	static constexpr int kFontSize = 75;
	static constexpr int kMaxFontSize = static_cast<int>(20 * 1.5f);
	//ウィンドウの大きさ
	static constexpr int kWindowWidth = 600;
	static constexpr int kWindowHeight = 450;
	//ウィンドウのスタート位置
	static constexpr int kWindowStartX = 300;
	static constexpr int kWindowStartY = 150;

	constexpr float kScale = 0.002f;//表示するプレイヤーモデルの大きさ

	constexpr int kWaitingTime = 60 * 2;//操作ができるようになるまでの時間

	constexpr int kHighScoreDrawPosY = 130;//ハイスコアを表示する位置Y
}

GameclearScene::GameclearScene(SceneManager& manager,const int selectStage, const int score, std::shared_ptr<Camera> camera, std::shared_ptr<Model> model, bool getCoin1, bool getCoin2, bool getCoin3) :
	Scene(manager), m_updateFunc(&GameclearScene::FadeInUpdat),
	m_drawFunc(&GameclearScene::NormalDraw) , m_camera(camera), m_model(model)
{
	m_camera->CameraPosInit();//カメラの位置を初期化

	m_selectStage = selectStage;//選択していたステージ
	
	m_file = std::make_shared<FileInformation>();
	m_file->Load();//各ステージの情報取得

	m_file->Clear(m_selectStage, m_score, getCoin1, getCoin2, getCoin3);//選択したステージ、スコア、スター獲得状況
	m_file->Save();


	m_map = std::make_shared<Map>(kFileName,0.0f,0);//ゲームクリアで使用するマップ

	m_model->SetAnimation(13, true, true);//モデルのアニメーションを変更
	m_model->SetPos(VGet(14.0f,-0.4f,0.0f));//モデルの位置を変更
	m_model->SetRot(VGet(0.0f,95.0f,0.0f));//モデルの回転を変更
	m_model->SetScale(VGet(kScale, kScale, kScale));//モデルの大きさを変更

	//今のステージの情報を取得
	Header header = m_file->GetHeader(m_selectStage);
	m_score = score;//今回のスコア
	m_highScore = header.highScore;//ハイスコア
	//スター取得状況
	m_getCoin[0] = getCoin1;
	m_getCoin[1] = getCoin2;
	m_getCoin[2] = getCoin3;

	//スターモデル
	int handle1 = MV1LoadModel(L"Data/Model/Star.mv1");
	int handle2 = MV1LoadModel(L"Data/Model/Star_Outline.mv1");
	float x = 5.0f;//表示位置
	int index = 1;//スターのインデックス
	for (auto& star : m_starHandle)
	{
		if (m_getCoin[index - 1])
		{
			star = std::make_shared<ItemStar>(VGet(x, 5.0f, 0.0f), handle1, index);
		}
		else
		{
			star = std::make_shared<ItemStar>(VGet(x, 5.0f, 0.0f), handle2, index);
		}
		x += 3.0f;//移動
		index++;
	}
	MV1DeleteModel(handle1);
	MV1DeleteModel(handle2);

	m_waitingTime = kWaitingTime;//次に操作できるようになるまでの時間

	//BGM
	m_BgmH = LoadSoundMem(L"Data/Sound/BGM/clearSound.mp3");
	ChangeVolumeSoundMem(0, m_BgmH);
	PlaySoundMem(m_BgmH, DX_PLAYTYPE_LOOP, true);
}

GameclearScene::~GameclearScene()
{
	DeleteSoundMem(m_BgmH);
	m_model->DeleteModel();
	m_model.reset();
	m_map.reset();
	for (auto& coin : m_starHandle)
	{
		coin.reset();
	}
}

void GameclearScene::Update(const InputState& input)
{
	m_map->Update();
	m_model->Update();
	(this->*m_updateFunc)(input);
}

void GameclearScene::Draw()
{
	m_map->Draw();
	(this->*m_drawFunc)();
	m_model->Draw();

	//フェードイン、アウト用
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeValue);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameclearScene::FadeInUpdat(const InputState& input)
{
	m_fadeValue = 255 * m_fadeTimer / kFadeInterval;//だんだん明るく
	ChangeVolumeSoundMem(SoundManager::GetInstance().GetBGMVolume() - m_fadeValue, m_BgmH);//BGMをだんだん大きくする
	if (--m_fadeTimer == 0)
	{
		m_updateFunc = &GameclearScene::NormalUpdat;
		m_fadeValue = 0;
		SoundManager::GetInstance().Play(SoundId::Gameclear);//ゲームクリアの音を鳴らす
	}
}
void GameclearScene::FadeOutUpdat(const InputState& input)
{
	m_fadeValue = 255 * m_fadeTimer / kFadeInterval;//だんだん暗く
	ChangeVolumeSoundMem(SoundManager::GetInstance().GetBGMVolume() - m_fadeValue, m_BgmH);//BGMをだんだん小さくする
	if (++m_fadeTimer == kFadeInterval)
	{
		m_manager.ChangeScene(new TitleScene(m_manager, false));
		StopSoundMem(m_BgmH);//画面を変更する際、BGMを止める
		return;
	}
}

void GameclearScene::NormalUpdat(const InputState& input)
{
	//スター
	for (int i = 0;i<3;i++)
	{
		//取得していないときは処理しない
		if (!m_getCoin[i])	continue;
		m_starHandle[i]->Update();
	}

	//待ち時間が過ぎて、nextを押したとき
	if (m_waitingTime-- <= 0 && input.IsTriggered(InputType::next))
	{
		m_updateFunc = &GameclearScene::FadeOutUpdat;
		return;
	}
}

void GameclearScene::NormalDraw()
{
	//スコア表示
	for (int i = 0; i < 3; i++)
	{
		m_starHandle[i]->Draw();
	}

	int x = 300;
	//今回のスコアがハイスコアの時
	if (m_file->IsHighScore())
	{
		SetFontSize(kFontSize);
		DrawFormatString(x, Game::kScreenHeight / 2, 0xf0f000, L"ハイスコア : %d", m_highScore);
	}
	else
	{
		SetFontSize(kFontSize - 20);
		DrawFormatString(x, kHighScoreDrawPosY, 0xf0f000, L"ハイスコア : %d", m_highScore);
		SetFontSize(kFontSize);
		DrawFormatString(x, Game::kScreenHeight / 2, 0x00f0f0, L"　スコア : %d", m_score);
	}
	SetFontSize(0);
}