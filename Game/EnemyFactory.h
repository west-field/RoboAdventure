#pragma once
#include "DxLib.h"
#include <list>
#include <memory>
#include <map>

class EnemyBase;
class Map;

enum class EnemyType
{
	StraightLeft,//まっすぐ移動する左を向いている敵
	StraightRight,//まっすぐ移動する右を向いている敵
	StraightFly,//まっすぐ飛ぶ敵
};

/// <summary>
/// 敵工場
/// </summary>
class EnemyFactory
{
public:
	EnemyFactory(std::shared_ptr<Map> map);
	virtual ~EnemyFactory();

	/// <summary>
	/// 敵全部の更新
	/// </summary>
	void Update();
	/// <summary>
	/// 敵全部の描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 敵オブジェクトの生成を行う
	/// 生成して、内部のリストで管理する
	/// </summary>
	/// <param name="type">敵種類</param>
	/// <param name="pos">生成座標</param>
	/// <returns>敵オブジェクト</returns>
	std::shared_ptr<EnemyBase> Create(EnemyType type, const VECTOR pos);

	/// <summary>
	/// 敵全てを取得する
	/// </summary>
	/// <returns>敵</returns>
	std::list<std::shared_ptr<EnemyBase>> GetEnemies();
private:
	std::list<std::shared_ptr<EnemyBase>> m_enemies;//敵

	std::map<EnemyType, int> m_handleMap;//敵のハンドル

	std::shared_ptr<Map> m_map;

	int m_effectHandle;//エフェクト
	int m_playingEffect;//エフェクトを表示
};

