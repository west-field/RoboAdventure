#include "TestPlayer.h"
#include "../Model.h"
#include "../InputState.h"
#include <cassert>	
#include "Hp.h"
#include "../Util/Sound.h"

//TODO:��邱��
//FIXME:�����Ȃ��Ⴂ���Ȃ�����
//HACK:����

namespace
{
	// ���x�i1=1m�A60fps�Œ�Ƃ��āA����10km�j10000m �� ���� �� �� �� �b �� �t���[��
	constexpr float kSpeed = static_cast<float>(10000.0 / 60.0 / 60.0 / 60.0);
	
	constexpr float kScale = 0.002f;// �X�P�[��
	
	constexpr float kJumpPower = 0.25f;//�W�����v��
	
	constexpr float kGravity = -0.1f;//�d��

	constexpr int kMaxHitColl = 2048;		// ��������R���W�����|���S���̍ő吔
	constexpr float kEnumDefaultSize = 800.0f;		// ���͂̃|���S�����o�Ɏg�p���鋅�̏����T�C�Y

	constexpr float kHitWidth = 0.5f;		// �����蔻��J�v�Z���̔��a
	constexpr float kHitHeight = 0.9f;		// �����蔻��J�v�Z���̍���


	constexpr int kSlideTryNum = 16;		// �ǉ����o�������̍ő厎�s��


	constexpr float kHitSlideLength = kSpeed / 2;		// ��x�̕ǉ����o�������ŃX���C�h�����鋗��

}

//�R���X�g���N�^
TestPlayer::TestPlayer():m_animUpdate(&TestPlayer::UpdateIdle),m_colision(true)
{
	m_model = std::make_shared<Model>(L"Data/Model/Robot.mv1",kScale,true);
	
	m_animType = PlayerAnimation::Idle;
	m_model->SetAnimation(static_cast<int>(m_animType), true, true);

#if _DEBUG
	m_colision.center = VGet((0.65f * 2) * 84, 1.0f, 0);
#else
	m_colision.center = VGet(10.0f, 1.0f, 0);
#endif
	m_colision.size = VGet(0.5f, 0.9f, 0.0f);
	m_colision.isCenter = false;

	m_colision.SetLTRB(VGet(-0.3f, 0.9f, 0.0f), VGet(-0.3f, 0.0f, 0.0f),
		VGet(0.3f, 0.9f, 0.0f), VGet(0.3f, 0.0f, 0.0f));

	m_colision.velocity = VGet(0, 0, 0);

	m_hp = std::make_shared<Hp>();
	m_hp->MaxHp(3);

	m_dir = VGet(0, 0, 1);
	m_rot = VGet(0.0f, -95.0f, 0.0f);

	m_model->SetPos(m_colision.GetCenter());
	m_model->SetRot(m_rot);


	m_jumpAdd = kGravity * 2;
	m_isJump = false;

	m_isLeft = false;

	m_flashing = 0;
}
//�f�X�g���N�^
TestPlayer::~TestPlayer()
{
	m_model.reset();
}

//�X�V
void TestPlayer::Update(const InputState& input)
{
	if (m_isDamage && m_flashing == 0)
	{
		m_isDamage = false;
	}
	else if (m_flashing > 0)
	{
		m_flashing--;
	}
	//�A�j���[�V������i�߂�
	m_model->Update();
	
	//�A�j���[�V�����̍Đ�
	(this->*m_animUpdate)(input);
}

void TestPlayer::Update()
{
	if (m_flashing != 0)
	{
		m_flashing = 0;
	}

	m_model->Update();
}

//�`��
void TestPlayer::Draw()
{
	m_hp->Draw();
	if (m_flashing % 10 != 0)	return;
	m_model->Draw();
#ifdef _DEBUG
	m_colision.Draw();
	DrawFormatString(0, 0, 0xffffff, L"%lf,%lf,%lf", m_colision.GetCenter().x, m_colision.GetCenter().y, m_colision.GetCenter().z);
	DrawFormatString(0, 20, 0xffffff, L"%lf,%lf,%lf", m_colision.GetSize().x, m_colision.GetSize().y, m_colision.GetSize().z);
	DrawFormatString(80, 40, 0xffffff, L"%lf,%lf,%lf", m_colision.GetVel().x, m_colision.GetVel().y, m_colision.GetVel().z);
#endif
}

//���݂̈ʒu���擾����
const VECTOR TestPlayer::GetPos() const
{
	return m_colision.GetCenter();
}
void TestPlayer::SetPos(VECTOR pos)
{
	m_colision.center = pos;
}
//���݂̃��f���̈ʒu���擾����
const VECTOR TestPlayer::GetModelPos() const
{
	return m_model->GetPos();
}
//�T�C�Y���擾����
const VECTOR TestPlayer::GetSize() const
{
	return m_colision.GetSize();
}
//�ړ���
const VECTOR& TestPlayer::GetVel() const
{
	return m_colision.GetVel();
}
//�ړ��͂�ύX
void TestPlayer::SetVel(VECTOR vel)
{
	m_colision.velocity = vel;
	m_colision.center = m_prevPos;
	//���̈ʒu���擾���Ă���
	m_prevPos = m_colision.center;
	// �|�W�V�������X�V.
	m_colision.center = VAdd(m_colision.center, m_colision.velocity);
}

const VECTOR& TestPlayer::GetDir() const
{
	return m_dir;
}

//�ړ�����
void TestPlayer::Move()
{
	// �|�W�V�������X�V.
	//m_coll.center = VAdd(m_coll.GetCenter(), m_coll.velocity);
	// �͂������I������x���V�e�B�̕����Ƀf�B���N�V�����𒲐�.
	if (VSize(m_colision.velocity) != 0)
	{
		m_dir = VNorm(m_colision.velocity);
	}
	//printfDx("%f %f %f\n", dir.x, dir.y, dir.z);
	m_model->SetPos(m_colision.GetCenter());//�ړ�������
	m_model->SetRot(m_rot);
}
//�ړ����Ȃ��Ƃ����̈ʒu�ɖ߂�
void TestPlayer::NoMove(int hit)
{
	switch (hit)
	{
	case 0://�㉺�ɓ������Ă���
		m_colision.center.y = m_prevPos.y;//�ړ��������Ɍ��̈ʒu�ɖ߂�
		break;
	case 1://���E�ɓ������Ă���
		m_colision.center.x = m_prevPos.x;//�ړ��������Ɍ��̈ʒu�ɖ߂�
		break;
	case 2: m_colision.center = m_prevPos;//�ړ��������Ɍ��̈ʒu�ɖ߂�
		break;
	default:
		break;
	}
	//�W�����v���Ă��āA�W�����v�͂�2.5f�ȉ��̎�
	if (m_isJump && m_jumpAdd <= 0.0f)
	{
		//������
		m_isJump = false;
		m_animUpdate = &TestPlayer::UpdateIdle;
	}
}

//��������
void TestPlayer::Drop()
{
	if (m_isJump)	return;

	//���̈ʒu���擾���Ă���
	m_prevPos = m_colision.center;
	// �|�W�V�������X�V.
	m_colision.center = VAdd(m_colision.center, VGet(0.0f, kGravity * 2, 0.0f));
	//m_model->SetPos(m_coll.GetCenter());
}

const int TestPlayer::GetHp()const
{
	return m_hp->GetHp();
}

//�J�����͈͓̔��ɂ��邩�ǂ���
bool TestPlayer::OnCamera()
{
	//��̍��W�ł���킳���{�b�N�X���J�����̎��E�ɓ����Ă��Ȃ����ǂ����𔻒肷��
	if (CheckCameraViewClip_Box(VAdd(m_colision.center, m_colision.size),
		VSub(m_colision.center, m_colision.size)))//true:���E�ɓ����Ă��Ȃ� false:���E�ɓ����Ă���
	{
		return false;
	}
	return true;
}

//�u���b�N�ɓ���������
void TestPlayer::HitBlockUp()
{
	//�W�����v���Ă���
	if (m_isJump)
	{
		//������
		m_jumpAdd = kGravity;
		m_isJump = false;
		m_animUpdate = &TestPlayer::UpdateIdle;
	}
}

//�u���b�N�ɓ���������
void TestPlayer::HitBlockWidth()
{
	//���Ɉړ����Ȃ��悤��
	m_dir = VAdd(m_dir, VGet(0, 0, 0));
	// �[�����Z����
	if (VSquareSize(m_dir) > 0)
	{
		// ���K��
		m_dir = VNorm(m_dir);
	}
	m_colision.velocity = VScale(m_dir, kSpeed);
}

void TestPlayer::OnDamage()
{
	if (m_isDamage)	return;
	m_isDamage = true;
	//�v���C���[��HP������
	m_hp->Damage(1);
	//�v���C���[���_�ł���
	m_flashing = 10 * 4 + 1;
}
bool TestPlayer::IsDamage()
{
	return m_isDamage;
}

const float TestPlayer::GetSpeed()const
{
	return kSpeed;
}

void TestPlayer::Attack()
{
	//�W�����v�A�j���[�V�����ɕύX
	SwitchAnimation(PlayerAnimation::WalkJump, true, false, 4);
	//�W�����v�̍X�V�ɕύX
	m_animUpdate = &TestPlayer::UpdateJump;
	m_jumpAdd = kJumpPower / 2;
	m_isJump = true;
}

void TestPlayer::SwitchAnimation(PlayerAnimation animNo, bool isLoop, bool isForceChange, int changeFrame)
{
	int number = static_cast<int>(animNo);
	m_model->ChangeAnimstion(number, isLoop, isForceChange, changeFrame);
}

//------------------------------private----------------------------------

//���Ɉړ�
void TestPlayer::MoveLeft()
{
	m_dir = VAdd(m_dir, VGet(-1, 0, 0));
	// �[�����Z����
	if (VSquareSize(m_dir) > 0)
	{
		// ���K��
		m_dir = VNorm(m_dir);
	}
	m_rot = VGet(0.0f, 95.0f, 0.0f);
	m_isLeft = true;
	m_colision.velocity = VScale(m_dir, kSpeed);
}
//�E�Ɉړ�
void TestPlayer::MoveRight()
{
	m_dir = VAdd(m_dir, VGet(1, 0, 0));
	// �[�����Z����
	if (VSquareSize(m_dir) > 0)
	{
		// ���K��
		m_dir = VNorm(m_dir);
	}
	m_rot = VGet(0.0f, -95.0f, 0.0f);
	m_isLeft = false;
	m_colision.velocity = VScale(m_dir, kSpeed);
}

//�ҋ@�A�j���[�V�����A�b�v�f�[�g
void TestPlayer::UpdateIdle(const InputState& input)
{
	bool isPrees = false;//�L�[�������Ă��邩
	// �P���ɕ����]��
	m_dir = VGet(0, 0, 0);
	m_colision.velocity = VScale(m_dir, kSpeed);

	if (input.IsPressed(InputType::up))
	{
		m_dir = VAdd(m_dir, VGet(0, 1, 0));
		isPrees = true;
	}
	else if (input.IsPressed(InputType::down))
	{
		m_dir = VAdd(m_dir, VGet(0, -1, 0));
		isPrees = true;
	}
	if (input.IsPressed(InputType::right))
	{
		MoveRight();
		isPrees = true;
	}
	else if (input.IsPressed(InputType::left))
	{
		MoveLeft();
		isPrees = true;
	}

	//�L�[�������Ă��鎞
	if (isPrees)
	{
		//�����A�j���[�V�����ɕύX
		SwitchAnimation(PlayerAnimation::Walk, true, false, 4);
	}
	else
	{
		//�����Ă��Ȃ��Ƃ��ҋ@�A�j���[�V�����ɕύX����
		SwitchAnimation(PlayerAnimation::Idle, true, false, 4);
	}

	//�W�����v�{�^������������
	if (!m_isJump && input.IsTriggered(InputType::junp))
	{
		//�W�����v�����Đ�
		SoundManager::GetInstance().Play(SoundId::PlayerJump);
		//�W�����v�A�j���[�V�����ɕύX
		SwitchAnimation(PlayerAnimation::WalkJump, true, false, 4);
		//�W�����v�̍X�V�ɕύX
		m_animUpdate = &TestPlayer::UpdateJump;
		m_jumpAdd = kJumpPower;
		m_isJump = true;
	}

	//���̈ʒu���擾���Ă���
	m_prevPos = m_colision.center;
	m_colision.center = VAdd(m_colision.GetCenter(), m_colision.velocity);
}
//�W�����v�A�b�v�f�[�g
void TestPlayer::UpdateJump(const InputState& input)
{
	m_jumpAdd -= 0.01f;//�W�����v

	//�ǂꂩ��0�łȂ��Ƃ�
	if (m_dir.x != 0.0f || m_dir.y != 0.0f || m_dir.z != 0.0f)
	{
		//���E�Ɉړ�
		if (input.IsPressed(InputType::right))
		{
			MoveRight();
		}
		else if (input.IsPressed(InputType::left))
		{
			MoveLeft();
		}
	}
	
	//���̈ʒu���擾���Ă���
	m_prevPos = m_colision.center;
	//�ʒu��ύX
	m_colision.velocity = VGet(kSpeed * m_dir.x, m_jumpAdd, 0.0f);
	m_colision.center = VAdd(m_colision.GetCenter(), m_colision.velocity);
	//m_model->SetPos(m_coll.GetCenter());
}

