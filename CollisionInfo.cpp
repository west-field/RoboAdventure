#include "CollisionInfo.h"
//20230630_タイトル_名前
//コンストラクタ
CollisionInfo::CollisionInfo(bool isCol) :rect(), isCenter(true)
{
	isCollider = isCol;
	center = VGet(0.0f,0.0f,0.0f);
	size = VGet(0.0f, 0.0f, 0.0f);
	velocity = VGet(0.0f, 0.0f, 0.0f);
}
//コンストラクタ
CollisionInfo::CollisionInfo(bool isCol,const VECTOR pos, const VECTOR si):rect(), isCenter(true)
{
	isCollider = isCol;
	center = pos;
	size = si;
	velocity = VGet(0.0f, 0.0f, 0.0f);
}
//デストラクタ
CollisionInfo::~CollisionInfo()
{
}

//上下左右を指定することで矩形を構築
void CollisionInfo::SetLTRB(VECTOR leftTop, VECTOR leftBottom, VECTOR rightTop, VECTOR rightBottom)
{
	rect[static_cast<int>(LTRB::leftTop)] = leftTop;
	rect[static_cast<int>(LTRB::leftBottom)] = leftBottom;
	rect[static_cast<int>(LTRB::rightTop)] = rightTop;
	rect[static_cast<int>(LTRB::rightBottom)] = rightBottom;
}

//現在の中心位置を取得
const VECTOR& CollisionInfo::GetCenter() const
{
	return center;
}
//大きさを取得する
const VECTOR& CollisionInfo::GetSize() const
{
	return size;
}
//移動力を取得
const VECTOR& CollisionInfo::GetVel() const
{
	return velocity;
}

//デバッグ用矩形を表示
void CollisionInfo::Draw(unsigned int color) const
{
	DrawLine3D(VAdd(center, rect[static_cast<int>(LTRB::leftTop)]), VAdd(center, rect[static_cast<int>(LTRB::leftBottom)]), color);
	DrawLine3D(VAdd(center, rect[static_cast<int>(LTRB::leftTop)]), VAdd(center, rect[static_cast<int>(LTRB::rightTop)]), color);
	DrawLine3D(VAdd(center, rect[static_cast<int>(LTRB::rightTop)]), VAdd(center,rect[static_cast<int>(LTRB::rightBottom)]), color);
	DrawLine3D(VAdd(center, rect[static_cast<int>(LTRB::rightBottom)]), VAdd(center, rect[static_cast<int>(LTRB::leftBottom)]), color);
}

//もう一つの矩形と当たったかどうか
bool CollisionInfo::IsHit(const CollisionInfo& col)const 
{
#if true
	if (isCenter && col.isCenter)
	{
		//1左と2の右側の判定
		if ((center.x - col.center.x) < (-size.x + -col.size.x) / 2)
		{
			return false;
		}
		//1右と2左の判定
		if ((center.x - col.center.x) > (size.x + col.size.x) / 2)
		{
			return false;
		}
		//1下と2上
		if ((center.y - col.center.y) > (size.y + col.size.y) / 2)
		{
			return false;
		}
		//1上と2下
		if ((center.y - col.center.y) < (-size.y + -col.size.y) / 2)
		{
			return false;
		}
	}
	else if(!isCenter && col.isCenter)
	{
		//1左と2の右側の判定
		if ((center.x - col.center.x) < (-size.x + -col.size.x) / 2)
		{
			return false;
		}
		//1右と2左の判定
		if ((center.x - col.center.x) > (size.x + col.size.x) / 2)
		{
			return false;
		}
		//1下と2上
		if ((center.y - col.center.y) > (col.size.y) / 2)
		{
			return false;
		}
		//1上と2下
		if ((center.y - col.center.y) < (-size.y + -col.size.y) / 2)
		{
			return false;
		}
	}
	else if (isCenter && !col.isCenter)
	{
		//1左と2の右側の判定
		if ((center.x - col.center.x) < (-size.x + -col.size.x) / 2)
		{
			return false;
		}
		//1右と2左の判定
		if ((center.x - col.center.x) > (size.x + col.size.x) / 2)
		{
			return false;
		}
		//1下と2上
		if ((col.center.y - center.y) > (size.y) / 2)
		{
			return false;
		}
		//1上と2下
		if ((col.center.y - center.y) < (-size.y + -col.size.y) / 2)
		{
			return false;
		}
	}
	else
	{
		//1左と2の右側の判定
		if ((center.x - col.center.x) < (-size.x + -col.size.x) / 2)
		{
			return false;
		}
		//1右と2左の判定
		if ((center.x - col.center.x) > (size.x + col.size.x) / 2)
		{
			return false;
		}
		//1下と2上
		if ((center.y - col.center.y) > (size.y + col.size.y) / 2)
		{
			return false;
		}
		//1上と2下
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

		//1左と2の右側の判定
		if ((nextPos.x - col.center.x) > (-size.x + -col.size.x) / 2)
		{
			isHit = true;
		}
		//1右と2左の判定
		if ((nextPos.x - col.center.x) < (size.x + col.size.x) / 2)
		{
			isHit = true;
		}
		//1下と2上
		if ((nextPos.y - col.center.y) < (col.size.y) / 2)
		{
			isHit = true;
		}
		//1上と2下
		if ((nextPos.y - col.center.y) > (-size.y + -col.size.y) / 2)
		{
			isHit = true;
		}
	}
	return isHit;
#endif
}

