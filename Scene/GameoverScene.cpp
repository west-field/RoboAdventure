#include "GameoverScene.h"
#include <DxLib.h>
#include "../game.h"
#include "../Camera.h"
#include "../Model.h"
#include "../Game/Map.h"
#include "../Util/Sound.h"
#include "../InputState.h"
#include "../Util/DrawFunctions.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "GameplayingScene.h"
#include "../Game/Item/ItemCoin.h"

namespace
{
	constexpr float kMoveNum = 2.0f;//プレイヤー移動スピード
	constexpr int kMojiSize = 90;//文字の大きさ

	const wchar_t* const kFileName = L"Data/Img/Scene.fmf";//使用するマップ

	constexpr float kScale = 0.002f;//表示するプレイヤーモデルの大きさ

	constexpr int kFontSize = 75;

	//数字画像
	constexpr int kFontGraphSizeX = 48;// 32;
	constexpr int kFontGraphSizeY = 74;// 15;

	constexpr int kScoreDrawPosX = Game::kScreenWidth / 1.7f;
	constexpr int kScoreDrawPosY = Game::kScreenHeight / 2.0f;
}

GameoverScene::GameoverScene(SceneManager& manager, const int selectStage, const int score, std::shared_ptr<Camera> camera, std::shared_ptr<Model> model, bool getCoin1, bool getCoin2, bool getCoin3) :
	Scene(manager),  m_updateFunc(&GameoverScene::FadeInUpdat),
	m_drawFunc(&GameoverScene::NormalDraw) , m_camera(camera), m_model(model)
{
	m_fadeColor = 0xff0000;//フェードの色（赤

	m_camera->CameraPosInit();//カメラの位置を初期化
	
	m_map = std::make_shared<Map>(kFileName, 0.0f, 0);//ゲームオーバーで使用するマップ

	//モデル更新
	m_model->SetAnimation(4, true, true);
	m_model->SetPos(VGet(14.0f, -0.4f, 0.0f));
	m_model->SetRot(VGet(0.0f, 95.0f, 0.0f));
	m_model->SetScale(VGet(kScale, kScale, kScale));

	m_score = score;
	//スター取得状況
	m_getStar[0] = getCoin1;
	m_getStar[1] = getCoin2;
	m_getStar[2] = getCoin3;

	//スターモデル
	int handle1 = MV1LoadModel(L"Data/Model/Star.mv1");
	int handle2 = MV1LoadModel(L"Data/Model/Star_Outline.mv1");
	float x = 5.0f;
	int index = 1;
	//スター
	for (auto& star : m_starHandle)
	{
		if (m_getStar[index - 1])
		{
			star = std::make_shared<ItemStar>(VGet(x, 5.0f, 0.0f), handle1, index);
		}
		else
		{
			star = std::make_shared<ItemStar>(VGet(x, 5.0f, 0.0f), handle2, index);
		}
		x += 3.0f;
		index++;
	}
	MV1DeleteModel(handle1);
	MV1DeleteModel(handle2);

	m_BgmH = LoadSoundMem(L"Data/Sound/BGM/gameoverSound.mp3");
	ChangeVolumeSoundMem(0, m_BgmH);
	PlaySoundMem(m_BgmH, DX_PLAYTYPE_LOOP, true);

	m_numGraphHandle = LoadGraph(L"Data/Img/num.png");
}

GameoverScene::~GameoverScene()
{
	DeleteSoundMem(m_BgmH);
	m_map.reset();
	m_model->DeleteModel();
	m_model.reset();
	DeleteGraph(m_numGraphHandle);
}

void
GameoverScene::Update(const InputState& input)
{
	m_map->Update();
	m_model->Update();
	(this->*m_updateFunc)(input);
}

void
GameoverScene::Draw()
{
	m_map->Draw();
	(this->*m_drawFunc)();
	m_model->Draw();
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeValue);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameoverScene::FadeInUpdat(const InputState& input)
{
	m_fadeValue = 255 * m_fadeTimer / kFadeInterval;//だんだん明るく
	ChangeVolumeSoundMem(SoundManager::GetInstance().GetBGMVolume() - m_fadeValue, m_BgmH);//だんだん大きく
	if (--m_fadeTimer == 0)
	{
		m_updateFunc = &GameoverScene::NormalUpdat;
		m_fadeColor = 0x000000;//黒色でフェードアウトする
		m_fadeValue = 0;
	}
}

void GameoverScene::FadeOutUpdat(const InputState& input)
{
	m_fadeValue = 255 * m_fadeTimer / kFadeInterval;//だんだん暗く
	ChangeVolumeSoundMem(SoundManager::GetInstance().GetBGMVolume() - m_fadeValue, m_BgmH);//だんだん小さく
	if (++m_fadeTimer == kFadeInterval)
	{
		m_manager.ChangeScene(new TitleScene(m_manager, false));
		StopSoundMem(m_BgmH);//音を止める
		return;
	}
}

void GameoverScene::NormalUpdat(const InputState& input)
{
	//スター
	for (int i = 0; i < 3; i++)
	{
		if (!m_getStar[i])	continue;
		m_starHandle[i]->Update();
	}

	if (input.IsTriggered(InputType::next))
	{
		m_updateFunc = &GameoverScene::FadeOutUpdat;
		return;
	}
}

void GameoverScene::NormalDraw()
{
	//スター表示
	for (int i = 0; i < 3; i++)
	{
		m_starHandle[i]->Draw();
	}
	//スコア表示
	SetFontSize(kFontSize);
	DrawFormatString(300, Game::kScreenHeight / 2, 0x00f0f0, L"　スコア:");
	PointDraw(kScoreDrawPosX, kScoreDrawPosY + 30, m_score, 1.0f);//スコア表示
	SetFontSize(0);
}

void GameoverScene::PointDraw(int leftX, int y, int dispNum, float size, int digit)
{
	int digitNum = 0;
	int temp = dispNum;
	int cutNum = 10;	// 表示桁数指定時に表示をおさめるために使用する

	// 表示する数字の桁数数える
	while (temp > 0)
	{
		digitNum++;
		temp /= 10;
		cutNum *= 10;
	}
	if (digitNum <= 0)
	{
		digitNum = 1;	// 0の場合は1桁表示する
	}

	// 表示桁数指定
	temp = dispNum;
	if (digit >= 0)
	{
		if (digitNum > digit)
		{
			// 下から指定桁数を表示するためはみ出し範囲を切り捨て
			temp %= cutNum;
		}
		digitNum = digit;
	}
	// 一番下の桁から表示
	int posX = leftX - kFontGraphSizeX * size;
	int posY = y;
	for (int i = 0; i < digitNum; i++)
	{
		int no = temp % 10;

		/*DrawRectGraph(posX, posY,
			no * kFontGraphSizeX, 0, kFontGraphSizeX, kFontGraphSizeY,
			m_numGraphHandle, true);*/

			/// <summary>
		/// グラフィックを表示する
		/// </summary>
		/// <param name="left">描画したい矩形の左座標</param>
		/// <param name="top">描画したい矩形の左座標</param>
		/// <param name="width">描画するグラフィックのサイズ</param>
		/// <param name="height">描画するグラフィックのサイズ</param>
		/// <param name="scale">拡大率</param>
		/// <param name="angle">描画角度</param>
		/// <param name="handle">描画するグラフィックのハンドル</param>
		/// <param name="transFlg">画像透明処理の有無</param>
		/// <returns>画像表示</returns>
		DrawRectRotaGraph(posX, posY,
			no * kFontGraphSizeX, 0, kFontGraphSizeX, kFontGraphSizeY,
			size, 0.0f,
			m_numGraphHandle, true);

		temp /= 10;
		posX -= kFontGraphSizeX * size;
	}
}
