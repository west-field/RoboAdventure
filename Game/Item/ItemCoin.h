#pragma once
#include "ItemBase.h"

/// <summary>
/// �R�C��
/// </summary>
class ItemStar : public ItemBase
{
public:
	ItemStar(VECTOR pos, int modelHandle,int number);
	virtual ~ItemStar();

	void Update()override;
	void Draw()override;

	int Score()override;

	void IsHit() override;

	const float GetSize()const;
private:
	float m_angle;//��]�p�x
};

