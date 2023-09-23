#pragma once
#include "Dxlib.h"
#include <memory>
#include "../../CollisionInfo.h"

class Model;
class Map;

class EnemyBase
{
public:
	EnemyBase(VECTOR pos,int modelHandle,float scale,bool isLeft, std::shared_ptr<Map> map);
	virtual ~EnemyBase();

	//�X�V
	virtual void Update() = 0;
	//�\��
	virtual void Draw() = 0;

	//�ړ�����
	virtual void Move() = 0;
	//�X�R�A���擾����
	virtual int Score() = 0;

	const int GetModel()const;

	/// <summary>
	/// ���݂��Ă��邩�ǂ������擾����
	/// </summary>
	/// <returns>true:���݂��Ă��� fasle:���݂��Ă��Ȃ�</returns>
	bool IsExist();

	/// <summary>
	/// �U�����󂯂�
	/// </summary>
	void OnDamage();

	/// <summary>
	/// �U���͂��擾����
	/// </summary>
	/// <returns>�U����</returns>
	int AttackDamage();

	/// <summary>
	/// �U�������łɎ󂯂Ă���
	/// </summary>
	/// <returns>ture:�U�����󂯂� false:�U�����󂯂ĂȂ�</returns>
	const bool IsBeAttack()const { return m_isHit; }

	//�G�t�F�N�g��\��
	void EffectDraw();

	/// <summary>
	/// ���΂�����
	/// </summary>
	void Reverse();

	virtual const VECTOR GetEffectPos() const = 0;
protected:
	/// <summary>
	/// �J�����͈͓̔��ɂ����瑶�݂��Ă���
	/// </summary>
	void OnCamera();
	/// <summary>
	/// �㉺���E���w�肷�邱�Ƃŋ�`���\�z
	/// </summary>
	/// <param name="leftTop"></param>
	/// <param name="leftBottom"></param>
	/// <param name="rightTop"></param>
	/// <param name="rightBottom"></param>
	void SetLTRB(VECTOR leftTop, VECTOR leftBottom, VECTOR rightTop, VECTOR rightBottom);

	VECTOR m_pos;//�\���ʒu
	VECTOR m_vel;//�ړ���
	VECTOR m_dir;//�\������
	VECTOR m_rot;//��]����

	//��`
	enum class LTRB
	{
		leftTop,
		leftBottom,
		rightTop,
		rightBottom,
		Max
	};
	VECTOR rect[static_cast<int>(LTRB::Max)];
	VECTOR size;//�T�C�Y

	VECTOR m_prevPos;//�O�̈ʒu

	std::shared_ptr<Model> m_model;//���f��

	std::shared_ptr<Map> m_map;//�����蔻��ɗp����}�b�v

	bool m_isLeft;//���������Ă��邩

	bool m_isExist;//���݂��邩�ǂ���

	bool m_isHit;//�U�����󂯂����ǂ���
};

