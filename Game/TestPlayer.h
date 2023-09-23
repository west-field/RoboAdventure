#pragma once
#include "DxLib.h"
#include <memory>
#include "../CollisionInfo.h"

class Model;
class InputState;
class Hp;

/// <summary>
/// �v���C���[�A�j���[�V����
/// </summary>
enum class PlayerAnimation
{
	Dance = 0,
	Death = 1,
	Idle = 2,
	Jump = 3,
	No = 4,
	Punch = 5,
	Run = 6,
	ThumbsUp = 9,
	Walk = 10,
	WalkJump = 11,
	Wave = 12,
	Yes = 13,
};

/// <summary>
/// �v���C���[
/// </summary>
class TestPlayer
{
public:
	TestPlayer();//�R���X�g���N�^
	virtual ~TestPlayer();//�f�X�g���N�^

	/// <summary>
	/// �X�V
	/// </summary>
	/// <param name="input">����</param>
	void Update(const InputState& input);
	void Update();
	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

	/// <summary>
	/// ���݂̈ʒu���擾����
	/// </summary>
	/// <returns>���݈ʒu</returns>
	const VECTOR GetPos() const;
	/// <summary>
	/// �ʒu��ݒ肷��
	/// </summary>
	void SetPos(VECTOR pos);
	/// <summary>
	/// ���݂̃��f���̈ʒu
	/// </summary>
	/// <returns>���f���̈ʒu</returns>
	const VECTOR GetModelPos()const;
	/// <summary>
	/// �T�C�Y���擾����
	/// </summary>
	/// <returns>�T�C�Y</returns>
	const VECTOR GetSize() const;
	/// <summary>
	/// �ړ���
	/// </summary>
	/// <returns>�ړ�</returns>
	const VECTOR& GetVel() const;
	/// <summary>
	/// �ړ��͂�ύX
	/// </summary>
	/// <param name="vel">�ړ�</param>
	void SetVel(VECTOR vel);
	/// <summary>
	/// �ړ���
	/// </summary>
	/// <returns>�ړ�</returns>
	const VECTOR& GetDir() const;

	/// <summary>
	/// �ړ�����
	/// </summary>
	void Move();
	/// <summary>
	/// �ړ����Ȃ��Ƃ����̈ʒu�ɖ߂�
	/// </summary>
	void NoMove(int hit);

	/// <summary>
	/// ��������
	/// </summary>
	void Drop();

	/// <summary>
	/// �����蔻����擾����
	/// </summary>
	/// <returns>�����蔻��</returns>
	const CollisionInfo& GetCol() const { return m_colision; }

	const int GetHp()const;

	/// <summary>
	/// �J�����͈͓̔��ɂ��邩�ǂ���
	/// </summary>
	/// <returns>true:�͈͓� false:�͈͊O</returns>
	bool OnCamera();

	/// <summary>
	/// �u���b�N�ɓ���������
	/// </summary>
	void HitBlockUp();

	/// <summary>
	/// �u���b�N�ɓ���������
	/// </summary>
	void HitBlockWidth();

	/// <summary>
	/// �W�����v���Ă��邩
	/// </summary>
	/// <returns>true:���Ă���@false:���Ă��Ȃ�</returns>
	bool IsJump()const { return m_isJump; }

	/// <summary>
	/// �ǂ����������Ă��邩
	/// </summary>
	/// <returns></returns>
	bool IsLeft()const { return m_isLeft; }

	/// <summary>
	/// �U��(�G�l�~�[)�ɓ�������
	/// </summary>
	void OnDamage();

	/// <summary>
	/// �U�����󂯂Ă���
	/// </summary>
	/// <returns></returns>
	bool IsDamage();

	/// <summary>
	/// �v���C���[�̈ړ��X�s�[�h���擾����
	/// </summary>
	/// <returns>�ړ��X�s�[�h</returns>
	const float GetSpeed()const;

	/// <summary>
	/// �U�����������ǂ���
	/// </summary>
	void Attack();

	/// <summary>
	/// �A�j���[�V������؂�ւ���
	/// </summary>
	/// <param name="animNo">�ύX��A�j���[�V����</param>
	/// <param name="isLoop">�A�j���[�V���������[�v�����邩</param>
	/// <param name="isForceChange">���łɎw�肳�ꂽ�A�j�����Đ�����Ă���ꍇ���ύX���邩</param>
	/// <param name="changeFrame">�t���[����</param>
	void SwitchAnimation(PlayerAnimation animNo, bool isLoop, bool isForceChange, int changeFrame);

	const std::shared_ptr<Model> GetModel()const { return m_model; }
private:

	/// <summary>
	/// ���Ɉړ�
	/// </summary>
	void MoveLeft();
	/// <summary>
	/// �E�Ɉړ�
	/// </summary>
	void MoveRight();

	/// <summary>
	/// �ҋ@�A�j���[�V�����A�b�v�f�[�g
	/// </summary>
	/// <param name="input"></param>
	void UpdateIdle(const InputState& input);
	/// <summary>
	/// �W�����v�A�b�v�f�[�g
	/// </summary>
	/// <param name="input"></param>
	void UpdateJump(const InputState& input);

	void (TestPlayer::* m_animUpdate)(const InputState& input);//Update�p�����o�֐��|�C���^

	VECTOR	m_dir;//��]����
	VECTOR m_rot;//�\������

	std::shared_ptr<Model> m_model;//���f��
	CollisionInfo m_colision;//�����蔻��
	std::shared_ptr<Hp> m_hp;//HP

	VECTOR m_prevPos;//���̈ʒu

	PlayerAnimation m_animType;//�A�j���[�V�����^�C�v

	float m_jumpAdd;//�W�����v
	bool m_isJump;//�W�����v���Ă��邩

	bool m_isLeft;//�������Ă��邩

	bool m_isDamage;
	int m_flashing;//�U���ɓ����������A�_�ł�����
};

