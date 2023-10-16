#include "GameplayingScene.h"
#include "DxLib.h"
#include <cassert>

#include "SceneManager.h"
#include "TitleScene.h"
#include "PauseScene.h"
#include "GameoverScene.h"
#include "GameclearScene.h"

#include "../game.h"
#include "../Camera.h"
#include "../InputState.h"
#include "../Util/Sound.h"
#include "../Util/DrawFunctions.h"
#include "../Game/Player.h"
#include "../Game/Enemy/EnemyBase.h"
#include "../Game/EnemyFactory.h"
#include "../Game/Item/ItemBase.h"
#include "../Game/ItemFactory.h"
#include "../Game/Map.h"

namespace
{
	constexpr int kTitleFontSize = 80;//文字のサイズ
	constexpr int kFontSize = 50;//文字のサイズ

	//スター取得画像表示
	constexpr int kStarGraphSize = 60;//画像の大きさ
	constexpr int kStarGraphStartX = 50;//表示し始める位置X
	constexpr int kStarGraphStartY = 130;//表示し始める位置Y

	//数字画像
	constexpr int kFontGraphSizeX = 48;// 24;// 32;
	constexpr int kFontGraphSizeY = 74;// 37;// 15;

	constexpr int kScoreDrawPosX = kStarGraphStartX * 4;
	constexpr int kScoreDrawPosY = static_cast<int>(kStarGraphStartY + kStarGraphSize * 1.5f);
}

GameplayingScene::GameplayingScene(SceneManager& manager, const wchar_t* const fileName, const int select, std::shared_ptr<Camera> camera) :
	Scene(manager), m_updateFunc(&GameplayingScene::FadeInUpdat), m_drawFunc(&GameplayingScene::NormalDraw), m_selectStage(select),
	m_camera(camera)
{
	m_map = std::make_shared<Map>(fileName, 1.0f,select);
	m_player = std::make_shared<Player>(m_map);
	m_camera->CameraPosInit(m_player->GetPos(),false);
	m_enemyFactory = std::make_shared<EnemyFactory>(m_map);
	m_itemFactory = std::make_shared<ItemFactory>();
	
	m_fade = 0;
	m_fadetimer = 5;

	m_score = 0;
	m_addScore = 0;
	//BGM
	m_BgmH = LoadSoundMem(L"Data/Sound/BGM/playSound.mp3");
	ChangeVolumeSoundMem(0, m_BgmH);
	PlaySoundMem(m_BgmH, DX_PLAYTYPE_LOOP, true);

	m_starHandle = LoadGraph(L"Data/Img/star.png");
	m_starOutlineHandle = LoadGraph(L"Data/Img/star_outline.png");

	m_numGraphHandle = LoadGraph(L"Data/Img/num.png");
}

GameplayingScene::~GameplayingScene()
{
	m_player.reset();
	m_enemyFactory.reset();
	m_itemFactory.reset();

	DeleteSoundMem(m_BgmH);

	DeleteGraph(m_starHandle);
	DeleteGraph(m_starOutlineHandle);
	
	DeleteGraph(m_numGraphHandle);
}

void GameplayingScene::Update(const InputState& input)
{
	//マップのアップデート
	m_map->Update();
	(this->*m_updateFunc)(input);
}

void GameplayingScene::Draw()
{
	m_map->Draw();//マップを表示
	m_player->Draw();//プレイヤー表示

	PointDraw(kScoreDrawPosX, kScoreDrawPosY, m_score);//スコア表示

	//スターを表示
	bool getCoin[3] = {false};
	m_itemFactory->IsGetCoin(getCoin);
	for (int i = 0; i < 3; i++)
	{
		if (getCoin[i])
		{
			DrawExtendGraph(kStarGraphStartX + i * kStarGraphSize, kStarGraphStartY,
				kStarGraphStartX + i * kStarGraphSize + kStarGraphSize, kStarGraphStartY + kStarGraphSize, m_starHandle, true);
		}
		else
		{
			DrawExtendGraph(kStarGraphStartX + i * kStarGraphSize, kStarGraphStartY,
				kStarGraphStartX + i * kStarGraphSize + kStarGraphSize, kStarGraphStartY + kStarGraphSize, m_starOutlineHandle, true);
		}
	}

	(this->*m_drawFunc)();

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeValue);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameplayingScene::FadeInUpdat(const InputState& input)
{
	m_fadeValue = 255 * m_fadeTimer / kFadeInterval;
	ChangeVolumeSoundMem(SoundManager::GetInstance().GetBGMVolume() - m_fadeValue, m_BgmH);
	if (--m_fadeTimer == 0)
	{
		m_updateFunc = &GameplayingScene::NormalUpdat;
		m_fadeValue = 0;
		return;
	}
}
void GameplayingScene::FadeOutUpdat(const InputState& input)
{
	m_fadeValue = 255 * m_fadeTimer / kFadeInterval;
	ChangeVolumeSoundMem(SoundManager::GetInstance().GetBGMVolume() - m_fadeValue, m_BgmH);

	if (++m_fadeTimer == kFadeInterval)
	{
		bool getCoin[3] = {};
		m_itemFactory->IsGetCoin(getCoin);
		switch (m_crea)
		{
		case 0:
			m_manager.ChangeScene(new GameclearScene(m_manager, m_selectStage, m_score, m_camera, m_player->GetModel(), getCoin[0], getCoin[1], getCoin[2]));
			return;
		case 1:
			m_manager.ChangeScene(new GameoverScene(m_manager, m_selectStage,m_score, m_camera, m_player->GetModel(), getCoin[0], getCoin[1], getCoin[2]));
		default:
			return;
		}
	}
}

void GameplayingScene::NormalUpdat(const InputState& input)
{
	//カメラのアップデート
	m_camera->Update(m_player->GetPos());
	m_map->UpdateBg(m_camera->GetMove(), m_player->IsJump());

	//プレイヤーアップデート
	m_player->Update(input);

	//エネミー
	CreateEnemy();
	m_enemyFactory->Update();

	//アイテム
	CreateItem();
	m_itemFactory->Update();

	//プレイヤーとの当たり判定
	CollisionPlayer();

	float x, y;
	if (m_map->GetEventParam(m_player->GetPos().x, m_player->GetPos().y,x,y) == static_cast<int>(EventChipType::crea))
	{
		m_player->SwitchAnimation(PlayerAnimation::Dance, true, true, 4);
		m_updateFunc = &GameplayingScene::MoveCameraCloser;
		m_drawFunc = &GameplayingScene::GameclearDraw;
		m_fadeColor = 0xffffff;
		m_crea = 0;
		return;
	}
	//プレイヤーが死亡判定の場所にいるとき　または　プレイヤーの位置が-3.1f以下の時 または プレイヤーのHPが0の時 ゲームオーバー
	if (/*!m_player->OnCamera()*/m_map->GetEventParam(m_player->GetPos().x, m_player->GetPos().y, x, y) == static_cast<int>(EventChipType::deth) ||
		m_player->GetPos().y <= -3.1f || m_player->GetHp() == 0)
	{
		m_player->SwitchAnimation(PlayerAnimation::Death, false, true, 4);
		m_updateFunc = &GameplayingScene::MoveCameraCloser;
		m_drawFunc = &GameplayingScene::GameoverDraw;
		m_fadeColor = 0xff0000;
		m_crea = 1;
		return;
	}
	

	//ポーズ画面
	if (input.IsTriggered(InputType::pause))
	{
		SoundManager::GetInstance().Play(SoundId::MenuOpen);
		m_manager.PushScene(new PauseScene(m_manager, m_BgmH));
		return;
	}
#ifdef _DEBUG
	if (input.IsTriggered(InputType::next))
	{
		m_player->SwitchAnimation(PlayerAnimation::Dance, true, true, 4);
		m_updateFunc = &GameplayingScene::MoveCameraCloser;
		m_drawFunc = &GameplayingScene::GameclearDraw;
		m_fadeColor = 0x000000;
		m_crea = 0;
		return;
	}
	if (input.IsTriggered(InputType::prev))
	{
		m_player->SwitchAnimation(PlayerAnimation::Death, false, true, 4);
		m_updateFunc = &GameplayingScene::MoveCameraCloser;
		m_drawFunc = &GameplayingScene::GameoverDraw;
		m_fadeColor = 0xff0000;
		m_crea = 1;
		return;
	}
#endif
}

void GameplayingScene::MoveCameraCloser(const InputState& input)
{
	//プレイヤーの方向にカメラを移動する
	VECTOR vel = VSub(m_player->GetPos(), GetCameraPosition());
	//ゼロ除算避け
	if (VSquareSize(vel) > 0)
	{
		// 正規化
		vel = VNorm(vel);
	}
	else
	{
		vel = VGet(0.0f, 0.0f, 1.0f);
	}
	//スピードをかける
	vel = VScale(vel, 0.2f);

	m_camera->moveUpdate(vel);
	if (GetCameraPosition().z > -5.0f)
	{
		switch (m_crea)
		{
		case 0:
			//残っていたHP分だけ得点を追加する
			m_addScore = m_player->GetHp() * 100;
			m_updateFunc = &GameplayingScene::GameclearUpdate;
			break;
		case 1:
			m_updateFunc = &GameplayingScene::GameoverUpdate;
			break;
		default:
			break;
		}
	}
}

//死ぬアニメーションをして終わってすこし時間がたったら次へ進める
void GameplayingScene::GameoverUpdate(const InputState& input)
{
	m_player->Update();

	//ボタンを押すか時間がたったら次のシーンへ移動する
	m_fade += m_fadetimer;
	if (m_fade > 60 * 5 || input.IsTriggered(InputType::next))
	{
		m_updateFunc = &GameplayingScene::FadeOutUpdat;
		return;
	}
}

//クリアのアニメーションをしてクリックされたら次へ進む
void GameplayingScene::GameclearUpdate(const InputState& input)
{
	m_player->Update();

	//プレイヤーにその位置で踊りを踊らせる

	//プッシュの文字のフェード
	if (m_fade < 0 || m_fade > 255)
	{
		m_fadetimer *= -1;
	}
	m_fade += m_fadetimer;

	if (m_addScore != 0)
	{
		m_addScore--;
		m_score++;
		SoundManager::GetInstance().Play(SoundId::Score);//スコア取得音を鳴らす
	}

	if (input.IsTriggered(InputType::next))
	{
		if (m_addScore != 0)
		{
			m_score += m_addScore;
		}

		SoundManager::GetInstance().Play(SoundId::Determinant);
		m_updateFunc = &GameplayingScene::FadeOutUpdat;
		return;
	}
}

void GameplayingScene::NormalDraw()
{
	m_enemyFactory->Draw();//敵を表示
	m_itemFactory->Draw();//アイテムを表示
}

//TODO:何も表示しないか、すこし赤くするか
void GameplayingScene::GameoverDraw()
{
	//何も表示しない?
}

//TODO:クリックしてみたいなのを表示させる
void GameplayingScene::GameclearDraw()
{
	//クラッカーみたいなもの

	SetFontSize(kFontSize);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fade);
	DrawString(((Game::kScreenWidth / 2) - (kFontSize * 9 / 2)), Game::kScreenHeight - kTitleFontSize * 2, L"Press Key or Button", 0xffffff);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetFontSize(0);
}

void GameplayingScene::CollisionPlayer()
{
	bool isHit = false;//プレイヤーと当たったかどうか

	for (auto& enemy : m_enemyFactory->GetEnemies())
	{
		//敵が存在していないとき処理をしない
		if (!enemy->IsExist())	continue;
		//敵がすでに攻撃を受けている時(もう当たらないから)処理をしない
		if (enemy->IsBeAttack())	continue;
		//プレイヤーが攻撃を受けている時は処理をしない
		if (m_player->IsDamage())	continue;

		//プレイヤー(カプセル)とエネミー(モデル)の当たり判定
		isHit = m_player->HitModel(enemy->GetModel());
		//当たっていないときは処理をしない
		if (!isHit)	continue;

		//攻撃されたとき
		if (!m_player->IsAttack())
		{
			m_player->OnDamage(enemy->AttackDamage());
		}
		//攻撃したとき
		else
		{
			enemy->OnDamage();
			m_player->Attack();
			m_score += enemy->Score();
		}
	}

	//プレイヤーとアイテムの当たり判定
	for (auto& item : m_itemFactory->GetItems())
	{
		if (!item->IsExist())	continue;

		//プレイヤー(カプセル)とエネミー(モデル)の当たり判定
		isHit = m_player->HitModel(item->GetModel());

		//当たっていないときは処理をしない
		if (!isHit)	continue;
		
		item->IsHit();
		m_score += item->Score();

		break;
	}
}

void GameplayingScene::CreateEnemy()
{
	int width, height, enemyChip;
	VECTOR pos;
	EnemyType type = EnemyType::StraightLeft;

	m_map->GetMapSize(width, height);
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			enemyChip = m_map->GetChipId(LayerType::Enemy, x, y);
			if (enemyChip == 0)	continue;
			pos = VGet((0.65f * 2) * x, 7.5f - (0.65f * 2) * y, 0.0f);
			//指定の座標がカメラの視界内に入っていないかどうかを判定する
			//true:視界に入っていない false:視界に入っている
			if (CheckCameraViewClip(pos))
			{
				//視界に入っていないときは処理をしない
				continue;
			}

			//視界に入っている時
			//敵を一度だけ生成する
			switch (enemyChip)
			{
			case static_cast<int>(EnemyChipType::straightLeft):
				type = EnemyType::StraightLeft;
				break;
			case static_cast<int>(EnemyChipType::straightRight):
				type = EnemyType::StraightRight;
				break;
			case static_cast<int>(EnemyChipType::straightFly):
				type = EnemyType::StraightFly;
				break;
			default:
				break;
			}
			m_enemyFactory->Create(type, pos);
			m_map->GetEnemyParam(x, y);
		}
	}
}

void GameplayingScene::CreateItem()
{
	int width, height, itemChip;
	VECTOR pos;
	ItemType type = ItemType::Coin1;
	m_map->GetMapSize(width, height);
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			itemChip = m_map->GetChipId(LayerType::Item, x, y);
			if (itemChip == 0)	continue;
			pos = VGet((0.65f * 2) * x, 7.5f - (0.65f * 2) * y, 0.0f);
			if (CheckCameraViewClip(pos))	continue;
			//視界に入っている時一度だけ生成する
			switch (itemChip)
			{
			case static_cast<int>(ItemChipType::coin1):
				type = ItemType::Coin1;
				break;
			case static_cast<int>(ItemChipType::coin2):
				type = ItemType::Coin2;
				break;
			case static_cast<int>(ItemChipType::coin3):
				type = ItemType::Coin3;
				break;
			default:
				break;
			}
			m_itemFactory->Create(type, pos);
			m_map->GetItemParam(x, y);
		}
	}
}

void GameplayingScene::PointDraw(int leftX, int y, int dispNum, float size, int digit)
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
	int posX = static_cast<int>(leftX - kFontGraphSizeX * size);
	int posY = y;
	for (int i = 0; i < digitNum; i++)
	{
		int no = temp % 10;

		DrawRectRotaGraph(posX, posY,
			no * kFontGraphSizeX, 0, kFontGraphSizeX, kFontGraphSizeY,
			size, 0.0f,
			m_numGraphHandle, true);

		temp /= 10;
		posX -= static_cast<int>(kFontGraphSizeX * size);
	}
}
