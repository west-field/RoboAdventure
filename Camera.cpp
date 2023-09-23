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
	VECTOR lookPos = VGet(m_pos.x, m_pos.y, m_pos.z + 1.0f);//カメラの注視点
	//カメラの位置を初期化
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
		m_target = VGet(m_pos.x, m_pos.y, m_pos.z + 1.0f);//カメラの注視点
		SetCameraPositionAndTarget_UpVecY(m_pos, m_target);
	}
}

void Camera::Update(const VECTOR pos)
{
	// プレイヤーのX座標には追従したいのでplayerのXを使う
	// 注視する視点は、カメラと平行にまっすぐz=0地点
	VECTOR playerPos = pos;
	playerPos.y = 0.0f;
	VECTOR cameraPos = VGet(playerPos.x, playerPos.y + 3.0f, playerPos.z - 10.0f);//カメラの位置(斜め)
	m_target= VGet(cameraPos.x, cameraPos.y, cameraPos.z + 1.0f);//カメラの注視点

	m_pos = cameraPos;
	m_isMove = false;
	//今回のX位置が前回のX位置よりも大きかったら
	if (m_pos.x > m_posX)
	{
		m_isMove = true;
		//今回の位置を持つ
		m_posX = m_pos.x;
		// カメラに位置を反映.
		SetCameraPositionAndTarget_UpVecY(m_pos, m_target);
	}
	SetLightPosition(VAdd(m_pos,VGet(0.0f,200.0f,-100.0f)));
}

void Camera::moveUpdate(const VECTOR move)
{
	//VECTOR cameraPos = VGet(m_pos.x, m_pos.y, m_pos.z);//カメラの位置(斜め)
	m_pos = VAdd(m_pos, move);
	VECTOR lookPos = VGet(m_pos.x, m_pos.y, m_pos.z + 1.0f);//カメラの注視点

	SetCameraPositionAndTarget_UpVecY(m_pos, lookPos);
}
