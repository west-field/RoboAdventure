#include "Hp.h"
#include "DxLib.h"
#include "../Model.h"
#include "../Util/DrawFunctions.h"

namespace
{
	constexpr int kMaxHp = 2;//HP�ő�l�w�肪�Ȃ��������̍ő�l

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

//�\��
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

//�ő�l��ݒ肷��
void Hp::MaxHp(int hp)
{
	m_maxHp = hp;
	m_nowHp = m_maxHp;
}

//�_���[�W�v�Z
void Hp::Damage(const int damage)
{
	SetHp(m_nowHp - damage);
}

//�񕜌v�Z
void Hp::Heal(const int heal)
{
	SetHp(m_nowHp + heal);
}

//HP���ő�l�܂���0�������Ă��Ȃ���
void Hp::SetHp(const int hp)
{
	//�ő�l�����傫���Ƃ�
	if (hp >= m_maxHp)
	{
		m_nowHp = m_maxHp;
	}
	//0�����������Ƃ�
	else if (hp < 0)
	{
		m_nowHp = 0;
	}
	else
	{
		m_nowHp = hp;
	}
}
