#pragma once
#include "DxLib.h"

/// <summary>
/// �����蔻��
/// </summary>
struct CollisionInfo
{
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="isCol">�����蔻������邩�ǂ���</param>
	CollisionInfo(bool isCol);
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="isCol">�����蔻������邩�ǂ���</param>
	/// <param name="pos">���S�ʒu</param>
	/// <param name="si">�T�C�Y</param>
	CollisionInfo(bool isCol,const VECTOR pos,const VECTOR si);
	virtual ~CollisionInfo();//�f�X�g���N�^

	/// <summary>
	/// �㉺���E���w�肷�邱�Ƃŋ�`���\�z
	/// </summary>
	/// <param name="leftTop"></param>
	/// <param name="leftBottom"></param>
	/// <param name="rightTop"></param>
	/// <param name="rightBottom"></param>
	void SetLTRB(VECTOR leftTop, VECTOR leftBottom, VECTOR rightTop, VECTOR rightBottom);

	/// <summary>
	/// ���݂̒��S�ʒu���擾
	/// </summary>
	/// <returns>���S�ʒu</returns>
	const VECTOR& GetCenter()const;
	/// <summary>
	/// �傫�����擾����
	/// </summary>
	/// <returns>�傫��</returns>
	const VECTOR& GetSize()const;
	/// <summary>
	/// �ړ��͂��擾
	/// </summary>
	/// <returns>�ړ���</returns>
	const VECTOR& GetVel()const;

	/// <summary>
	/// �f�o�b�O�p��`�\��
	/// </summary>
	/// <param name="color">�F���w��ł���(�Ȃ��Ă�ok)</param>
	void Draw(unsigned int color = 0xffffff)const;

	/// <summary>
	/// ������̋�`�Ɠ����������ǂ���
	/// </summary>
	/// <param name="col">�����蔻��</param>
	/// <returns>true:���������@false:�O�ꂽ</returns>
	bool IsHit(const CollisionInfo& col)const;

	/// <summary>
	/// �����蔻������邩�ǂ���
	/// </summary>
	/// <returns>true:�����蔻������� false:�����蔻������Ȃ�</returns>
	const bool GetCollider() const { return isCollider; }

	VECTOR center;//���S�ʒu
	VECTOR size;//�T�C�Y
	VECTOR velocity;//�ړ���

	bool isCenter;//���f���ʒu�����S�ɂ��邩�ǂ���

	bool isCollider;//�����蔻������邩�ǂ���
	
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

	int draw = 0;
};

