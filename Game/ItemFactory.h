#pragma once
#include <DxLib.h>
#include <memory>
#include <list>
#include <map>

class ItemBase;

enum class ItemType
{
	Coin1,//�X�^�[�R�C��
	Coin2,//�X�^�[�R�C��
	Coin3,//�X�^�[�R�C��
};

/// <summary>
/// �A�C�e���H��
/// </summary>
class ItemFactory
{
public:
	ItemFactory();
	virtual ~ItemFactory();

	void Update();
	void Draw();

	void Create(ItemType type, const VECTOR pos);

	std::list<std::shared_ptr<ItemBase>> GetItems();

	void IsGetCoin(bool data[3]);

	void GetCoin();
private:
	std::list<std::shared_ptr<ItemBase>> m_items;//�A�C�e��
	std::map<ItemType, int> m_handleMap;//�A�C�e���n���h��

	bool m_isGetCoin[3];
};

