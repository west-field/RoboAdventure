#pragma once
#include "DxLib.h"

class Player;

class Camera
{
public:
	Camera();//�R���X�g���N�^
	virtual ~Camera();//�f�X�g���N�^

	//�J�����̈ʒu��������
	void CameraPosInit();
	void CameraPosInit(const VECTOR pos,bool isCameraPos);
	//�X�V
	void Update(const VECTOR pos);

	//�J�������ړ�������
	void moveUpdate(const VECTOR move);

	// �|�W�V�������擾����
	const VECTOR& GetPos() const { return m_pos; }

	//�^�[�Q�b�g���擾
	const VECTOR& GetTarget()const { return m_target; }

	const bool GetMove() const { return m_isMove; }

private:
	VECTOR	m_pos;//�|�W�V����
	VECTOR m_target;//�^�[�Q�b�g

	float m_posX;//�O��̃|�W�V����X

	bool m_isMove;//������
};

