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
	Dance = 0,		//�_���X
	Death = 1,		//�����
	Idle = 2,		//�����~�܂�
	Jump = 3,		//���̏�W�����v
	No = 4,			//��U��
	Punch = 5,		//�p���`
	Run = 6,		//����
	ThumbsUp = 9,	//
	Walk = 10,		//����
	WalkJump = 11,	//�����Ȃ���W�����v
	Wave = 12,		//
	Yes = 13,		//���Ȃ���
};

class Player
{
public:
	Player(std::shared_ptr<Map> map);
	virtual ~Player();

	void Update(const InputState& input);
	void Update();

	void Draw();

	/// <summary>
	/// �\���ʒu���擾
	/// </summary>
	/// <returns>�\���ʒu</returns>
	const VECTOR GetPos() const;

	/// <summary>
	/// ���f���Ɠ������Ă��邩�ǂ���
	/// </summary>
	/// <param name="model">���f��</param>
	bool HitModel(int model);
	
	/// <summary>
	/// �_���[�W���󂯂�
	/// </summary>
	/// <param name="attack">�_���[�W��</param>
	void OnDamage(int attack);
	//�_���[�W���󂯂Ă��邩
	
	/// <summary>
	/// ���łɃ_���[�W���󂯂Ă��邩
	/// </summary>
	/// <returns>true:�_���[�W���󂯂Ă��� false:�_���[�W���󂯂Ă��Ȃ�</returns>
	bool IsDamage() { return m_isDamage; }

	/// <summary>
	/// ���݂�HP���擾
	/// </summary>
	/// <returns>���݂�HP</returns>
	const int GetHp()const;

	/// <summary>
	/// �U������
	/// </summary>
	void Attack();

	/// <summary>
	/// �U�����ł��邩�ǂ������擾����
	/// </summary>
	/// <returns>true:�U���ł��� false:�U���ł��Ȃ�</returns>
	bool IsAttack();

	/// <summary>
	/// �W�����v���Ă��邩�ǂ������擾����
	/// </summary>
	/// <returns>true:�W�����v���Ă��� false:�W�����v���Ă��Ȃ�</returns>
	bool IsJump();

	/// <summary>
	/// ���f�����擾����
	/// </summary>
	/// <returns>���f��</returns>
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

	/// <summary>
	/// �ʏ�A�b�v�f�[�g
	/// </summary>
	/// <param name="input">�L�[����</param>
	void NormalUpdate(const InputState& input);
	
	/// <summary>
	/// �W�����v�A�b�v�f�[�g
	/// </summary>
	/// <param name="input">�L�[����</param>
	void JumpUpdate(const InputState& input);

	/// <summary>
	/// �ړ�����
	/// </summary>
	void Move();

	VECTOR m_pos;//�\���ʒu
	VECTOR m_vel;//�ړ���
	VECTOR m_dir;//�ړ�����
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


