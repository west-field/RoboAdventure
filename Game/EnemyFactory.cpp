#include "EffekseerForDxLib.h"

#include "EnemyFactory.h"
#include "Enemy/EnemyStraight.h"
#include "Enemy/EnemyStraightFly.h"

#include <cassert>

EnemyFactory::EnemyFactory(std::shared_ptr<Map> map)
{
	m_handleMap[EnemyType::StraightLeft] = MV1LoadModel(L"Data/Model/Enemy.mv1");
	m_handleMap[EnemyType::StraightRight] = MV1DuplicateModel(m_handleMap[EnemyType::StraightLeft]);
	m_handleMap[EnemyType::StraightFly] = MV1LoadModel(L"Data/Model/Bee.mv1");

	m_map = map;

	m_effectHandle = LoadEffekseerEffect("Data/Effect/ToonHit.efk",0.3f);
	assert(m_effectHandle != -1);
}

EnemyFactory::~EnemyFactory()
{
	for (auto& handle : m_handleMap)
	{
		MV1DeleteModel(handle.second);
	}

	DeleteEffekseerEffect(m_effectHandle);
}

void EnemyFactory::Update()
{
	//���Ȃ��Ȃ����G������
	//�����ɓ��Ă͂܂�G�����ɂ悯�Ă���
	auto rmIt = std::remove_if(m_enemies.begin(), m_enemies.end(),
		[](const std::shared_ptr<EnemyBase>& enemy) {
			return !enemy->IsExist();
		});
	//���ۂɔ͈͂��w�肵�ď����Ă���
	m_enemies.erase(rmIt, m_enemies.end());

	//�X�V
	for (auto& enemy : m_enemies)
	{
		if (enemy->IsExist())
		{
			if (enemy->IsBeAttack())
			{
				//�G�t�F�N�g�̔���
				if (IsEffekseer3DEffectPlaying(m_playingEffect))
				{
					StopEffekseer3DEffect(m_playingEffect);
				}

				//�G�t�F�N�g�𔭐�
				m_playingEffect = PlayEffekseer3DEffect(m_effectHandle);

				//�G�t�F�N�g�̈ʒu�w��
				VECTOR pos = enemy->GetEffectPos();
				SetPosPlayingEffekseer3DEffect(m_playingEffect, pos.x, pos.y, pos.z);

				enemy->EffectDraw();
			}
			else
			{
				enemy->Update();
			}
		}
	}
}

void EnemyFactory::Draw()
{
	for (auto& enemy : m_enemies)
	{
		if (enemy->IsExist())
		{
			enemy->Draw();
		}
	}

#if _DEBUG
	if (IsEffekseer3DEffectPlaying(m_playingEffect))
	{
		DrawString(0, 100, L"EffectPlay", 0x000000);
	}
	else
	{
		DrawString(0, 100, L"EffectNoPlay", 0x000000);
	}
#endif
}

std::shared_ptr<EnemyBase> EnemyFactory::Create(EnemyType type, const VECTOR pos)
{
	switch (type)
	{
	case EnemyType::StraightLeft:
		m_enemies.push_back(
			std::make_shared<EnemyStraight>(pos, m_handleMap[EnemyType::StraightLeft], true, m_map));
		break;
	case EnemyType::StraightRight:
		m_enemies.push_back(
			std::make_shared<EnemyStraight>(pos, m_handleMap[EnemyType::StraightRight], false, m_map));
		break;
	case EnemyType::StraightFly:
		m_enemies.push_back(
			std::make_shared<EnemyStraightFly>(pos, m_handleMap[EnemyType::StraightFly], true, m_map));
		break;
	default:
		break;
	}
	return m_enemies.back();//�����̗v�f��Ԃ�
}

std::list<std::shared_ptr<EnemyBase>>  EnemyFactory::GetEnemies()
{
	return m_enemies;
}