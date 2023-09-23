#include "Player.h"
#include "../Model.h"
#include "../InputState.h"
#include <cassert>
#include "../Game/Map.h"
#include "../Game/Hp.h"
#include "../Util/Sound.h"

namespace
{
	constexpr float kMoveSpeed = static_cast<float>(10000.0 / 60.0 / 60.0 / 60.0);//�ړ��X�s�[�h

	constexpr int kSlideTryNum = 16;//�ʒu�̕␳�������

	constexpr float kJumpSpeed = 0.5f;//�W�����v��

	constexpr float kGravity = 0.1f;//�d��

	constexpr float kSlideMoveSpeed = 0.1f;//�ʒu��␳����ۂɈ��ňړ��ł��鋗��

	constexpr int kMaxHitColl = 1000;		// ��������R���W�����|���S���̍ő吔

	constexpr float kCapsuleRadius = 0.3f;//�J�v�Z���̔��a
	constexpr float kHitCapsuleTop = 0.6f;//�J�v�Z���̏�̈ʒu
	constexpr float kHitCapsuleBottom = 0.3f;//�J�v�Z���̉��̈ʒu

	constexpr float kHitLineTop = 0.9f;//���̏�̈ʒu
	constexpr float kHitLineBottom = -0.1f;//���̉��̈ʒu

	constexpr float kScale = 0.002f;// �X�P�[��

	constexpr float kSizeX = 0.3f;
	constexpr float kSizeY = 0.9f;

	constexpr float kSize = 0.08f;
}

Player::Player(std::shared_ptr<Map> map) : m_update(&Player::NormalUpdate)
{

	m_model = std::make_shared<Model>(L"Data/Model/Robot.mv1", kScale, true);
#if false
	m_pos = VGet(10.0f, 1.0f, 0);
#else
	m_pos = VGet(10.0f, 5.0f, 0);
#endif
	m_model->SetPos(m_pos);

	m_animType = PlayerAnimation::Idle;
	m_model->SetAnimation(static_cast<int>(m_animType), true, true);

	m_map = map;

	m_jumpPower = 0.0f;

	m_dir = VGet(0, 0, 1);
	m_rot = VGet(0.0f, -95.0f, 0.0f);
	m_model->SetRot(m_rot);

	m_isDamage = false;
	m_flashing = 0;

	m_hp = std::make_shared<Hp>();
	m_hp->MaxHp(3);

	float left = VAdd(m_pos, VGet(kSizeX, 0.0f, 0.0f)).x;
	float right = VAdd(m_pos, VGet(-kSizeX, 0.0f, 0.0f)).x;
	float top = VAdd(m_pos, VGet(0.0f, kSizeY, 0.0f)).y;
	float bottom = m_pos.y;

	//m_line[static_cast<int>(Direction::Top)].lineFirst = VGet(left, top, 0.0f);
	//m_line[static_cast<int>(Direction::Top)].lineSecond = VGet(right, top, 0.0f);
	//m_line[static_cast<int>(Direction::Top)].isWidth = true;
	//
	//m_line[static_cast<int>(Direction::Bottom)].lineFirst = VGet(left , bottom, 0.0f);
	//m_line[static_cast<int>(Direction::Bottom)].lineSecond = VGet(right , bottom, 0.0f);
	//m_line[static_cast<int>(Direction::Bottom)].isWidth = true;
	//
	//m_line[static_cast<int>(Direction::Left)].lineFirst = VGet(left, top , 0.0f);
	//m_line[static_cast<int>(Direction::Left)].lineSecond = VGet(left, bottom , 0.0f);
	//m_line[static_cast<int>(Direction::Left)].isWidth = false;

	//m_line[static_cast<int>(Direction::Right)].lineFirst = VGet(right, top , 0.0f);
	//m_line[static_cast<int>(Direction::Right)].lineSecond = VGet(right, bottom , 0.0f);
	//m_line[static_cast<int>(Direction::Right)].isWidth = false;

	m_isAttack = false;
}

Player::~Player()
{
	//m_model->DeleteModel();
}

void Player::Update(const InputState& input)
{
	if (m_isDamage && m_flashing == 0)
	{
		m_isDamage = false;
	}
	else if(m_flashing > 0)
	{
		m_flashing--;
	}

	(this->*m_update)(input);

	Move();
	
	// �͂������I������x���V�e�B�̕����Ƀf�B���N�V�����𒲐�.
	if (VSize(m_vel) != 0)
	{
		m_dir = VNorm(m_vel);
	}
	m_model->SetPos(m_pos);
	m_model->SetRot(m_rot);
	m_model->Update();
}

void Player::Update()
{
	if (m_flashing != 0)
	{
		m_flashing = 0;
	}

	m_model->Update();
}

void Player::Draw()
{
	m_hp->Draw();
	//�U�����󂯂��Ƃ��͓_�ł�����
	if (m_flashing % 10 != 0)	return;
	m_model->Draw();
#if true
	DrawFormatString(0, 20, 0xffffff, L"pos.x%lf,y%lf,z%lf", m_pos.x, m_pos.y, m_pos.z);
	DrawFormatString(0, 40, 0xffffff, L"jump%lf", m_jumpPower);

	DrawSphere3D(VAdd(m_pos, VGet(0.0f, kSizeY / 3, 0.0f)), kCapsuleRadius + VSize(m_dir), 10, 0xffffff, 0xffffff, false);
	DrawCapsule3D(VAdd(m_pos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(m_pos, VGet(0.0f, kHitCapsuleTop, 0.0f)),
		kCapsuleRadius, 10, 0xffffff, 0xffffff, false);

	DrawLine3D(VAdd(m_pos, VGet(0.0f, kHitLineTop, 0.0f)), VAdd(m_pos, VGet(0.0f, kHitLineBottom - 0.2f, 0.0f)), 0xff0000);//��

	switch (m_animType)
	{
	case PlayerAnimation::Dance:
		DrawFormatString(0, 60, 0xffffff, L"Dance");
		break;
	case PlayerAnimation::Death:
		DrawFormatString(0, 60, 0xffffff, L"Death");
		break;
	case PlayerAnimation::Idle:
		DrawFormatString(0, 60, 0xffffff, L"Idle");
		break;
	case PlayerAnimation::Jump:
		DrawFormatString(0, 60, 0xffffff, L"Jump");
		break;
	case PlayerAnimation::No:
		DrawFormatString(0, 60, 0xffffff, L"No");
		break;
	case PlayerAnimation::Punch:
		DrawFormatString(0, 60, 0xffffff, L"Punch");
		break;
	case PlayerAnimation::Run:
		DrawFormatString(0, 60, 0xffffff, L"Run");
		break;
	case PlayerAnimation::ThumbsUp:
		DrawFormatString(0, 60, 0xffffff, L"ThumbsUp");
		break;
	case PlayerAnimation::Walk:
		DrawFormatString(0, 60, 0xffffff, L"Walk");
		break;
	case PlayerAnimation::WalkJump:
		DrawFormatString(0, 60, 0xffffff, L"WalkJump");
		break;
	case PlayerAnimation::Wave:
		DrawFormatString(0, 60, 0xffffff, L"Wave");
		break;
	case PlayerAnimation::Yes:
		DrawFormatString(0, 60, 0xffffff, L"Yes");
		break;
	default:
		break;
	}

#endif
}

const VECTOR Player::GetPos() const
{
	return m_model->GetPos();
}

bool Player::HitModel(int model)
{
	MV1_COLL_RESULT_POLY_DIM dim = MV1CollCheck_Capsule(model, -1, 
		VAdd(m_pos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(m_pos, VGet(0.0f, kHitCapsuleTop, 0.0f)), kCapsuleRadius);

	if (dim.HitNum != 0)
	{
		MV1CollResultPolyDimTerminate(dim);
		return true;
	}

	MV1CollResultPolyDimTerminate(dim);
	return false;
}

void Player::OnDamage(int attack)
{
	//���łɍU�����󂯂Ă���ꍇ�͏��������Ȃ�
	if (m_isDamage)	return;
	m_isDamage = true;
	//�v���C���[���G�ɓ���������
	SoundManager::GetInstance().Play(SoundId::PlayeyHit);
	//�v���C���[��HP������
	m_hp->Damage(attack);
	//�v���C���[���_�ł���
	m_flashing = 10 * 4 + 1;
}

const int Player::GetHp() const
{
	return m_hp->GetHp();
}

void Player::Attack()
{
	//��ɏオ��
	m_jumpPower += kJumpSpeed / 2;
	m_update = &Player::JumpUpdate;
}

bool Player::IsAttack()
{
	return m_isAttack;
}

bool Player::IsJump()
{
	if (m_jumpPower > 0.0f)
	{
		return true;
	}
	return false;
}

const std::shared_ptr<Model> Player::GetModel() const
{
	return m_model;
}

void Player::SwitchAnimation(PlayerAnimation animNo, bool isLoop, bool isForceChange, int changeFrame)
{
	int number = static_cast<int>(animNo);
	m_model->ChangeAnimstion(number, isLoop, isForceChange, changeFrame);
}

bool Player::OnCamera()
{
	////��̍��W�ł���킳���{�b�N�X���J�����̎��E�ɓ����Ă��Ȃ����ǂ����𔻒肷��
	//if (CheckCameraViewClip_Box(m_line[static_cast<int>(Direction::Top)].lineFirst,
	//	m_line[static_cast<int>(Direction::Bottom)].lineSecond))//true:���E�ɓ����Ă��Ȃ� false:���E�ɓ����Ă���
	//{
	//	return false;
	//}
	return true;
}

void Player::NormalUpdate(const InputState& input)
{
	//m_dir = VGet(0.0f, 0.0f, 0.0f);//������
	//m_vel = VGet(0.0f, 0.0f, 0.0f);//������
	bool isPrees = false;
#if true
	if (input.IsPressed(InputType::up))
	{
		m_dir = VGet(0.0f, 1.0f, 0.0f);
		isPrees = true;
		m_jumpPower = 1.0f;
		m_animType = PlayerAnimation::WalkJump;
	}
	if (input.IsPressed(InputType::down))
	{
		m_dir = VGet(0.0f, -1.0f, 0.0f);
		isPrees = true;
	}
#endif
	if (input.IsPressed(InputType::left))
	{
		m_dir = VGet(-1.0f, 0.0f, 0.0f);
		m_rot = VGet(0.0f, 95.0f, 0.0f);
		isPrees = true;
	}
	else if (input.IsPressed(InputType::right))
	{
		m_dir = VGet(1.0f, 0.0f, 0.0f);
		m_rot = VGet(0.0f, -95.0f, 0.0f);
		isPrees = true;
	}

	//�L�[�������Ă��鎞
	if (isPrees)
	{
		m_animType = PlayerAnimation::Walk;
		//�����A�j���[�V�����ɕύX
		SwitchAnimation(PlayerAnimation::Walk, true, false, 4);
	}
	else
	{
		m_animType = PlayerAnimation::Idle;
		//�����Ă��Ȃ��Ƃ��ҋ@�A�j���[�V�����ɕύX����
		SwitchAnimation(PlayerAnimation::Idle, true, false, 4);
	}

	//�W�����v��Ԃł͂Ȃ��A�W�����v�{�^���������ꂽ��
	if (m_animType != PlayerAnimation::WalkJump && input.IsTriggered(InputType::junp))
	{
		//�W�����v��
		SoundManager::GetInstance().Play(SoundId::PlayerJump);
		////�W�����v��Ԃɂ���
		m_animType = PlayerAnimation::WalkJump;
		//Y�������̑��x���Z�b�g
		m_jumpPower = kJumpSpeed;
		//�A�j���[�V�����̕ύX
		SwitchAnimation(m_animType, true, false, 4);

		m_vel.y = m_jumpPower;

		m_update = &Player::JumpUpdate;
	}

	// �[�����Z����
	if (VSquareSize(m_dir) > 0)
	{
		// ���K��
		m_dir = VNorm(m_dir);
	}
	m_vel = VScale(m_dir, kMoveSpeed);

	m_dir.y = -1.0f;
	m_vel.y += m_dir.y * kGravity;
}

void Player::JumpUpdate(const InputState& input)
{
	//��Ԃ��W�����v�̏ꍇ��
	if (m_animType == PlayerAnimation::WalkJump && m_jumpPower > 0.0f)
	{
		//�d�͕�����
		m_jumpPower -= kGravity ;

		//�ړ��x�N�g����Y�����ɑ������
		m_vel.y += m_jumpPower;
	}
	else 
	{
		m_jumpPower = 0.0f;
		m_isAttack = true;
		m_update = &Player::NormalUpdate;
	}
}

void Player::Move()
{
#if true
	bool isMoveFlag;						// ���������Ɉړ��������ǂ����̃t���O( �O:�ړ����Ă��Ȃ�  �P:�ړ����� )
	bool isHitFlag;						// �|���S���ɓ����������ǂ������L�����Ă����̂Ɏg���ϐ�( �O:�������Ă��Ȃ�  �P:�������� )
	MV1_COLL_RESULT_POLY_DIM HitDim;			// �v���C���[�̎��͂ɂ���|���S�������o�������ʂ��������铖���蔻�茋�ʍ\����
	int wallCollisionNum;						// �ǃ|���S���Ɣ��f���ꂽ�|���S���̐�
	int floorCollisionNum;						// ���|���S���Ɣ��f���ꂽ�|���S���̐�
	MV1_COLL_RESULT_POLY* wallCol[kMaxHitColl];	// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
	MV1_COLL_RESULT_POLY* floorCol[kMaxHitColl];	// ���|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
	HITRESULT_LINE LineRes;				// �����ƃ|���S���Ƃ̓����蔻��̌��ʂ�������\����
	VECTOR oldPos;						// �ړ��O�̍��W	
	VECTOR nowPos;						// �ړ���̍��W

	// �ړ��O�̍��W��ۑ�
	oldPos = m_pos;

	// �ړ���̍��W���Z�o
	nowPos = VAdd(m_pos, m_vel);
	//m_map���瓖���蔻��p��VECTOR���擾����
	for (auto& model : m_map->GetPiece())
	{
		if (!model.model->IsExist())	continue;
		// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����
		// ( ���o����͈͈͂ړ��������l������ )
		HitDim = MV1CollCheck_Sphere(model.model->GetModelHandle(), -1, VAdd(m_pos, VGet(0.0f, kSizeY / 3, 0.0f)), kCapsuleRadius + VSize(m_dir));

		if (HitDim.HitNum == 0)	continue;

		// x����z�������� 0.01f �ȏ�ړ������ꍇ�́u�ړ������v�t���O���P�ɂ���
		if (fabs(m_vel.x) > 0.01f || fabs(m_vel.z) > 0.01f)
		{
			isMoveFlag = true;
		}
		else
		{
			isMoveFlag = false;
		}

		// ���o���ꂽ�|���S�����ǃ|���S��( �w�y���ʂɐ����ȃ|���S�� )�����|���S��( �w�y���ʂɐ����ł͂Ȃ��|���S�� )���𔻒f����
		{
			// �ǃ|���S���Ə��|���S���̐�������������
			wallCollisionNum = 0;
			floorCollisionNum = 0;

			// ���o���ꂽ�|���S���̐������J��Ԃ�
			for (int i = 0; i < HitDim.HitNum; i++)
			{
				// �w�y���ʂɐ������ǂ����̓|���S���̖@���̂x�������O�Ɍ���Ȃ��߂����ǂ����Ŕ��f����
				if (HitDim.Dim[i].Normal.y < 0.000001f && HitDim.Dim[i].Normal.y > -0.000001f)
				{
					// �ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{�P�D�O����荂���|���S���̂ݓ����蔻����s��
					if (HitDim.Dim[i].Position[0].y > m_pos.y + 1.0f ||
						HitDim.Dim[i].Position[1].y > m_pos.y + 1.0f ||
						HitDim.Dim[i].Position[2].y > m_pos.y + 1.0f)
					{
						// �|���S���̐����񋓂ł�����E���ɒB���Ă��Ȃ�������|���S����z��ɒǉ�
						if (wallCollisionNum < kMaxHitColl)
						{
							// �|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
							wallCol[wallCollisionNum] = &HitDim.Dim[i];

							// �ǃ|���S���̐������Z����
							wallCollisionNum++;
						}
					}
				}
				else
				{
					// �|���S���̐����񋓂ł�����E���ɒB���Ă��Ȃ�������|���S����z��ɒǉ�
					if (floorCollisionNum < kMaxHitColl)
					{
						// �|���S���̍\���̂̃A�h���X�����|���S���|�C���^�z��ɕۑ�����
						floorCol[floorCollisionNum] = &HitDim.Dim[i];

						// ���|���S���̐������Z����
						floorCollisionNum++;
					}
				}
			}
		}

		// �ǃ|���S���Ƃ̓����蔻�菈��
		if (wallCollisionNum != 0)
		{
			// �ǂɓ����������ǂ����̃t���O�͏�����Ԃł́u�������Ă��Ȃ��v�ɂ��Ă���
			isHitFlag = false;

			// �ړ��������ǂ����ŏ����𕪊�
			if (isMoveFlag)
			{
				// �ǃ|���S���̐������J��Ԃ�
				for (int i = 0; i < wallCollisionNum; i++)
				{
					// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
					if (HitCheck_Capsule_Triangle(VAdd(nowPos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitCapsuleTop, 0.0f)),
													kCapsuleRadius, wallCol[i]->Position[0], wallCol[i]->Position[1], wallCol[i]->Position[2]) == FALSE) continue;

					// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
					isHitFlag = true;

					// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
					{
						VECTOR SlideVec;	// �v���C���[���X���C�h������x�N�g��

						// �i�s�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o
						SlideVec = VCross(m_vel, wallCol[i]->Normal);

						// �Z�o�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o�A���ꂪ
						// ���̈ړ���������Ǖ����̈ړ������𔲂����x�N�g��
						SlideVec = VCross(wallCol[i]->Normal, SlideVec);

						// ������ړ��O�̍��W�ɑ��������̂�V���ȍ��W�Ƃ���
						nowPos = VAdd(oldPos, SlideVec);
						nowPos.z = 0.0f;
					}
					int j = 0;
					// �V���Ȉړ����W�ŕǃ|���S���Ɠ������Ă��Ȃ����ǂ����𔻒肷��
					for (j = 0; j < wallCollisionNum; j++)
					{
						// �������Ă����烋�[�v���甲����
						if (HitCheck_Capsule_Triangle(VAdd(nowPos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitCapsuleTop, 0.0f)),
							kCapsuleRadius, wallCol[j]->Position[0], wallCol[j]->Position[1], wallCol[j]->Position[2]) == TRUE) break;
					}

					// j �� KabeNum �������ꍇ�͂ǂ̃|���S���Ƃ�������Ȃ������Ƃ������ƂȂ̂�
					// �ǂɓ��������t���O��|������Ń��[�v���甲����
					if (j == wallCollisionNum)
					{
						isHitFlag = false;
						break;
					}
				}
			}
			else
			{
				// �ړ����Ă��Ȃ��ꍇ�̏���

				// �ǃ|���S���̐������J��Ԃ�
				for (int i = 0; i < wallCollisionNum; i++)
				{
					// �|���S���ɓ������Ă����瓖�������t���O�𗧂Ă���Ń��[�v���甲����
					if (HitCheck_Capsule_Triangle(VAdd(nowPos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitCapsuleTop, 0.0f)), kCapsuleRadius, wallCol[i]->Position[0], wallCol[i]->Position[1], wallCol[i]->Position[2]) == TRUE)
					{
						isHitFlag = true;
						break;
					}
				}
			}

			// �ǂɓ������Ă�����ǂ��牟���o���������s��
			if (isHitFlag)
			{
				// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
				for (int k = 0; k < kSlideTryNum; k++)
				{
					int i = 0;
					// �ǃ|���S���̐������J��Ԃ�
					for (i = 0; i < wallCollisionNum; i++)
					{
						// �v���C���[�Ɠ������Ă��邩�𔻒�
						if (HitCheck_Capsule_Triangle(VAdd(nowPos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitCapsuleTop, 0.0f)), kCapsuleRadius, wallCol[i]->Position[0], wallCol[i]->Position[1], wallCol[i]->Position[2]) == FALSE) continue;

						// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
						nowPos = VAdd(nowPos, VScale(wallCol[i]->Normal, kSlideMoveSpeed));
						nowPos.z = 0.0f;

						int j = 0;
						// �ړ�������ŕǃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
						for (j = 0; j < wallCollisionNum; j++)
						{
							if (HitCheck_Capsule_Triangle(VAdd(nowPos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitCapsuleTop, 0.0f)), kCapsuleRadius, wallCol[j]->Position[0], wallCol[j]->Position[1], wallCol[j]->Position[2]) == TRUE) break;
						}

						// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
						if (j == wallCollisionNum) break;
					}

					// i �� KabeNum �ł͂Ȃ��ꍇ�͑S���̃|���S���ŉ����o�������݂�O�ɑS�Ă̕ǃ|���S���ƐڐG���Ȃ��Ȃ����Ƃ������ƂȂ̂Ń��[�v���甲����
					if (i != wallCollisionNum) break;
				}
			}
		}

		// ���|���S���Ƃ̓����蔻��
		if (floorCollisionNum != 0)
		{
			// �W�����v�����㏸���̏ꍇ�͏����𕪊�
			if (m_animType == PlayerAnimation::WalkJump && m_jumpPower > 0.0f)
			{
				float MinY;

				// �V��ɓ����Ԃ��鏈�����s��

				// ��ԒႢ�V��ɂԂ���ׂ̔���p�ϐ���������
				MinY = 0.0f;

				// �����������ǂ����̃t���O�𓖂����Ă��Ȃ����Ӗ�����O�ɂ��Ă���
				isHitFlag = false;

				// ���|���S���̐������J��Ԃ�
				for (int i = 0; i < floorCollisionNum; i++)
				{
					// ���悩�瓪�̍����܂ł̊ԂŃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
					LineRes = HitCheck_Line_Triangle(VAdd(nowPos, VGet(0.0f, kHitLineTop, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitLineBottom+1.0f, 0.0f)),
						floorCol[i]->Position[0], floorCol[i]->Position[1], floorCol[i]->Position[2]);

					// �ڐG���Ă��Ȃ������牽�����Ȃ�
					if (LineRes.HitFlag == FALSE) continue;

					// ���Ƀ|���S���ɓ������Ă��āA�����܂Ō��o�����V��|���S����荂���ꍇ�͉������Ȃ�
					if (isHitFlag && MinY < LineRes.Position.y) continue;

					// �|���S���ɓ��������t���O�𗧂Ă�
					isHitFlag = true;

					// �ڐG�����x���W��ۑ�����
					MinY = LineRes.Position.y;
				}

				// �ڐG�����|���S�������������ǂ����ŏ����𕪊�
				if (isHitFlag)
				{
					// �ڐG�����ꍇ�̓v���C���[�̂x���W��ڐG���W�����ɍX�V
					nowPos.y = MinY - kHitCapsuleTop;

					// �x�������̑��x�͔��]
					m_jumpPower = -m_jumpPower;
				}
			}
			else
			{
				float MaxY;

				// ���~�����W�����v���ł͂Ȃ��ꍇ�̏���

				// ���|���S���ɓ����������ǂ����̃t���O��|���Ă���
				isHitFlag = false;

				// ��ԍ������|���S���ɂԂ���ׂ̔���p�ϐ���������
				MaxY = 0.0f;

				// ���|���S���̐������J��Ԃ�
				for (int i = 0; i < floorCollisionNum; i++)
				{
					// �W�����v�����ǂ����ŏ����𕪊�
					if (m_animType == PlayerAnimation::WalkJump)
					{
						// �W�����v���̏ꍇ�͓��̐悩�瑫���菭���Ⴂ�ʒu�̊Ԃœ������Ă��邩�𔻒�
						LineRes = HitCheck_Line_Triangle(VAdd(nowPos, VGet(0.0f, kHitLineTop, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitLineBottom, 0.0f)), 
							floorCol[i]->Position[0], floorCol[i]->Position[1], floorCol[i]->Position[2]);
					}
					else
					{
						// �����Ă���ꍇ�͓��̐悩�炻�������Ⴂ�ʒu�̊Ԃœ������Ă��邩�𔻒�( �X�΂ŗ�����ԂɈڍs���Ă��܂�Ȃ��� )
						LineRes = HitCheck_Line_Triangle(VAdd(nowPos, VGet(0.0f, kHitLineTop, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitLineBottom - 0.2f, 0.0f)),
							floorCol[i]->Position[0], floorCol[i]->Position[1], floorCol[i]->Position[2]);
					}

					// �������Ă��Ȃ������牽�����Ȃ�
					if (LineRes.HitFlag == FALSE) continue;

					// ���ɓ��������|���S��������A�����܂Ō��o�������|���S�����Ⴂ�ꍇ�͉������Ȃ�
					if (isHitFlag && MaxY > LineRes.Position.y) continue;

					// �|���S���ɓ��������t���O�𗧂Ă�
					isHitFlag = true;

					// �ڐG�����x���W��ۑ�����
					MaxY = LineRes.Position.y;
				}

				// ���|���S���ɓ����������ǂ����ŏ����𕪊�
				if (isHitFlag)
				{
					// ���������ꍇ
					m_isAttack = false;

					// �ڐG�����|���S���ň�ԍ����x���W���v���C���[�̂x���W�ɂ���
					nowPos.y = MaxY;

					// �x�������̈ړ����x�͂O��
					m_jumpPower = 0.0f;

					// �����W�����v���������ꍇ�͒��n��Ԃɂ���
					if (m_animType == PlayerAnimation::WalkJump)
					{
						// �ړ����Ă������ǂ����Œ��n��̏�ԂƍĐ�����A�j���[�V�����𕪊򂷂�
						if (isMoveFlag)
						{
							// �ړ����Ă���ꍇ�͑����Ԃ�
							m_animType = PlayerAnimation::Run;
							SwitchAnimation(m_animType, true, false, 4);
						}
						else
						{
							// �ړ����Ă��Ȃ��ꍇ�͗����~���Ԃ�
							m_animType = PlayerAnimation::Idle;
							SwitchAnimation(m_animType, true, false, 4);
						}
					}
				}
				//else
				//{
				//	// ���R���W�����ɓ������Ă��Ȃ��Ċ��W�����v��Ԃł͂Ȃ������ꍇ��
				//	if (m_animType != PlayerAnimation::WalkJump)
				//	{
				//		// �W�����v���ɂ���
				//		m_animType = PlayerAnimation::WalkJump;

				//		// ������Ƃ����W�����v����
				//		//m_jumpPower = kJumpSpeed / 2;

				//		// �A�j���[�V�����͗������̂��̂ɂ���
				//		SwitchAnimation(m_animType,true, false, 4);
				//	}
				//}
			}
		}


	}

	// �V�������W��ۑ�����
	m_pos = nowPos;

#else
	VECTOR nowPos = VAdd(m_pos, m_vel);	//�ړ���̍��W

	MV1_COLL_RESULT_POLY_DIM HitPolyDim; //�����蔻��̌���

	int wallNum;//�ǃ|���S���̐�
	int floorNum;//���|���S���̐�

	MV1_COLL_RESULT_POLY* wallCol[kMaxHitColl];//�ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̔z��
	MV1_COLL_RESULT_POLY* floorCol[kMaxHitColl];//���|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̔z��

	bool isMoveX = false;//���s(x����)�Ɉړ��������ǂ���
	if (fabs(m_vel.x) > 0.01f)
	{
		isMoveX = true;
	}

	for (auto& mapModel : m_map->GetPiece())
	{
		//���f����\�����Ă��Ȃ��Ƃ��͏��������Ȃ�
		if (!mapModel.model->IsExist())	continue;

		// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����
		// ( ���o����͈͈͂ړ��������l������ )
		HitPolyDim = MV1CollCheck_Sphere(mapModel.model->GetModelHandle(), -1, VAdd(m_pos,VGet(0.0f,0.3f,0.0f)), kCapsuleRadius + VSize(m_vel));

		////�v���C���[(�J�v�Z��)�ƃ}�b�v(���f��)�̓����蔻��
		//HitPolyDim = MV1CollCheck_Capsule(mapModel.model->GetModelHandle(), -1, VAdd(nowPos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitCapsuleTop, 0.0f)), kSizeX);

		//�ǂ��ɂ�������Ȃ������珈�������Ȃ�
		if (HitPolyDim.HitNum == 0)	continue;

		wallNum = 0;
		floorNum = 0;

		//���������|���S���̐������J��Ԃ�
		for (int i = 0; i < HitPolyDim.HitNum; i++)
		{
			//xz���ʂɐ������ǂ����̓|���S���̖@����y������0�Ɍ���Ȃ��߂����ǂ����Ŕ��f����
			if (HitPolyDim.Dim[i].Normal.y < 0.000001f && HitPolyDim.Dim[i].Normal.y > -0.000001f)
			{
				//�ǂƔ��f���ꂽ�|���S���ł��A�v���C���[��y���W����0.2f��荂���|���S���̂ݓ����蔻����s��
				if (HitPolyDim.Dim[i].Position[0].y > m_pos.y + 0.2f ||
					HitPolyDim.Dim[i].Position[1].y > m_pos.y + 0.2f || 
					HitPolyDim.Dim[i].Position[2].y > m_pos.y + 0.2f)
				{
					//�|���S�����z��ł�����E���ɒB���Ă��Ȃ�������|���S����z��ɒǉ�
					if (wallNum < kMaxHitColl)
					{
						//�|���S���̍\���̂̃A�h���X���|���S���|�C���^�z��ɕۑ�����
						wallCol[wallNum] = &HitPolyDim.Dim[i];
						//�ǃ|���S���̐������Z����
						wallNum++;
					}
				}
			}
			else
			{
				if (floorNum < kMaxHitColl)
				{
					//�|���S���̍\���̂̃A�h���X���|���S���|�C���^�z��ɕۑ�����
					floorCol[floorNum] = &HitPolyDim.Dim[i];
					//�ǃ|���S���̐������Z����
					floorNum++;
				}
			}
		}

		bool isHit = false;
		//�ǃ|���S���Ƃ̓����蔻��
		if (wallNum != 0)
		{
			//���ړ����Ă��鎞
			if (isMoveX)
			{
				//�ǃ|���S���̐������J��Ԃ�
				for (int i = 0; i < wallNum; i++)
				{
					//�|���S���ƃv���C���[���������Ă��Ȃ������珈�����������̃J�E���g��
					if (HitCheck_Capsule_Triangle(VAdd(nowPos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitCapsuleTop, 0.0f)), kSizeX,
						wallCol[i]->Position[0], wallCol[i]->Position[1], wallCol[i]->Position[2]) == FALSE)
					{
						continue;
					}

					//�|���S���Ɠ�������
					isHit = true;

					// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
					{
						VECTOR SlideVec;	// �v���C���[���X���C�h������x�N�g��

						// �i�s�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o
						SlideVec = VCross(m_vel, wallCol[i]->Normal);

						// �Z�o�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o�A���ꂪ
						// ���̈ړ���������Ǖ����̈ړ������𔲂����x�N�g��
						SlideVec = VCross(wallCol[i]->Normal, SlideVec);

						// ������ړ��O�̍��W�ɑ��������̂�V���ȍ��W�Ƃ���
						nowPos = VAdd(m_pos, SlideVec);
					}

					//�V�������W�ŕǃ|���S���Ɠ������Ă��Ȃ����ǂ����𔻒f����
					int j = 0;
					for (j = 0; j < wallNum; j++)
					{
						//�������Ă�����j�̃��[�v���甲����
						if (HitCheck_Capsule_Triangle(VAdd(nowPos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitCapsuleTop, 0.0f)), kSizeX,
							wallCol[j]->Position[0], wallCol[j]->Position[1], wallCol[j]->Position[2]) == TRUE)
						{
							continue;
						}
					}

					//j���ǃ|���S���̐��������ꍇ�͂ǂ��ɂ�������Ȃ�����
					if (j == wallNum)
					{
						//�ǂ��ɂ�������Ȃ��Ȃ���
						isHit = false;
						//i�̃��[�v�𔲂���
						break;
					}

				}
			}
			//���ړ����Ă��Ȃ�
			else
			{
				//�ǃ|���S���̐������J��Ԃ�
				for (int i = 0; i < wallNum; i++)
				{
					//�|���S���ɓ������Ă�����t���O�𗧂Ă������Ń��[�v���甲����
					if (HitCheck_Capsule_Triangle(VAdd(nowPos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitCapsuleTop, 0.0f)), kSizeX,
						wallCol[i]->Position[0], wallCol[i]->Position[1], wallCol[i]->Position[2]) == TRUE)
					{
						isHit = true;
						break;
					}
				}
			}

			//�ǃ|���S���ɓ������Ă����牟���o���������s��
			if (isHit)
			{
				//���o���������݂�
				for (int i = 0; i < kHitTryNum; i++)
				{
					//�ǃ|���S���̐��������݂�
					int j = 0;
					for (j = 0; j < wallNum; j++)
					{
						//�|���S���ƃv���C���[���������Ă��Ȃ������珈�����������̃J�E���g��
						if (HitCheck_Capsule_Triangle(VAdd(nowPos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitCapsuleTop, 0.0f)), kSizeX,
							wallCol[j]->Position[0], wallCol[j]->Position[1], wallCol[j]->Position[2]) == FALSE)
						{
							continue;
						}

						//�������Ă�����ړ������Ɣ��΂ɃX���C�h������
						nowPos = VAdd(nowPos, VScale(VGet(wallCol[i]->Normal.x, 0.0f, 0.0f), kSlideMoveSpeed));


						//�V�������W�ŕǃ|���S���Ɠ������Ă��Ȃ����ǂ����𔻒f����
						int k = 0;
						for (k = 0; k < wallNum; k++)
						{
							//�������Ă�����k�̃��[�v���甲����
							if (HitCheck_Capsule_Triangle(VAdd(nowPos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitCapsuleTop, 0.0f)), kSizeX,
								wallCol[k]->Position[0], wallCol[k]->Position[1], wallCol[k]->Position[2]) == TRUE)
							{
								continue;
							}
						}

						//k���ǃ|���S���̐��������ꍇ�͂ǂ��ɂ�������Ȃ�����
						if (k == wallNum)
						{
							//�ǂ��ɂ�������Ȃ��Ȃ���
							isHit = false;
							//j�̃��[�v�𔲂���
							break;
						}
					}

					//j���ǃ|���S���̐��ł͂Ȃ��ꍇ
					if (j != wallNum)
					{
						//�S�Ă̕ǃ|���S���ƐڐG���Ȃ��Ȃ����̂�i(�����o������)�̃��[�v�𔲂���
						break;
					}

				}
			}

		}


		HITRESULT_LINE LineRes;
		//���|���S���Ƃ̓����蔻��
		if (floorNum != 0)
		{
			isHit = false;
			//�W�����v���ŁA�㏸���̎�
			if (m_animType == PlayerAnimation::WalkJump && m_jumpPower > 0.0f)
			{
				//��ԒႢ�V��ɂԂ��邽�߂̔���p
				float MinY = 0.0f;

				//���|���S���̐������J��Ԃ�
				for (int i = 0; i < floorNum; i++)
				{
					//���悩�瓪�̍����܂ł̐�
					LineRes = HitCheck_Line_Triangle(VAdd(nowPos, VGet(0.0f, -0.1f, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitLineTop, 0.0f)),
						floorCol[i]->Position[0], floorCol[i]->Position[1], floorCol[i]->Position[2]);

					//�������Ă��Ȃ������珈�������Ȃ�
					if (!LineRes.HitFlag)	continue;

					//���łɓ������Ă��āA�����o�����ʒu���������ꍇ�͉������Ȃ�
					if (isHit && MinY < LineRes.Position.y)	continue;

					//�|���S���ɓ��������t���O
					isHit = true;
					//�ڐG����y���W��ۑ�
					MinY = LineRes.Position.y;
				}

				//�������Ă��鎞
				if (isHit)
				{
					//�v���C���[��y���W�𓖂��������W�����ɍX�V
					nowPos.y = MinY - 0.9f;
					//y�����̑��x�͔��]
					m_jumpPower *= -1.0f;
				}
			}
			//���������W�����v���ł͂Ȃ��Ƃ�
			else
			{
				isHit = false;
				//��ԍ������ɂԂ��邽�߂̔���p
				float MaxY = 0.0f;

				//���|���S���̐������J��Ԃ�
				for (int i = 0; i < floorNum; i++)
				{
					//�W�����v��
					if (m_animType == PlayerAnimation::WalkJump)
					{
						//�W�����v���̏ꍇ�͓��̍������瑫���菭���Ⴂ�ʒu�̊Ԃœ������Ă��邩�𔻒�
						LineRes = HitCheck_Line_Triangle(VAdd(nowPos, VGet(0.0f, kHitLineBottom, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitLineTop, 0.0f)),
							floorCol[i]->Position[0], floorCol[i]->Position[1], floorCol[i]->Position[2]);
					}
					else
					{
						//�����Ă���ꍇ�͓��̐悩�炻�������Ⴂ�ʒu�̊Ԃœ������Ă��邩�𔻒�
						LineRes = HitCheck_Line_Triangle(VAdd(nowPos, VGet(0.0f, kHitLineBottom*2, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitLineTop, 0.0f)),
							floorCol[i]->Position[0], floorCol[i]->Position[1], floorCol[i]->Position[2]);
					}

					//�������Ă��Ȃ��Ƃ�
					if (!LineRes.HitFlag)	continue;

					//�������Ă��āA�����o�����ʒu�����Ⴂ�ꍇ�͉������Ȃ�
					if (isHit && MaxY > LineRes.Position.y)	continue;

					//�|���S���ɓ��������t���O
					isHit = true;
					//�ڐG����y���W��ۑ�
					MaxY = LineRes.Position.y;
				}

				//�������Ă�����
				if (isHit)
				{
					//�ڐG�����|���S���ň�ԍ���y���W���v���C���[��y���W��
					nowPos.y = MaxY + 0.1f;

					//�W�����v�͂�0��
					m_jumpPower = 0;

					//�W�����v������������Idle��Ԃɂ���
					if (m_animType == PlayerAnimation::WalkJump)
					{
						if (isMoveX)
						{
							m_animType = PlayerAnimation::Walk;
							SwitchAnimation(m_animType, true, false, 4);
						}
						else
						{
							m_animType = PlayerAnimation::Idle;
							SwitchAnimation(m_animType, true, false, 4);
						}
					}
				}
				//�������Ă��Ȃ��Ƃ�
				else
				{
					//�W�����v��Ԃł͂Ȃ������Ƃ�
					if (m_animType != PlayerAnimation::WalkJump)
					{
						//�W�����v��Ԃɂ���
						m_animType = PlayerAnimation::WalkJump;
						SwitchAnimation(m_animType, true, false, 4);
						//������ƃW�����v����
 						m_jumpPower += kJumpSpeed / 2;
					}
				}
			}

			/*isHit = false;
			//�W�����v�̏�����(���܂��͓V��ɓ������Ă��邩��)
			m_jumpPower = 0.0f;

			//���|���S���̐������J��Ԃ�
			for (int i = 0; i < floorNum; i++)
			{
				//�|���S���ƃv���C���[���������Ă��Ȃ������珈�����������̃J�E���g��
				if (HitCheck_Capsule_Triangle(VAdd(nowPos, VGet(0.0f, 0.3f, 0.0f)), VAdd(nowPos, VGet(0.0f, 0.6f, 0.0f)), kSizeX,
					floorCol[i]->Position[0], floorCol[i]->Position[1], floorCol[i]->Position[2]) == FALSE)
				{
					continue;
				}

				//�|���S���Ɠ�������
				isHit = true;

				// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
				{
					VECTOR SlideVec;	// �v���C���[���X���C�h������x�N�g��

					// �i�s�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o
					SlideVec = VCross(m_vel, floorCol[i]->Normal);

					// �Z�o�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o�A���ꂪ
					// ���̈ړ���������Ǖ����̈ړ������𔲂����x�N�g��
					SlideVec = VCross(floorCol[i]->Normal, SlideVec);

					// ������ړ��O�̍��W�ɑ��������̂�V���ȍ��W�Ƃ���
					nowPos = VAdd(m_pos, SlideVec);
				}

				//�V�������W�ŕǃ|���S���Ɠ������Ă��Ȃ����ǂ����𔻒f����
				int j = 0;
				for (j = 0; j < floorNum; j++)
				{
					//�������Ă�����j�̃��[�v���甲����
					if (HitCheck_Capsule_Triangle(VAdd(nowPos, VGet(0.0f, 0.3f, 0.0f)), VAdd(nowPos, VGet(0.0f, 0.6f, 0.0f)), kSizeX,
						floorCol[j]->Position[0], floorCol[j]->Position[1], floorCol[j]->Position[2]) == TRUE)
					{
						continue;
					}
				}

				//j���ǃ|���S���̐��������ꍇ�͂ǂ��ɂ�������Ȃ�����
				if (j == floorNum)
				{
					//�ǂ��ɂ�������Ȃ��Ȃ���
					isHit = false;
					//i�̃��[�v�𔲂���
					break;
				}
			}

			//�ǃ|���S���ɓ������Ă����牟���o���������s��
			if (isHit)
			{
				//���o���������݂�
				for (int i = 0; i < kHitTryNum; i++)
				{
					//�ǃ|���S���̐��������݂�
					int j = 0;
					for (j = 0; j < floorNum; j++)
					{
						//�|���S���ƃv���C���[���������Ă��Ȃ������珈�����������̃J�E���g��
						if (HitCheck_Capsule_Triangle(VAdd(nowPos, VGet(0.0f, 0.3f, 0.0f)), VAdd(nowPos, VGet(0.0f, 0.6f, 0.0f)), kSizeX,
							floorCol[j]->Position[0], floorCol[j]->Position[1], floorCol[j]->Position[2]) == FALSE)
						{
							continue;
						}

						//�������Ă�����ړ������Ɣ��΂ɃX���C�h������
						nowPos = VAdd(nowPos, VScale(VGet(floorCol[i]->Normal.x, 0.0f, 0.0f), kSlideMoveSpeed));


						//�V�������W�ŕǃ|���S���Ɠ������Ă��Ȃ����ǂ����𔻒f����
						int k = 0;
						for (k = 0; k < floorNum; k++)
						{
							//�������Ă�����k�̃��[�v���甲����
							if (HitCheck_Capsule_Triangle(VAdd(nowPos, VGet(0.0f, 0.3f, 0.0f)), VAdd(nowPos, VGet(0.0f, 0.6f, 0.0f)), kSizeX,
								wallCol[k]->Position[0], floorCol[k]->Position[1], floorCol[k]->Position[2]) == TRUE)
							{
								continue;
							}
						}

						//k���ǃ|���S���̐��������ꍇ�͂ǂ��ɂ�������Ȃ�����
						if (k == floorNum)
						{
							//�ǂ��ɂ�������Ȃ��Ȃ���
							isHit = false;
							//j�̃��[�v�𔲂���
							break;
						}
					}

					//j���ǃ|���S���̐��ł͂Ȃ��ꍇ
					if (j != floorNum)
					{
						//�S�Ă̕ǃ|���S���ƐڐG���Ȃ��Ȃ����̂�i(�����o������)�̃��[�v�𔲂���
						break;
					}

				}
			}*/

		}

		// �����蔻����̌�n��
		MV1CollResultPolyDimTerminate(HitPolyDim);
	}


	// �V�������W��ۑ�����
	m_pos = nowPos;
#endif
}

