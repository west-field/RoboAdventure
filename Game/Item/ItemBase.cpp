#include "ItemBase.h"
#include <DxLib.h>
#include "../../Model.h"

ItemBase::ItemBase(VECTOR pos, int modelHandle, float scale):m_collision(true)
{
	m_dir = VGet(0.0f, 0.0f, 0.0f);
	m_rot = VGet(0.0f, 0.0f, 0.0f);

	m_model = std::make_shared<Model>(modelHandle, scale,true);
	m_model->SetPos(pos);
	m_model->SetRot(m_rot);

	m_collision.center = pos;
	m_collision.velocity = VGet(0.0f, 0.0f, 0.0f);

	m_isExist = true;

	m_coinNumber = 0;
}

ItemBase::~ItemBase()
{
	m_model.reset();
}

const int ItemBase::GetModel() const
{
	return m_model->GetModelHandle();
}

void ItemBase::OnCamera()
{
	VECTOR LeftTopFlont, RightBottomBack;
	LeftTopFlont = VAdd(m_collision.GetCenter(), m_collision.rect[static_cast<int>(CollisionInfo::LTRB::leftTop)]);//�܂�����
	RightBottomBack = VAdd(m_collision.GetCenter(), m_collision.rect[static_cast<int>(CollisionInfo::LTRB::rightBottom)]);//�܂��E��

	LeftTopFlont = VAdd(LeftTopFlont, VGet(0.0f, 0.0f, m_collision.size.z));//��O
	RightBottomBack = VAdd(RightBottomBack, VGet(0.0f, 0.0f, -m_collision.size.z));//��

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
