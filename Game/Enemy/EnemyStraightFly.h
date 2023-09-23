#pragma once
#include "EnemyBase.h"

/// <summary>
/// �܂������ړ������ԓG
/// </summary>
class EnemyStraightFly : public EnemyBase
{
public:
	EnemyStraightFly(VECTOR pos, int modelHandle, bool isLeft, std::shared_ptr<Map> map);
	virtual ~EnemyStraightFly();

	void Update()override;
	void Draw()override;
	int Score()override;
	//�ړ�����
	void Move()override;

	const VECTOR GetEffectPos() const override;
private:
	enum class Animation
	{
		Bite_Front = 0,
		Death = 1,
		Flying = 2,
		HitRecieve = 3,
	};

};

