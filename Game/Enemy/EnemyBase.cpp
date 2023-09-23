#include "EnemyBase.h"
#include "../../Model.h"
#include "../../Util/Sound.h"

EnemyBase::EnemyBase(VECTOR pos, int modelHandle, float scale, bool isLeft,std::shared_ptr<Map> map)
{
	m_dir = VGet(0.0f, 0.0f, 0.0f);
	
	m_isLeft = isLeft;
	float rot = 95.0f;
	if (!m_isLeft)
	{
		rot *= -1.0f;
	}
	m_rot = VGet(0.0f, rot, 0.0f);
	m_prevPos = VGet(0.0f, 0.0f, 0.0f);

	m_pos = pos;
	m_vel = VGet(0.0f, 0.0f, 0.0f);

	m_model = std::make_shared<Model>(modelHandle, scale,true);
	m_model->SetPos(pos);
	m_model->SetRot(m_rot);

	m_isExist = true;

	m_isHit = false;

	m_map = map;

	//m_effect->Initialize(m_effectHandle, 516, 0);
}

EnemyBase::~EnemyBase()
{
}

const int EnemyBase::GetModel() const
{
	return m_model->GetModelHandle();
}

bool EnemyBase::IsExist()
{
	return m_isExist;
}

void EnemyBase::OnDamage()
{
	SoundManager::GetInstance().Play(SoundId::EnemyBurst);
	m_isHit = true;
}

int EnemyBase::AttackDamage()
{
	return 1;
}

void EnemyBase::EffectDraw()
{
	m_isExist = false;
}

void EnemyBase::Reverse()
{
	m_isLeft = !m_isLeft;
}

const VECTOR EnemyBase::GetEffectPos() const
{
	return VECTOR();
}

void EnemyBase::OnCamera()
{
	VECTOR LeftTopFlont, RightBottomBack;
	LeftTopFlont = VAdd(m_pos, rect[static_cast<int>(CollisionInfo::LTRB::leftTop)]);//�܂�����
	RightBottomBack = VAdd(m_pos, rect[static_cast<int>(CollisionInfo::LTRB::rightBottom)]);//�܂��E��

	LeftTopFlont = VAdd(LeftTopFlont, VGet(0.0f, 0.0f, size.z));//��O
	RightBottomBack = VAdd(RightBottomBack, VGet(0.0f, 0.0f, -size.z));//��

	//��̍��W�ł���킳���{�b�N�X���J�����̎��E�ɓ����Ă��Ȃ����ǂ����𔻒肷��
	//true:���E�ɂ͂����Ă��Ȃ�//false:���E�ɓ����Ă���
	if (CheckCameraViewClip_Box(LeftTopFlont, RightBottomBack))
	{
		//�J�����̎��E�ɓ����Ă��Ȃ��āA���݂��Ă��鎞
		if (m_isExist)
		{
			m_isExist = false;
			m_model->DeleteModel();
		}
	}
	else
	{
		//�J�����̎��E�ɓ����Ă��āA���݂��Ă��Ȃ��Ƃ�
		if (!m_isExist)
		{
			m_isExist = true;
		}
	}
}

void EnemyBase::SetLTRB(VECTOR leftTop, VECTOR leftBottom, VECTOR rightTop, VECTOR rightBottom)
{
	rect[static_cast<int>(LTRB::leftTop)] = leftTop;
	rect[static_cast<int>(LTRB::leftBottom)] = leftBottom;
	rect[static_cast<int>(LTRB::rightTop)] = rightTop;
	rect[static_cast<int>(LTRB::rightBottom)] = rightBottom;
}
