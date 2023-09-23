#pragma once

class Hp
{
public:
	Hp();
	virtual ~Hp();

	void Draw();

	/// <summary>
	/// �ő�HP
	/// </summary>
	/// <param name="hp">�ő�HP</param>
	void MaxHp(int hp);

	/// <summary>
	/// ���݂�HP���擾����
	/// </summary>
	/// <returns>���݂�HP</returns>
	const int GetHp()const { return m_nowHp; }

	/// <summary>
	/// �ő�HP���擾����
	/// </summary>
	/// <returns>�ő�HP</returns>
	const int GetMaxHp()const { return m_maxHp; }

	/// <summary>
	/// �_���[�W�v�Z
	/// </summary>
	/// <param name="damage">�󂯂��_���[�W��</param>
	void Damage(const int damage);

	/// <summary>
	/// �񕜌v�Z
	/// </summary>
	/// <param name="heal">�񕜗�</param>
	void Heal(const int heal);
private:
	/// <summary>
	/// �v�Z����HP�̌��ʂ��ő�l�܂���0�𒴂��Ă��Ȃ���
	/// </summary>
	/// <param name="hp">�v�Z����HP</param>
	void SetHp(const int hp);

	enum HpGraph
	{
		Full,
		Container,

		max
	};
	int m_handle[HpGraph::max];//�摜�n���h��

	int m_maxHp;//�ő�HP
	int m_nowHp;//����HP
};

