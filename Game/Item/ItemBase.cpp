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
	LeftTopFlont = VAdd(m_collision.GetCenter(), m_collision.rect[static_cast<int>(CollisionInfo::LTRB::leftTop)]);//まず左上
	RightBottomBack = VAdd(m_collision.GetCenter(), m_collision.rect[static_cast<int>(CollisionInfo::LTRB::rightBottom)]);//まず右下

	LeftTopFlont = VAdd(LeftTopFlont, VGet(0.0f, 0.0f, m_collision.size.z));//手前
	RightBottomBack = VAdd(RightBottomBack, VGet(0.0f, 0.0f, -m_collision.size.z));//奥

	//二つの座標であらわされるボックスがカメラの視界に入っていないかどうかを判定する
	//true:視界にはいっていない//false:視界に入っている
	if (CheckCameraViewClip_Box(LeftTopFlont, RightBottomBack))
	{
		//カメラの視界に入っていなくて、存在している時
		if (m_isExist)
		{
			m_isExist = false;
			m_model->DeleteModel();
		}
	}
	else
	{
		//カメラの視界に入っていて、存在していないとき
		if (!m_isExist)
		{
			m_isExist = true;
		}
	}
}
