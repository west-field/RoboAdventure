#include "ItemCoin.h"
#include "../../Model.h"
#include "../../Util/Sound.h"

namespace
{
	constexpr float kRotSpeed = 0.05f;
	constexpr float kScale = 0.006f;//‘å‚«‚³Ý’è
	constexpr float kDrawScale = 0.5f;//•\Ž¦

	constexpr int kScore = 1000;
}

ItemStar::ItemStar(VECTOR pos, int modelHandle,int number) :
	ItemBase(pos, modelHandle, kScale)
{
	m_angle = 0.0f;

	m_coinNumber = number;

	m_collision.size = VGet(kDrawScale * 2, kDrawScale * 2, 0.0f);
	m_collision.isCenter = true;
	m_collision.SetLTRB(VGet(-kDrawScale, kDrawScale, 0.0f), VGet(-kDrawScale, -kDrawScale, 0.0f),
		VGet(kDrawScale, kDrawScale, 0.0f), VGet(kDrawScale, -kDrawScale, 0.0f));
}

ItemStar::~ItemStar()
{
}

void ItemStar::Update()
{
	m_model->UpdateCollision();

	m_angle += kRotSpeed;
	if (m_angle >= 180)
	{
		m_angle = 0.0f;
	}
	//í‚É‰ñ“]‚·‚é
	m_rot = VGet(0.0f, m_angle, 0.0f);
	m_model->SetRot(m_rot);
}

void ItemStar::Draw()
{
	m_model->Draw();
#ifdef _DEBUG
	m_collision.Draw();
#endif
}

int ItemStar::Score()
{
	return kScore;
}

void ItemStar::IsHit()
{
	SoundManager::GetInstance().Play(SoundId::StarGet);
	m_isExist = false;
}

const float ItemStar::GetSize() const
{
	return kDrawScale;
}
