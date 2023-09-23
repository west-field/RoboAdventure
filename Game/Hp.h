#pragma once

class Hp
{
public:
	Hp();
	virtual ~Hp();

	void Draw();

	/// <summary>
	/// 最大HP
	/// </summary>
	/// <param name="hp">最大HP</param>
	void MaxHp(int hp);

	/// <summary>
	/// 現在のHPを取得する
	/// </summary>
	/// <returns>現在のHP</returns>
	const int GetHp()const { return m_nowHp; }

	/// <summary>
	/// 最大HPを取得する
	/// </summary>
	/// <returns>最大HP</returns>
	const int GetMaxHp()const { return m_maxHp; }

	/// <summary>
	/// ダメージ計算
	/// </summary>
	/// <param name="damage">受けたダメージ量</param>
	void Damage(const int damage);

	/// <summary>
	/// 回復計算
	/// </summary>
	/// <param name="heal">回復量</param>
	void Heal(const int heal);
private:
	/// <summary>
	/// 計算したHPの結果が最大値または0を超えていないか
	/// </summary>
	/// <param name="hp">計算したHP</param>
	void SetHp(const int hp);

	enum HpGraph
	{
		Full,
		Container,

		max
	};
	int m_handle[HpGraph::max];//画像ハンドル

	int m_maxHp;//最大HP
	int m_nowHp;//今のHP
};

