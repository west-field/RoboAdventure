#pragma once
#include <DxLib.h>
#include <memory>
#include <list>
#include <map>

class ItemBase;

enum class ItemType
{
	Coin1,//スターコイン
	Coin2,//スターコイン
	Coin3,//スターコイン
};

/// <summary>
/// アイテム工場
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
	std::list<std::shared_ptr<ItemBase>> m_items;//アイテム
	std::map<ItemType, int> m_handleMap;//アイテムハンドル

	bool m_isGetCoin[3];
};

