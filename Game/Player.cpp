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
	constexpr float kJumpSpeed = 1.0f;//�W�����v��
	constexpr float kGravity = 0.1f;//�d��

	constexpr int kSlideTryNum = 16;//�ʒu�̕␳�������
	constexpr float kSlideMoveSpeed = 0.1f;//�ʒu��␳����ۂɈ��ňړ��ł��鋗��

	constexpr int kMaxHitColl = 1000;// ��������R���W�����|���S���̍ő吔

	constexpr float kCapsuleRadius = 0.25f;		//�J�v�Z���̔��a

	constexpr float kHitCapsuleTop = 0.6f;		//�J�v�Z���̏�̈ʒu
	constexpr float kHitCapsuleBottom = 0.3f;	//�J�v�Z���̉��̈ʒu

	constexpr float kHitLineTop = 0.9f;		//���̏�̈ʒu
	constexpr float kHitLineBottom = -0.1f;	//���̉��̈ʒu

	constexpr float kScale = 0.002f;//���f���̑傫��

	constexpr float kSizeX = 0.3f;//�v���C���[�̃T�C�YX
	constexpr float kSizeY = 0.9f;//�v���C���[�̃T�C�YY

	constexpr int kMaxHp = 3;//�ő�HP
}

Player::Player(std::shared_ptr<Map> map) : m_update(&Player::NormalUpdate)
{

	m_model = std::make_shared<Model>(L"Data/Model/Robot.mv1", kScale, true);
#if _DEBUG
	m_pos = VGet(10.0f, -0.3f, 0);
#else
	m_pos = VGet(10.0f, -0.3f, 0);
#endif
	m_model->SetPos(m_pos);

	m_animType = PlayerAnimation::Idle;//�ŏ��͗����~�܂��Ă���A�j���[�V����
	m_model->SetAnimation(static_cast<int>(m_animType), true, true);

	m_map = map;

	m_jumpPower = 0.0f;

	m_dir = VGet(0, 0, 1);
	m_rot = VGet(0.0f, -95.0f, 0.0f);
	m_model->SetRot(m_rot);

	m_isDamage = false;
	m_flashing = 0;

	m_hp = std::make_shared<Hp>();
	m_hp->MaxHp(kMaxHp);

	//float left = VAdd(m_pos, VGet(kSizeX, 0.0f, 0.0f)).x;
	//float right = VAdd(m_pos, VGet(-kSizeX, 0.0f, 0.0f)).x;
	//float top = VAdd(m_pos, VGet(0.0f, kSizeY, 0.0f)).y;
	//float bottom = m_pos.y;
	//m_line[static_cast<int>(Direction::Top)].lineFirst = VGet(left, top, 0.0f);
	//m_line[static_cast<int>(Direction::Top)].lineSecond = VGet(right, top, 0.0f);
	//m_line[static_cast<int>(Direction::Top)].isWidth = true;
	//m_line[static_cast<int>(Direction::Bottom)].lineFirst = VGet(left , bottom, 0.0f);
	//m_line[static_cast<int>(Direction::Bottom)].lineSecond = VGet(right , bottom, 0.0f);
	//m_line[static_cast<int>(Direction::Bottom)].isWidth = true;
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
	//m_model->DeleteModel();//�����ŏ�����
}

void Player::Update(const InputState& input)
{
	//�U�����󂯂Ă���&&�_�Ŏ��Ԃ�0��菬������
	if (m_isDamage && m_flashing <= 0)
	{
		m_isDamage = false;//�U�����󂯂Ă��Ȃ��悤�ɂ���
	}
	//�_�Ŏ��Ԃ�0�����傫���Ƃ�
	else if(m_flashing > 0)
	{
		m_flashing--;//���炷
	}

	(this->*m_update)(input);

	//�����蔻��
	Move();
	
	//���ۂɃ��f���̈ʒu�A��]��ύX
	m_model->SetPos(m_pos);
	m_model->SetRot(m_rot);
	m_model->Update();
}

void Player::Update()
{
	//�_�Ŏ��Ԃ�0�o�Ȃ������Ƃ�
	if (m_flashing != 0)
	{
		m_flashing = 0;//�_�ł����Ȃ��悤��0�ɂ���
	}

	m_model->Update();
}

void Player::Draw()
{
	//HP��\��
	m_hp->Draw();
	//�U�����󂯂��Ƃ��͓_�ł�����
	if (m_flashing % 10 != 0)	return;
	//���f���\��
	m_model->Draw();
#if _DEBUG
	DrawFormatString(0, 20, 0xffffff, L"pos.x%lf,y%lf,z%lf", m_pos.x, m_pos.y, m_pos.z);//���f���\���ʒu
	DrawFormatString(0, 40, 0xffffff, L"jump%lf", m_jumpPower);//�W�����v�̍�����ύX

	DrawSphere3D(VAdd(m_pos, VGet(0.0f, kSizeY / 3, 0.0f)), kCapsuleRadius + VSize(m_dir), 10, 0xffffff, 0xffffff, false);//�X�t�B�A
	DrawCapsule3D(VAdd(m_pos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(m_pos, VGet(0.0f, kHitCapsuleTop, 0.0f)),
		kCapsuleRadius, 10, 0xffffff, 0xffffff, false);//�J�v�Z��

	DrawLine3D(VAdd(m_pos, VGet(0.0f, (kHitCapsuleTop + 0.5f), 0.0f)), VAdd(m_pos, VGet(0.0f, kHitLineBottom - 0.2f, 0.0f)), 0xff0000);//��

	//���Đ����Ă���A�j���[�V�����𕶎��ŕ\��
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

	//HitNum��0�ȊO�̎�
	if (dim.HitNum != 0)
	{
		MV1CollResultPolyDimTerminate(dim);
		return true;//�������Ă���
	}

	MV1CollResultPolyDimTerminate(dim);
	return false;//�������Ă��Ȃ�
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
	if (m_jumpPower > 0.0f)
	{
		m_jumpPower += kJumpSpeed / 1.5f;
	}
	else
	{
		m_jumpPower = kJumpSpeed / 1.5f;
	}
	m_update = &Player::JumpUpdate;
}

bool Player::IsAttack()
{
	return m_isAttack;
}

bool Player::IsJump()
{
	//�A�j���[�V�������W�����v���Ă���Ƃ�
	if (m_animType == PlayerAnimation::WalkJump)
	{
		return true;//�W�����v���Ă���
	}
	return false;//�W�����v���Ă��Ȃ�
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
	//�L�����N�^�[�̈ʒu��1.0f�����������Ƃ� && ��̍��W�ł���킳���{�b�N�X���J�����̎��E�ɓ����Ă��Ȃ����ǂ����𔻒肷��
	if (m_pos.y <= 1.0f && CheckCameraViewClip_Box(VAdd(m_pos, VGet(kSizeX, kSizeY, kSizeX)),
		VAdd(m_pos, VGet(-kSizeX, 0.0f, -kSizeX))))//true:���E�ɓ����Ă��Ȃ� false:���E�ɓ����Ă���
	{
		return false;
	}
	return true;
}

void Player::NormalUpdate(const InputState& input)
{
	m_dir = VGet(0.0f, 0.0f, 0.0f);//������
	m_vel = VGet(0.0f, 0.0f, 0.0f);//������
	bool isPrees = false;
#if false
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

	if (m_animType != PlayerAnimation::WalkJump)
	{
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
			//�����A�j���[�V�����ɕύX
			m_animType = PlayerAnimation::Walk;
			SwitchAnimation(PlayerAnimation::Walk, true, false, 4);
		}
		else
		{
			//�����Ă��Ȃ��Ƃ��ҋ@�A�j���[�V�����ɕύX����
			m_animType = PlayerAnimation::Idle;
			SwitchAnimation(PlayerAnimation::Idle, true, false, 4);
		}
		
		//�W�����v��Ԃł͂Ȃ��A�W�����v�{�^���������ꂽ��
		if (input.IsTriggered(InputType::junp))
		{
			//�W�����v��
			SoundManager::GetInstance().Play(SoundId::PlayerJump);
			//�W�����v��Ԃɂ���
			m_animType = PlayerAnimation::WalkJump;
			//�A�j���[�V�����̕ύX
			SwitchAnimation(m_animType, true, false, 4);
			//Y�������̑��x���Z�b�g
			m_jumpPower = kJumpSpeed;
			m_vel.y = m_jumpPower;
			//�W�����v�̃A�b�v�f�[�g�ֈړ�
			m_update = &Player::JumpUpdate;
		}
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
	m_vel = VScale(VGet(m_dir.x, 0.0f, 0.0f), kMoveSpeed / 1.5f);
	m_dir.y = -1.0f;
	m_vel.y += m_dir.y * kGravity;

	//��Ԃ��W�����v�̏ꍇ��
	if (m_animType == PlayerAnimation::WalkJump && m_jumpPower > 0.0f)
	{
		//�d�͕�����
		m_jumpPower -= kGravity;

		//�ړ��x�N�g����Y�����ɑ������
		m_vel.y += m_jumpPower;
	}
	else if(m_animType != PlayerAnimation::WalkJump)
	{
		m_jumpPower = 0.0f;
		m_update = &Player::NormalUpdate;
	}
	else
	{
		m_isAttack = true;
	}
}

void Player::Move()
{
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

		//HitNum��0�̎�(�ǂ��ɂ��������Ă��Ȃ���)���������Ȃ�
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
					// �ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{1.0f��荂���|���S���̂ݓ����蔻����s��
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

					// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
					nowPos = VAdd(nowPos, VScale(VGet(wallCol[i]->Normal.x, 0.0f, 0.0f), kSlideMoveSpeed));
					nowPos.z = 0.0f;

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
					LineRes = HitCheck_Line_Triangle(VAdd(nowPos, VGet(0.0f, kHitLineTop + 0.5f, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitLineBottom+1.0f, 0.0f)),
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
					nowPos.y = MinY - (kHitCapsuleTop + 0.5f);

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

					if (MaxY < -0.38f)
					{
						MaxY = -0.38f;
					}
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
}

