#include "CollisionInfo.h"
//20230630_�^�C�g��_���O
//�R���X�g���N�^
CollisionInfo::CollisionInfo(bool isCol) :rect(), isCenter(true)
{
	isCollider = isCol;
	center = VGet(0.0f,0.0f,0.0f);
	size = VGet(0.0f, 0.0f, 0.0f);
	velocity = VGet(0.0f, 0.0f, 0.0f);
}
//�R���X�g���N�^
CollisionInfo::CollisionInfo(bool isCol,const VECTOR pos, const VECTOR si):rect(), isCenter(true)
{
	isCollider = isCol;
	center = pos;
	size = si;
	velocity = VGet(0.0f, 0.0f, 0.0f);
}
//�f�X�g���N�^
CollisionInfo::~CollisionInfo()
{
}

//�㉺���E���w�肷�邱�Ƃŋ�`���\�z
void CollisionInfo::SetLTRB(VECTOR leftTop, VECTOR leftBottom, VECTOR rightTop, VECTOR rightBottom)
{
	rect[static_cast<int>(LTRB::leftTop)] = leftTop;
	rect[static_cast<int>(LTRB::leftBottom)] = leftBottom;
	rect[static_cast<int>(LTRB::rightTop)] = rightTop;
	rect[static_cast<int>(LTRB::rightBottom)] = rightBottom;
}

//���݂̒��S�ʒu���擾
const VECTOR& CollisionInfo::GetCenter() const
{
	return center;
}
//�傫�����擾����
const VECTOR& CollisionInfo::GetSize() const
{
	return size;
}
//�ړ��͂��擾
const VECTOR& CollisionInfo::GetVel() const
{
	return velocity;
}

//�f�o�b�O�p��`��\��
void CollisionInfo::Draw(unsigned int color) const
{
	DrawLine3D(VAdd(center, rect[static_cast<int>(LTRB::leftTop)]), VAdd(center, rect[static_cast<int>(LTRB::leftBottom)]), color);
	DrawLine3D(VAdd(center, rect[static_cast<int>(LTRB::leftTop)]), VAdd(center, rect[static_cast<int>(LTRB::rightTop)]), color);
	DrawLine3D(VAdd(center, rect[static_cast<int>(LTRB::rightTop)]), VAdd(center,rect[static_cast<int>(LTRB::rightBottom)]), color);
	DrawLine3D(VAdd(center, rect[static_cast<int>(LTRB::rightBottom)]), VAdd(center, rect[static_cast<int>(LTRB::leftBottom)]), color);
}

//������̋�`�Ɠ����������ǂ���
bool CollisionInfo::IsHit(const CollisionInfo& col)const 
{
#if true
	if (isCenter && col.isCenter)
	{
		//1����2�̉E���̔���
		if ((center.x - col.center.x) < (-size.x + -col.size.x) / 2)
		{
			return false;
		}
		//1�E��2���̔���
		if ((center.x - col.center.x) > (size.x + col.size.x) / 2)
		{
			return false;
		}
		//1����2��
		if ((center.y - col.center.y) > (size.y + col.size.y) / 2)
		{
			return false;
		}
		//1���2��
		if ((center.y - col.center.y) < (-size.y + -col.size.y) / 2)
		{
			return false;
		}
	}
	else if(!isCenter && col.isCenter)
	{
		//1����2�̉E���̔���
		if ((center.x - col.center.x) < (-size.x + -col.size.x) / 2)
		{
			return false;
		}
		//1�E��2���̔���
		if ((center.x - col.center.x) > (size.x + col.size.x) / 2)
		{
			return false;
		}
		//1����2��
		if ((center.y - col.center.y) > (col.size.y) / 2)
		{
			return false;
		}
		//1���2��
		if ((center.y - col.center.y) < (-size.y + -col.size.y) / 2)
		{
			return false;
		}
	}
	else if (isCenter && !col.isCenter)
	{
		//1����2�̉E���̔���
		if ((center.x - col.center.x) < (-size.x + -col.size.x) / 2)
		{
			return false;
		}
		//1�E��2���̔���
		if ((center.x - col.center.x) > (size.x + col.size.x) / 2)
		{
			return false;
		}
		//1����2��
		if ((col.center.y - center.y) > (size.y) / 2)
		{
			return false;
		}
		//1���2��
		if ((col.center.y - center.y) < (-size.y + -col.size.y) / 2)
		{
			return false;
		}
	}
	else
	{
		//1����2�̉E���̔���
		if ((center.x - col.center.x) < (-size.x + -col.size.x) / 2)
		{
			return false;
		}
		//1�E��2���̔���
		if ((center.x - col.center.x) > (size.x + col.size.x) / 2)
		{
			return false;
		}
		//1����2��
		if ((center.y - col.center.y) > (size.y + col.size.y) / 2)
		{
			return false;
		}
		//1���2��
		if ((center.y - col.center.y) < (-size.y + -col.size.y) / 2)
		{
			return false;
		}
	}

	return true;
#else
	bool isHit = false;
	if (!isCenter && col.isCenter)
	{
		VECTOR nextPos = VAdd(center, velocity);

		//1����2�̉E���̔���
		if ((nextPos.x - col.center.x) > (-size.x + -col.size.x) / 2)
		{
			isHit = true;
		}
		//1�E��2���̔���
		if ((nextPos.x - col.center.x) < (size.x + col.size.x) / 2)
		{
			isHit = true;
		}
		//1����2��
		if ((nextPos.y - col.center.y) < (col.size.y) / 2)
		{
			isHit = true;
		}
		//1���2��
		if ((nextPos.y - col.center.y) > (-size.y + -col.size.y) / 2)
		{
			isHit = true;
		}
	}
	return isHit;
#endif
}

