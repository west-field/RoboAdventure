#pragma once
#include "DxLib.h"

class Player;

class Camera
{
public:
	Camera();//コンストラクタ
	virtual ~Camera();//デストラクタ

	//カメラの位置を初期化
	void CameraPosInit();
	void CameraPosInit(const VECTOR pos,bool isCameraPos);
	//更新
	void Update(const VECTOR pos);

	//カメラを移動させる
	void moveUpdate(const VECTOR move);

	// ポジションを取得する
	const VECTOR& GetPos() const { return m_pos; }

	//ターゲットを取得
	const VECTOR& GetTarget()const { return m_target; }

	const bool GetMove() const { return m_isMove; }

private:
	VECTOR	m_pos;//ポジション
	VECTOR m_target;//ターゲット

	float m_posX;//前回のポジションX

	bool m_isMove;//動ける
};

