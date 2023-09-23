#pragma once
#include "EnemyBase.h"

/// <summary>
/// ‚Ü‚Á‚·‚®ˆÚ“®‚·‚é“G
/// </summary>
class EnemyStraight : public EnemyBase
{
public:
	EnemyStraight(VECTOR pos, int modelHandle, bool isLeft, std::shared_ptr<Map> map);
	virtual ~EnemyStraight();

	void Update()override;
	void Draw()override;
	int Score()override;
	//ˆÚ“®‚·‚é
	void Move()override;

	const VECTOR GetEffectPos() const override;
private:
	
	enum class Animation
	{
		Bite_Front = 0,
		Bite_InPlace = 1,
		Dance = 2,
		Death = 3,
		HitRecieve = 4,
		Idle = 5,
		Jump = 6,
		No = 7,
		Walk = 8,
		Yes = 9
	};
};

