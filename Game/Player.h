#pragma once
#include <DxLib.h>
#include <memory>

class InputState;
class Model;
class Map;
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

class Player
{
public:
	Player(std::shared_ptr<Map> map);
	virtual ~Player();

	void Update(const InputState& input);
	void Update();

	void Draw();

	//�ʒu���擾
	const VECTOR GetPos() const;

	/// <summary>
	/// ���f���Ɠ������Ă��邩�ǂ���
	/// </summary>
	/// <param name="model">���f��</param>
	bool HitModel(int model);
	
	//�_���[�W���󂯂�
	void OnDamage(int attack);
	//�_���[�W���󂯂Ă��邩
	bool IsDamage() { return m_isDamage; }
	//���݂�Hp���擾
	const int GetHp()const;

	//�A�^�b�N
	void Attack();
	//�U���ł��邩
	bool IsAttack();

	//�W�����v���Ă��邩�ǂ���
	bool IsJump();

	//���f�����擾
	const std::shared_ptr<Model> GetModel() const;

	/// <summary>
	/// �A�j���[�V������؂�ւ���
	/// </summary>
	/// <param name="animNo">�ύX��A�j���[�V����</param>
	/// <param name="isLoop">�A�j���[�V���������[�v�����邩</param>
	/// <param name="isForceChange">���łɎw�肳�ꂽ�A�j�����Đ�����Ă���ꍇ���ύX���邩</param>
	/// <param name="changeFrame">�t���[����</param>
	void SwitchAnimation(PlayerAnimation animNo, bool isLoop, bool isForceChange, int changeFrame);

	/// <summary>
	/// �J�����͈͓̔��ɂ��邩�ǂ���
	/// </summary>
	/// <returns>true:�͈͓� false:�͈͊O</returns>
	bool OnCamera();
private:
	void (Player::* m_update)(const InputState& input);//Update�p�����o�֐��|�C���^

	//�ʏ�A�b�v�f�[�g
	void NormalUpdate(const InputState& input);
	//�W�����v����
	void JumpUpdate(const InputState& input);

	//�v���C���[�ړ�
	void Move();

	VECTOR m_pos;//�\���ʒu
	VECTOR m_vel;//�ړ���
	VECTOR m_dir;//�ړ������A��]����
	VECTOR m_rot;//�\������

	std::shared_ptr<Model> m_model;//���f��
	PlayerAnimation m_animType;//���ݍĐ����Ă���A�j���[�V����

	std::shared_ptr<Map> m_map;//�}�b�v

	float m_jumpPower;//�W�����v��
	bool m_isAttack;//�U���ł���

	bool m_isDamage;//�U�����󂯂�
	int m_flashing;//�_���[�W���󂯂��Ƃ��_�ł�����

	std::shared_ptr<Hp> m_hp;//HP
};


