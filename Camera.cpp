#include "Camera.h"

Camera::Camera()
{
	m_pos = VGet(0, 0, 0);
	m_posX = 0.0f;
	m_isMove = false;
}

Camera::~Camera()
{
}

void Camera::CameraPosInit()
{
	m_pos = VGet(10.4f, 3.0f, -10.0f);
	VECTOR lookPos = VGet(m_pos.x, m_pos.y, m_pos.z + 1.0f);//�J�����̒����_
	//�J�����̈ʒu��������
	SetCameraPositionAndTarget_UpVecY(m_pos, lookPos);
}

void Camera::CameraPosInit(const VECTOR pos, bool isCameraPos)
{
	if (!isCameraPos)
	{
		Update(pos);
	}
	else
	{
		m_pos = pos;
		m_target = VGet(m_pos.x, m_pos.y, m_pos.z + 1.0f);//�J�����̒����_
		SetCameraPositionAndTarget_UpVecY(m_pos, m_target);
	}
}

void Camera::Update(const VECTOR pos)
{
	// �v���C���[��X���W�ɂ͒Ǐ]�������̂�player��X���g��
	// �������鎋�_�́A�J�����ƕ��s�ɂ܂�����z=0�n�_
	VECTOR playerPos = pos;
	playerPos.y = 0.0f;
	VECTOR cameraPos = VGet(playerPos.x, playerPos.y + 3.0f, playerPos.z - 10.0f);//�J�����̈ʒu(�΂�)
	m_target= VGet(cameraPos.x, cameraPos.y, cameraPos.z + 1.0f);//�J�����̒����_

	m_pos = cameraPos;
	m_isMove = false;
	//�����X�ʒu���O���X�ʒu�����傫��������
	if (m_pos.x > m_posX)
	{
		m_isMove = true;
		//����̈ʒu������
		m_posX = m_pos.x;
		// �J�����Ɉʒu�𔽉f.
		SetCameraPositionAndTarget_UpVecY(m_pos, m_target);
	}
	SetLightPosition(VAdd(m_pos,VGet(0.0f,200.0f,-100.0f)));
}

void Camera::moveUpdate(const VECTOR move)
{
	//VECTOR cameraPos = VGet(m_pos.x, m_pos.y, m_pos.z);//�J�����̈ʒu(�΂�)
	m_pos = VAdd(m_pos, move);
	VECTOR lookPos = VGet(m_pos.x, m_pos.y, m_pos.z + 1.0f);//�J�����̒����_

	SetCameraPositionAndTarget_UpVecY(m_pos, lookPos);
}
