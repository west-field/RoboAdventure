#pragma once

/// <summary>
/// �x�N�g���\����
/// �v���C���[�̍��W�⑬�x��\�����߂̂���
/// </summary>
struct Vector2
{
	float x;
	float y;

	Vector2() : x(0),y(0){}
	Vector2(float inx,float iny):x(inx),y(iny){}

	//���Z
	Vector2 operator+(const Vector2& rval)const
	{
		return { x + rval.x,y + rval.y };
	}
	//���Z
	Vector2 operator-(const Vector2& rval)const
	{
		return { x - rval.x,y - rval.y };
	}
	void operator+=(const Vector2& rval);
	void operator-=(const Vector2& rval);

	//�X�J���[�{,�X�J���[��
	Vector2 operator*(float scale)const;
	Vector2 operator/(float div)const;
	void operator*=(float scale);
	void operator/=(float div);

	//�x�N�^�[�t�]�i-�j
	Vector2 operator-()const;

	//�x�N�g���̑傫����Ԃ�
	float Length()const;
	//�x�N�g���̑傫����2���Ԃ�
	float SQLength()const;
	//�x�N�g���̐��K��
	void Normalize();
	//���K�������x�N�g����Ԃ�
	Vector2 GetNormalized()const;
};

//���W��\���ʖ�
using Position2 = Vector2;

//�T�C�Y���܂Ƃ߂�\����
struct Size
{
	int w;//��
	int h;//����
};
//��`�\����
struct Rect
{
	Position2 center;//��`�̒��S�ʒu
	Size size;//�T�C�Y

	Rect();
	Rect(const Position2& c, const Size& sz);

	/// <summary>
	/// ����E�����w�肷�邱�Ƃŋ�`���\�z
	/// </summary>
	/// <param name="left">��</param>
	/// <param name="top">��</param>
	/// <param name="right">�E</param>
	/// <param name="bottom">��</param>
	void SetLTRB(int left, int top, int right, int bottom);

	int Left()const;
	int Top()const;
	int Right()const;
	int Bottom()const;

	const Position2& GetCenter()const;
	const Size& GetSize()const;

	///�f�o�b�O�p��`�\��
	void Draw(unsigned int color = 0xffffff)const;
	/// <summary>
	/// ������̋�`�Ɠ����������ǂ���
	/// </summary>
	/// <param name="rc">��`</param>
	/// <returns>true�F���������@false�F�O�ꂽ</returns>
	bool IsHit(const Rect& rc)const;
};

