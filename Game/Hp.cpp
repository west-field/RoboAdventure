#include "Hp.h"
#include "DxLib.h"
#include "../Model.h"
#include "../Util/DrawFunctions.h"

namespace
{
	constexpr int kMaxHp = 2;//HP最大値指定がなかった時の最大値

	constexpr int kGraphSizeContainerW = 153;
	constexpr int kGraphSizeContainerH = 150;

	constexpr int kGraphSizeFullW = 123;
	constexpr int kGraphSizeFullH = 126;
}

Hp::Hp()
{
	m_handle[HpGraph::Full] = LoadGraph(L"Data/Img/HeatFull.png");
	m_handle[HpGraph::Container] = LoadGraph(L"Data/Img/HeartContainer.png");

	m_maxHp = kMaxHp;
	m_nowHp = m_maxHp;
}

Hp::~Hp()
{
	for (auto& handle : m_handle)
	{
		DeleteGraph(handle);
	}
}

//表示
void Hp::Draw()
{
	int x = kGraphSizeContainerW / 2;
	int y = kGraphSizeContainerH / 2;
	for (int i = 0; i < m_maxHp; i++)
	{
		my::MyDrawRectRotaGraph(x, y, 0, 0, kGraphSizeContainerW, kGraphSizeContainerH, 
			0.8f, 0.0f, m_handle[HpGraph::Container], true, false);
		if (i < m_nowHp)
		{
			my::MyDrawRectRotaGraph(x +10 , y - 5, 0, 0, kGraphSizeFullW, kGraphSizeFullH,
				0.8f, 0.0f, m_handle[HpGraph::Full], true, false);
		}
		x += static_cast<int>(kGraphSizeContainerW * 0.8f);
	}

}

//最大値を設定する
void Hp::MaxHp(int hp)
{
	m_maxHp = hp;
	m_nowHp = m_maxHp;
}

//ダメージ計算
void Hp::Damage(const int damage)
{
	SetHp(m_nowHp - damage);
}

//回復計算
void Hp::Heal(const int heal)
{
	SetHp(m_nowHp + heal);
}

//HPが最大値または0をこえていないか
void Hp::SetHp(const int hp)
{
	//最大値よりも大きいとき
	if (hp >= m_maxHp)
	{
		m_nowHp = m_maxHp;
	}
	//0よりも小さいとき
	else if (hp < 0)
	{
		m_nowHp = 0;
	}
	else
	{
		m_nowHp = hp;
	}
}
