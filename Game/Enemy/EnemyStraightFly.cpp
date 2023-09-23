#include "EnemyStraightFly.h"
#include "../../Model.h"
#include "../../Util/DrawFunctions.h"
#include "../../WorldSprite.h"
#include "../Map.h"

#include "EffekseerForDXLib.h"

namespace
{
	// ���x�i1=1m�A60fps�Œ�Ƃ��āA����10km�j10000m �� ���� �� �� �� �b �� �t���[��
	constexpr float kSpeed = static_cast<float>(10000.0 / 60.0 / 60.0 / 60.0) - 0.01f;

	constexpr float kScale = 0.005f;//���f���̑傫��

	constexpr int kEffectSize = 516;//�G�t�F�N�g�̑傫��
	constexpr int kAnimNum = 22;//�G�t�F�N�g�摜�̐�
	constexpr int kAnimSpeed = 3;//�G�t�F�N�g�̍Đ��X�s�[�h

	constexpr int kScore = 150;

	constexpr int kMaxHitColl = 1000;//�擾�ł��铖�������|���S���̐�

	constexpr float kSizeX = 0.6f;//���f���̑傫��X
	constexpr float kSizeY = 0.8f;//���f���̑傫��Y

	constexpr float kCapsuleRadius = 0.6f;//���̔��a

	constexpr int kSlideTryNum = 16;//�X���C�h�����݂��

	constexpr float kSlideMoveSpeed = 0.01f;//�X���C�h

	constexpr float kHitLineTop = kSizeY;//���̏�̈ʒu
	constexpr float kHitLineBottom = -0.1f;//���̉��̈ʒu
}

EnemyStraightFly::EnemyStraightFly(VECTOR pos, int modelHandle, bool isLeft, std::shared_ptr<Map> map) :
	EnemyBase(pos, modelHandle, kScale, isLeft,map)
{
	size = VGet(0.6f, 0.8f, 0.4f);
	SetLTRB(VGet(-0.4f, size.y, 0.0f), VGet(-0.4f, 0.0f, 0.0f),
		VGet(0.4f, size.y, 0.0f), VGet(0.4f, 0.0f, 0.0f));

	m_model->SetAnimation(static_cast<int>(Animation::Flying), true, true);
}

EnemyStraightFly::~EnemyStraightFly()
{
}

void EnemyStraightFly::Update()
{
	//�A�j���[�V������i�߂�
	m_model->Update();

	//�������Ă�������Ɉړ�����
	if (m_isLeft)
	{
		m_dir = VAdd(m_dir, VGet(-1, 0, 0));
		m_rot = VGet(0.0f, -80.0f, 0.0f);
	}
	else
	{
		m_dir = VAdd(m_dir, VGet(1, 0, 0));
		m_rot = VGet(0.0f, 80.0f, 0.0f);
	}

	// �[�����Z����
	if (VSquareSize(m_dir) > 0)
	{
		// ���K��
		m_dir = VNorm(m_dir);
	}
	m_vel = VScale(m_dir, kSpeed);

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

void EnemyStraightFly::Draw()
{
	//���f����\��
	m_model->Draw();
}

int EnemyStraightFly::Score()
{
	return kScore;
}

void EnemyStraightFly::Move()
{
	bool isHitFlag;
	MV1_COLL_RESULT_POLY_DIM HitDim;//���͂ɂ���|���S�������o�X�^���ʂ��������铖���蔻�茋�ʍ\����
	int wallCollisionNum;//�ǃ|���S���Ɣ��f���ꂽ�|���S���̐�
	MV1_COLL_RESULT_POLY* wallCol[kMaxHitColl];	// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
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

		// ���o���ꂽ�|���S�����ǃ|���S��( �w�y���ʂɐ����ȃ|���S�� )�����|���S��( �w�y���ʂɐ����ł͂Ȃ��|���S�� )���𔻒f����
		{
			// �ǃ|���S���Ə��|���S���̐�������������
			wallCollisionNum = 0;

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
			}
		}

		// �ǃ|���S���Ƃ̓����蔻�菈��
		if (wallCollisionNum != 0)
		{
			// �ǂɓ����������ǂ����̃t���O�͏�����Ԃł́u�������Ă��Ȃ��v�ɂ��Ă���
			isHitFlag = false;

			// �ǃ|���S���̐������J��Ԃ�
			for (int i = 0; i < wallCollisionNum; i++)
			{
				// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
				if (HitCheck_Sphere_Triangle(VAdd(m_pos, VGet(0.0f, kSizeY / 3, 0.0f)), kCapsuleRadius,
					wallCol[i]->Position[0], wallCol[i]->Position[1], wallCol[i]->Position[2]) == FALSE)	continue;

				// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
				isHitFlag = true;

				m_isLeft = !m_isLeft;//�ǂƓ���������t�����Ɉړ�������

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
					if (HitCheck_Sphere_Triangle(VAdd(m_pos, VGet(0.0f, kSizeY / 3, 0.0f)), kCapsuleRadius,
						wallCol[i]->Position[0], wallCol[i]->Position[1], wallCol[i]->Position[2]) == TRUE) break;
				}

				// j �� KabeNum �������ꍇ�͂ǂ̃|���S���Ƃ�������Ȃ������Ƃ������ƂȂ̂�
				// �ǂɓ��������t���O��|������Ń��[�v���甲����
				if (j == wallCollisionNum)
				{
					isHitFlag = false;
					break;
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
						if (HitCheck_Sphere_Triangle(VAdd(m_pos, VGet(0.0f, kSizeY / 3, 0.0f)), kCapsuleRadius,
							wallCol[i]->Position[0], wallCol[i]->Position[1], wallCol[i]->Position[2]) == FALSE) continue;

						// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
						nowPos = VAdd(nowPos, VScale(wallCol[i]->Normal, kSlideMoveSpeed));
						nowPos.z = 0.0f;

						int j = 0;
						// �ړ�������ŕǃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
						for (j = 0; j < wallCollisionNum; j++)
						{
							if (HitCheck_Sphere_Triangle(VAdd(m_pos, VGet(0.0f, kSizeY / 3, 0.0f)), kCapsuleRadius,
								wallCol[i]->Position[0], wallCol[i]->Position[1], wallCol[i]->Position[2]) == TRUE) break;
						}

						// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
						if (j == wallCollisionNum) break;
					}

					// i �� KabeNum �ł͂Ȃ��ꍇ�͑S���̃|���S���ŉ����o�������݂�O�ɑS�Ă̕ǃ|���S���ƐڐG���Ȃ��Ȃ����Ƃ������ƂȂ̂Ń��[�v���甲����
					if (i != wallCollisionNum) break;
				}
			}
		}
	}

	// �V�������W��ۑ�����
	m_pos = nowPos;
}

const VECTOR EnemyStraightFly::GetEffectPos() const
{
	return VAdd(m_pos, VGet(0.0f, kSizeY / 2, 0.0f));
}
