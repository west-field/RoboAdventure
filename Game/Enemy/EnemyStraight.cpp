#include "EnemyStraight.h"
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
	constexpr float kGravity = -0.1f;//�d��

	constexpr int kEffectSize = 516;//�G�t�F�N�g�̑傫��
	constexpr int kAnimNum = 22;//�G�t�F�N�g�摜�̐�
	constexpr int kAnimSpeed = 3;//�G�t�F�N�g�̍Đ��X�s�[�h

	constexpr int kScore = 100;//�X�R�A

	constexpr int kMaxHitColl = 1000;//�擾�ł��铖�������|���S���̐�

	constexpr float kSizeX = 0.6f;//���f���̑傫��X
	constexpr float kSizeY = 0.8f;//���f���̑傫��Y

	constexpr float kCapsuleRadius = 0.6f;//���̔��a

	constexpr int kSlideTryNum = 16;//�X���C�h�����݂��

	constexpr float kSlideMoveSpeed = 0.01f;//�X���C�h

	constexpr float kHitLineTop = kSizeY;//���̏�̈ʒu
	constexpr float kHitLineBottom = -0.1f;//���̉��̈ʒu
}

EnemyStraight::EnemyStraight(VECTOR pos, int modelHandle, bool isLeft, std::shared_ptr<Map> map) :
	EnemyBase(pos, modelHandle, kScale, isLeft,map)
{
	size = VGet(0.6f, kSizeY, 0.4f);

	SetLTRB(VGet(-0.4f, size.y, 0.0f), VGet(-0.4f, 0.0f, 0.0f),
		VGet(0.4f, size.y, 0.0f), VGet(0.4f, 0.0f, 0.0f));

	m_model->SetAnimation(static_cast<int>(Animation::Idle), true, true);
}

EnemyStraight::~EnemyStraight()
{

}

void EnemyStraight::Update()
{
	//�A�j���[�V������i�߂�
	m_model->Update();

	//�������Ă�������Ɉړ�����
	if (m_isLeft)
	{
		m_dir = VGet(-1, 0, 0);
		m_rot = VGet(0.0f, 95.0f, 0.0f);
	}
	else
	{
		m_dir = VGet(1, 0, 0);
		m_rot = VGet(0.0f, -95.0f, 0.0f);
	}

	// �[�����Z����
	if (VSquareSize(m_dir) > 0)
	{
		// ���K��
		m_dir = VNorm(m_dir);
	}
	m_vel = VScale(m_dir, kSpeed);
	m_dir.y = -1.0f;
	m_vel.y += m_dir.y * kGravity;

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

void EnemyStraight::Draw()
{
	//���f����\��
	m_model->Draw();
}

int EnemyStraight::Score()
{
	return kScore;
}

void EnemyStraight::Move()
{
	bool isHitFlag;
	MV1_COLL_RESULT_POLY_DIM HitDim;//���͂ɂ���|���S�������o�X�^���ʂ��������铖���蔻�茋�ʍ\����
	int wallCollisionNum;//�ǃ|���S���Ɣ��f���ꂽ�|���S���̐�
	int floorCollisionNum;//���|���S���Ɣ��肳�ꂽ�|���S���̐�
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

		// ���|���S���Ƃ̓����蔻��
		if (floorCollisionNum != 0)
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
				// �����Ă���ꍇ�͓��̐悩�炻�������Ⴂ�ʒu�̊Ԃœ������Ă��邩�𔻒�( �X�΂ŗ�����ԂɈڍs���Ă��܂�Ȃ��� )
				LineRes = HitCheck_Line_Triangle(VAdd(nowPos, VGet(0.0f, kHitLineTop, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitLineBottom - 0.2f, 0.0f)),
					floorCol[i]->Position[0], floorCol[i]->Position[1], floorCol[i]->Position[2]);

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
				// �ڐG�����|���S���ň�ԍ����x���W���v���C���[�̂x���W�ɂ���
				nowPos.y = MaxY;
			}
		}


	}

	// �V�������W��ۑ�����
	m_pos = nowPos;
}

const VECTOR EnemyStraight::GetEffectPos() const
{
	return VAdd(m_pos, VGet(0.0f, kSizeY / 2, 0.0f));
}