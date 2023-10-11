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
	constexpr int kTitleFontSize = 80;//�����̃T�C�Y
	constexpr int kFontSize = 50;//�����̃T�C�Y

	//�X�^�[�擾�摜�\��
	constexpr int kStarGraphSize = 60;//�摜�̑傫��
	constexpr int kStarGraphStartX = 50;//�\�����n�߂�ʒuX
	constexpr int kStarGraphStartY = 130;//�\�����n�߂�ʒuY

	//�����摜
	constexpr int kFontGraphSizeX = 48;// 24;// 32;
	constexpr int kFontGraphSizeY = 74;// 37;// 15;

	constexpr int kScoreDrawPosX = kStarGraphStartX * 4;
	constexpr int kScoreDrawPosY = kStarGraphStartY + kStarGraphSize * 1.5f;
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
	//�}�b�v�̃A�b�v�f�[�g
	m_map->Update();
	(this->*m_updateFunc)(input);
}

void GameplayingScene::Draw()
{
	m_map->Draw();//�}�b�v��\��
	//m_player->Draw();//�v���C���[�\��

	PointDraw(kScoreDrawPosX, kScoreDrawPosY, m_score);//�X�R�A�\��

	//�X�^�[��\��
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
	//�J�����̃A�b�v�f�[�g
	m_camera->Update(m_player->GetPos());
	m_map->UpdateBg(m_camera->GetMove(), m_player->IsJump());

	//�v���C���[�A�b�v�f�[�g
	m_player->Update(input);

	//�G�l�~�[
	CreateEnemy();
	m_enemyFactory->Update();

	//�A�C�e��
	CreateItem();
	m_itemFactory->Update();

	//�v���C���[�Ƃ̓����蔻��
	CollisionPlayer();

	if (m_map->GetEventParam(m_player->GetPos().x, m_player->GetPos().y) == static_cast<int>(EventChipType::crea))
	{
		m_player->SwitchAnimation(PlayerAnimation::Dance, true, true, 4);
		m_updateFunc = &GameplayingScene::MoveCameraCloser;
		m_drawFunc = &GameplayingScene::GameclearDraw;
		m_fadeColor = 0xffffff;
		m_crea = 0;
		return;
	}
	//�v���C���[���J�����͈̔͊O�ɂ���Ƃ� �܂��� �v���C���[��HP��0�̎� �Q�[���I�[�o�[
	if (!m_player->OnCamera() || m_player->GetHp() == 0)
	{
		m_player->SwitchAnimation(PlayerAnimation::Death, false, true, 4);
		m_updateFunc = &GameplayingScene::MoveCameraCloser;
		m_drawFunc = &GameplayingScene::GameoverDraw;
		m_fadeColor = 0xff0000;
		m_crea = 1;
		return;
	}
	

	//�|�[�Y���
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
	//�v���C���[�̕����ɃJ�������ړ�����
	VECTOR vel = VSub(m_player->GetPos(), GetCameraPosition());
	//�[�����Z����
	if (VSquareSize(vel) > 0)
	{
		// ���K��
		vel = VNorm(vel);
	}
	else
	{
		vel = VGet(0.0f, 0.0f, 1.0f);
	}
	//�X�s�[�h��������
	vel = VScale(vel, 0.2f);

	m_camera->moveUpdate(vel);
	if (GetCameraPosition().z > -5.0f)
	{
		switch (m_crea)
		{
		case 0:
			//�c���Ă���HP���������_��ǉ�����
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

//���ʃA�j���[�V���������ďI����Ă��������Ԃ��������玟�֐i�߂�
void GameplayingScene::GameoverUpdate(const InputState& input)
{
	m_player->Update();

	//�{�^�������������Ԃ��������玟�̃V�[���ֈړ�����
	m_fade += m_fadetimer;
	if (m_fade > 60 * 5 || input.IsTriggered(InputType::next))
	{
		m_updateFunc = &GameplayingScene::FadeOutUpdat;
		return;
	}
}

//�N���A�̃A�j���[�V���������ăN���b�N���ꂽ�玟�֐i��
void GameplayingScene::GameclearUpdate(const InputState& input)
{
	m_player->Update();

	//�v���C���[�ɂ��̈ʒu�ŗx���x�点��

	//�v�b�V���̕����̃t�F�[�h
	if (m_fade < 0 || m_fade > 255)
	{
		m_fadetimer *= -1;
	}
	m_fade += m_fadetimer;

	if (m_addScore != 0)
	{
		m_addScore--;
		m_score++;
		SoundManager::GetInstance().Play(SoundId::Score);//�X�R�A�擾����炷
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
	m_enemyFactory->Draw();//�G��\��
	m_itemFactory->Draw();//�A�C�e����\��
}

//TODO:�����\�����Ȃ����A�������Ԃ����邩
void GameplayingScene::GameoverDraw()
{
	//�����\�����Ȃ�?
}

//TODO:�N���b�N���Ă݂����Ȃ̂�\��������
void GameplayingScene::GameclearDraw()
{
	//�N���b�J�[�݂����Ȃ���

	SetFontSize(kFontSize);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fade);
	DrawString(((Game::kScreenWidth / 2) - (kFontSize * 9 / 2)), Game::kScreenHeight - kTitleFontSize * 2, L"Press Key or Button", 0xffffff);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetFontSize(0);
}

void GameplayingScene::CollisionPlayer()
{
	bool isHit = false;//�v���C���[�Ɠ����������ǂ���

	for (auto& enemy : m_enemyFactory->GetEnemies())
	{
		//�G�����݂��Ă��Ȃ��Ƃ����������Ȃ�
		if (!enemy->IsExist())	continue;
		//�G�����łɍU�����󂯂Ă��鎞(����������Ȃ�����)���������Ȃ�
		if (enemy->IsBeAttack())	continue;
		//�v���C���[���U�����󂯂Ă��鎞�͏��������Ȃ�
		if (m_player->IsDamage())	continue;

		//�v���C���[(�J�v�Z��)�ƃG�l�~�[(���f��)�̓����蔻��
		isHit = m_player->HitModel(enemy->GetModel());
		//�������Ă��Ȃ��Ƃ��͏��������Ȃ�
		if (!isHit)	continue;

		//�U�����ꂽ�Ƃ�
		if (!m_player->IsAttack())
		{
			m_player->OnDamage(enemy->AttackDamage());
		}
		//�U�������Ƃ�
		else
		{
			enemy->OnDamage();
			m_player->Attack();
			m_score += enemy->Score();
		}
	}

	//�v���C���[�ƃA�C�e���̓����蔻��
	for (auto& item : m_itemFactory->GetItems())
	{
		if (!item->IsExist())	continue;

		//�v���C���[(�J�v�Z��)�ƃG�l�~�[(���f��)�̓����蔻��
		isHit = m_player->HitModel(item->GetModel());

		//�������Ă��Ȃ��Ƃ��͏��������Ȃ�
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
			//�w��̍��W���J�����̎��E���ɓ����Ă��Ȃ����ǂ����𔻒肷��
			//true:���E�ɓ����Ă��Ȃ� false:���E�ɓ����Ă���
			if (CheckCameraViewClip(pos))
			{
				//���E�ɓ����Ă��Ȃ��Ƃ��͏��������Ȃ�
				continue;
			}

			//���E�ɓ����Ă��鎞
			//�G����x������������
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
			//���E�ɓ����Ă��鎞��x������������
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
	int cutNum = 10;	// �\�������w�莞�ɕ\���������߂邽�߂Ɏg�p����

	// �\�����鐔���̌���������
	while (temp > 0)
	{
		digitNum++;
		temp /= 10;
		cutNum *= 10;
	}
	if (digitNum <= 0)
	{
		digitNum = 1;	// 0�̏ꍇ��1���\������
	}

	// �\�������w��
	temp = dispNum;
	if (digit >= 0)
	{
		if (digitNum > digit)
		{
			// ������w�茅����\�����邽�߂͂ݏo���͈͂�؂�̂�
			temp %= cutNum;
		}
		digitNum = digit;
	}
	// ��ԉ��̌�����\��
	int posX = leftX - kFontGraphSizeX * size;
	int posY = y;
	for (int i = 0; i < digitNum; i++)
	{
		int no = temp % 10;

		DrawRectRotaGraph(posX, posY,
			no * kFontGraphSizeX, 0, kFontGraphSizeX, kFontGraphSizeY,
			size, 0.0f,
			m_numGraphHandle, true);

		temp /= 10;
		posX -= kFontGraphSizeX * size;
	}
}
