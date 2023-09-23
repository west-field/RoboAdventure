#pragma once
#include "DxLib.h"

class Model
{
public:
	/// <summary>
	/// �R���X�g���N�^
	/// �t�@�C�������w�肵�ă��[�h���s��
	/// </summary>
	/// <param name="fileName">���f���t�@�C����</param>
	/// <param name="scale">�傫�����w��</param>
	/// 	/// <param name="useCollision">�R���W���������悤���邩�ǂ���</param>
	Model(const TCHAR* fileName, float scale, bool useCollision);
	/// <summary>
	/// �R���X�g���N�^
	/// �w�肳�ꂽ�n���h���̃��f����Duplicate���Đ�������
	/// </summary>
	/// <param name="orgModel">���f���n���h��</param>
	/// <param name="scale">�傫�����w��</param>
	/// <param name="useCollision">�R���W���������悤���邩�ǂ���</param>
	Model(int orgModel, float scale, bool useCollision);
	//�f�X�g���N�^
	virtual ~Model();

	//�X�V
	void Update();
	//�`��
	void Draw();

	/// <summary>
	/// ���f���n���h���̎擾
	/// </summary>
	/// <returns>���f���n���h��</returns>
	int GetModelHandle() const { return m_modelHandle; }

	//�\���ʒu���擾����
	VECTOR GetPos() const;
	//�\���ʒu�̐ݒ�
	void SetPos(VECTOR pos);
	//��]��Ԃ̐ݒ�
	void SetRot(VECTOR rot);
	//�g��k���̐ݒ�
	void SetScale(VECTOR scale);

	/// <summary>
	/// �A�j���[�V������ݒ肷��(�ς��Ɛ؂�ւ���)
	/// </summary>
	/// <param name="animNo">�ύX��A�j���[�V�����ԍ�</param>
	/// <param name="isLoop">�A�j���[�V���������[�v�����邩</param>
	/// <param name="isForceChange">���łɎw�肳�ꂽ�A�j�����Đ�����Ă���ꍇ���ύX���邩</param>
	void SetAnimation(int animNo, bool isLoop, bool isForceChange);

	/// <summary>
	/// �A�j���[�V������ω�������(���t���[�������Đ؂�ւ���)
	/// </summary>
	/// <param name="animNo">�ύX��A�j���[�V����</param>
	/// <param name="isLoop">�A�j���[�V���������[�v�����邩</param>
	/// <param name="isForceChange">���łɎw�肳�ꂽ�A�j�����Đ�����Ă���ꍇ���ύX���邩</param>
	/// <param name="changeFrame">�t���[����</param>
	void ChangeAnimstion(int animNo, bool isLoop, bool isForceChange, int changeFrame);

	/// <summary>
	/// ���݂̃A�j���[�V�������I�����Ă��邩�ǂ������擾����
	/// Loop�A�j���̏ꍇ�擾�ł��Ȃ�
	/// </summary>
	/// <returns>true:�I�����Ă��� false:�I�����Ă��Ȃ�</returns>
	bool isAnimEnd();

	/// <summary>
	/// ���݂��邩�ǂ������擾����
	/// </summary>
	/// <returns>true:���݂��� false:���݂��Ȃ�</returns>
	bool IsExist()const { return m_isExist; }
	/// <summary>
	/// ���݂��邩�ǂ�����ݒ肷��
	/// </summary>
	/// <param name="isExist">true:���݂��� false:���݂��Ȃ�</param>
	void SetExist(bool isExist) { m_isExist = isExist; }

	/// <summary>
	/// ���f�����폜����
	/// </summary>
	void DeleteModel();

	//�R���W�����̍X�V
	void UpdateCollision();
private:
	//�A�j���[�V�������
	struct AnimData
	{
		int animNo;			//�A�j���[�V�����ԍ�
		int attachNo;		//�A�^�b�`�ԍ�
		float totalTime;	//�A�j���[�V�����̑��Đ�������
		bool isLoop;		//�A�j���[�V���������[�v���邩
	};

private:
	/// <summary>
	/// �A�j���[�V�����f�[�^�̃N���A
	/// </summary>
	/// <param name="anim">�A�j���[�V�����f�[�^</param>
	void ClearAnimData(AnimData& anim);

	/// <summary>
	/// �A�j���[�V�����̍X�V
	/// </summary>
	/// <param name="anim">�A�j���[�V�����f�[�^</param>
	/// <param name="dt">�o�ߎ���</param>
	void UpdateAnim(AnimData anim, float dt = 1.0f);

	/// <summary>
	/// ���݂̃A�j���[�V�����؂�ւ���񂩂�A�j���[�V�����̃u�����h����ݒ肷��
	/// </summary>
	void UpdateAnimBlendRate();

private:
	//���f���n���h��
	int m_modelHandle;

	//�����蔻������g�p����
	bool m_isUseCollision;
	//�����蔻����𖈃t���[���X�V����
	bool m_isUpdateVollision;

	//�A�j���[�V�����̃A�^�b�`�ԍ�
	AnimData m_animPrev;//�ύX�O�A�j���[�V�������
	AnimData m_animNext;//�ύX��A�j���[�V�������

	//�A�j���[�V�����̐؂�ւ����
	int m_animChangeFrame;		//���݂̐؂�ւ��t���[����
	int m_animChangeFrameTotal;	//�؂�ւ��ɂ����鑍�t���[����

	bool m_isExist;//���݂��邩
};

