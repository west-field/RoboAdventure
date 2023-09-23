#include "Model.h"
#include <cassert>

namespace
{
	//�����蔻��Ƃ��Ďg�p����t���[���̖��O
	const TCHAR* const kCollisionFrameName = L"Collision";
}

//���f���t�@�C�������w�肵�ă��[�h���s��
Model::Model(const TCHAR* fileName,float scale,bool useCollision) :
	m_modelHandle(-1), m_isUseCollision(useCollision), m_isUpdateVollision(false), m_animChangeFrame(0),
	m_animChangeFrameTotal(0), m_isExist(true)
{
	m_modelHandle = MV1LoadModel(fileName);
	//���f���̃��[�h�Ɏ��s����
	assert(m_modelHandle != -1);
	if (m_isUseCollision)
	{
		MV1SetupCollInfo(m_modelHandle, -1);
	}
	//�A�j���[�V�����f�[�^�̃N���A
	ClearAnimData(m_animPrev);
	ClearAnimData(m_animNext);
	SetScale(VGet(scale, scale, scale));
}
//�w�肳�ꂽ�n���h���̃��f����Duplicate���Đ�������
Model::Model(int orgModel, float scale, bool useCollision) :
	m_modelHandle(-1), m_isUseCollision(useCollision), m_isUpdateVollision(false), m_animChangeFrame(0),
	m_animChangeFrameTotal(0)
{
	//���[�h�Ɏ��s�������f�����R�s�[���Ƃ��Ďw�肳��Ă���
	assert(orgModel != -1);
	m_modelHandle = MV1DuplicateModel(orgModel);
	assert(m_modelHandle != -1);
	if (m_isUseCollision)
	{
		MV1SetupCollInfo(m_modelHandle, -1);
	}
	//�A�j���[�V�����f�[�^�̃N���A
	ClearAnimData(m_animPrev);
	ClearAnimData(m_animNext);
	SetScale(VGet(scale, scale, scale));
}
//�f�X�g���N�^
Model::~Model()
{
	if (m_isUseCollision)
	{
		//�����蔻������g�p���Ă�����j������
		MV1TerminateCollInfo(m_modelHandle, -1);
	}
	MV1DeleteModel(m_modelHandle);
}

//�X�V
void Model::Update()
{
	UpdateAnim(m_animPrev);
	UpdateAnim(m_animNext);

	//�w��t���[�������ăA�j���[�V������ύX����
	m_animChangeFrame++;
	if (m_animChangeFrame > m_animChangeFrameTotal)
	{
		m_animChangeFrame = m_animChangeFrameTotal;
	}
	//�A�j���[�V�����̃u�����h����ݒ肷��
	UpdateAnimBlendRate();
	//�R���W�����̍X�V
	UpdateCollision();
}
//�`��
void Model::Draw()
{
	MV1DrawModel(m_modelHandle);
}

//�\���ʒu���擾����
VECTOR Model::GetPos() const
{
	return MV1GetPosition(m_modelHandle);
}
//�\���ʒu�̐ݒ�
void Model::SetPos(VECTOR pos)
{
	MV1SetPosition(m_modelHandle, pos);
}
//��]��Ԃ̐ݒ�
void Model::SetRot(VECTOR rot)
{
	MV1SetRotationXYZ(m_modelHandle, rot);
}
//�g��k���̐ݒ�
void Model::SetScale(VECTOR scale)
{
	MV1SetScale(m_modelHandle, scale);
}

//�A�j���[�V������ݒ肷��(�p�b�g�؂�ւ���)
void Model::SetAnimation(int animNo, bool isLoop, bool isForceChange)
{
	if (!isForceChange)
	{
		//���łɍĐ�����Ă���A�j���͍Đ����Ȃ�
		if (m_animNext.animNo == animNo)	return;
	}

	//�ȑO�̃A�j���[�V�������c���Ă���ΏI��
	if (m_animPrev.attachNo != -1)
	{
		MV1DetachAnim(m_modelHandle, m_animPrev.attachNo);
		ClearAnimData(m_animPrev);
	}
	if (m_animNext.attachNo != -1)
	{
		MV1DetachAnim(m_modelHandle, m_animNext.attachNo);
		ClearAnimData(m_animNext);
	}
	//�V�����A�j���[�V������ݒ�
	m_animNext.animNo = animNo;
	m_animNext.attachNo = MV1AttachAnim(m_modelHandle, animNo, -1, false);
	m_animNext.totalTime = MV1GetAttachAnimTotalTime(m_modelHandle, m_animNext.attachNo);
	m_animNext.isLoop = isLoop;

	m_animChangeFrameTotal = 1;
	m_animChangeFrame = 1;
}

//�A�j���[�V������ω�������(���t���[�������Đ؂�ւ���)
void Model::ChangeAnimstion(int animNo, bool isLoop, bool isForceChange, int changeFrame)
{
	if (!isForceChange)
	{
		//���łɍĐ�����Ă���A�j���͍Đ����Ȃ�
		if (m_animNext.animNo == animNo)	return;
	}

	if (m_animPrev.attachNo != -1)
	{
		//�O��̃A�j���[�V�������ݒ肳��Ă����犮�S�ɏI��������
		MV1DetachAnim(m_modelHandle, m_animPrev.attachNo);
		ClearAnimData(m_animPrev);
	}

	//���ݍĐ����Ă���A�j���[�V�������Â��A�j���[�V�����Ƃ���
	m_animPrev = m_animNext;
	//�V�����A�j���[�V������ݒ�
	m_animNext.animNo = animNo;
	m_animNext.attachNo = MV1AttachAnim(m_modelHandle, animNo, -1, false);
	m_animNext.totalTime = MV1GetAttachAnimTotalTime(m_modelHandle, m_animNext.attachNo);
	m_animNext.isLoop = isLoop;

	//�ύX�ɂ�����t���[�������o���Ă���
	m_animChangeFrameTotal = changeFrame;
	m_animChangeFrame = 0;
	//�A�j���[�V�����̃u�����h����ݒ肷��
	UpdateAnimBlendRate();
}

//���݂̃A�j���[�V�������I�����Ă��邩�ǂ������擾����
bool Model::isAnimEnd()
{
	//Loop�A�j���̏ꍇ�͏��false��Ԃ�
	if (m_animNext.isLoop)	return false;

	float time = MV1GetAttachAnimTime(m_modelHandle, m_animNext.attachNo);
	//���݂̃A�j���[�V�������Ԃ��g�[�^�����Ԃ��������Ƃ�
	if (time >= m_animNext.totalTime)	return true;

	return false;
}

void Model::DeleteModel()
{
	if (m_isUseCollision)
	{
		//�����蔻������g�p���Ă�����j������
		MV1TerminateCollInfo(m_modelHandle, -1);
	}
	MV1DeleteModel(m_modelHandle);
}

void Model::UpdateCollision()
{
	//�R���W�������g�p����ꍇ�A�X�V����
	if (m_isUseCollision)
	{
		MV1SetupCollInfo(m_modelHandle, -1);
	}
}

void Model::ClearAnimData(AnimData& anim)
{
	anim.animNo = -1;
	anim.attachNo = -1;
	anim.totalTime = 0.0f;
	anim.isLoop = false;
}

void Model::UpdateAnim(AnimData anim, float dt)
{
	//�A�j���[�V�������ݒ肳��Ă��Ȃ��ꍇ�͉������Ȃ�
	if (anim.attachNo == -1) return;

	//�A�j���[�V�����̍X�V
	float time = MV1GetAttachAnimTime(m_modelHandle, anim.attachNo);
	time += dt;

	if (time >= anim.totalTime)
	{
		if (anim.isLoop)
		{
			//�A�j���[�V�����̃��[�v
			time -= anim.totalTime;
		}
		else
		{
			//�Ō�̏�ԂŎ~�܂�
			time = anim.totalTime;
		}
	}

	MV1SetAttachAnimTime(m_modelHandle, anim.attachNo, time);

}

void Model::UpdateAnimBlendRate()
{
	//�A�j���[�V�����ω��̃t���[�����ɉ������u�����h����ݒ肷��
	float rate = static_cast<float>(m_animChangeFrame / m_animChangeFrameTotal);

	if (rate > 1.0f)
	{
		rate = 1.0f;
	}

	MV1SetAttachAnimBlendRate(m_modelHandle, m_animPrev.attachNo, 1.0f - rate);
	MV1SetAttachAnimBlendRate(m_modelHandle, m_animNext.attachNo, rate);

}
