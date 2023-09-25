#include "Player.h"
#include "../Model.h"
#include "../InputState.h"
#include <cassert>
#include "../Game/Map.h"
#include "../Game/Hp.h"
#include "../Util/Sound.h"

namespace
{
	constexpr float kMoveSpeed = static_cast<float>(10000.0 / 60.0 / 60.0 / 60.0);//移動スピード
	constexpr float kJumpSpeed = 1.0f;//ジャンプ力
	constexpr float kGravity = 0.1f;//重力

	constexpr int kSlideTryNum = 16;//位置の補正をする回数
	constexpr float kSlideMoveSpeed = 0.1f;//位置を補正する際に一回で移動できる距離

	constexpr int kMaxHitColl = 1000;// 処理するコリジョンポリゴンの最大数

	constexpr float kCapsuleRadius = 0.25f;		//カプセルの半径

	constexpr float kHitCapsuleTop = 0.6f;		//カプセルの上の位置
	constexpr float kHitCapsuleBottom = 0.3f;	//カプセルの下の位置

	constexpr float kHitLineTop = 0.9f;		//線の上の位置
	constexpr float kHitLineBottom = -0.1f;	//線の下の位置

	constexpr float kScale = 0.002f;//モデルの大きさ

	constexpr float kSizeX = 0.3f;//プレイヤーのサイズX
	constexpr float kSizeY = 0.9f;//プレイヤーのサイズY

	constexpr int kMaxHp = 3;//最大HP
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

	m_animType = PlayerAnimation::Idle;//最初は立ち止まっているアニメーション
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
	//m_model->DeleteModel();//ここで消すと
}

void Player::Update(const InputState& input)
{
	//攻撃を受けている&&点滅時間が0より小さい時
	if (m_isDamage && m_flashing <= 0)
	{
		m_isDamage = false;//攻撃を受けていないようにする
	}
	//点滅時間が0よりも大きいとき
	else if(m_flashing > 0)
	{
		m_flashing--;//減らす
	}

	(this->*m_update)(input);

	//当たり判定
	Move();
	
	//実際にモデルの位置、回転を変更
	m_model->SetPos(m_pos);
	m_model->SetRot(m_rot);
	m_model->Update();
}

void Player::Update()
{
	//点滅時間が0出なかったとき
	if (m_flashing != 0)
	{
		m_flashing = 0;//点滅させないように0にする
	}

	m_model->Update();
}

void Player::Draw()
{
	//HPを表示
	m_hp->Draw();
	//攻撃を受けたときは点滅させる
	if (m_flashing % 10 != 0)	return;
	//モデル表示
	m_model->Draw();
#if _DEBUG
	DrawFormatString(0, 20, 0xffffff, L"pos.x%lf,y%lf,z%lf", m_pos.x, m_pos.y, m_pos.z);//モデル表示位置
	DrawFormatString(0, 40, 0xffffff, L"jump%lf", m_jumpPower);//ジャンプの高さを変更

	DrawSphere3D(VAdd(m_pos, VGet(0.0f, kSizeY / 3, 0.0f)), kCapsuleRadius + VSize(m_dir), 10, 0xffffff, 0xffffff, false);//スフィア
	DrawCapsule3D(VAdd(m_pos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(m_pos, VGet(0.0f, kHitCapsuleTop, 0.0f)),
		kCapsuleRadius, 10, 0xffffff, 0xffffff, false);//カプセル

	DrawLine3D(VAdd(m_pos, VGet(0.0f, (kHitCapsuleTop + 0.5f), 0.0f)), VAdd(m_pos, VGet(0.0f, kHitLineBottom - 0.2f, 0.0f)), 0xff0000);//線

	//今再生しているアニメーションを文字で表示
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

	//HitNumが0以外の時
	if (dim.HitNum != 0)
	{
		MV1CollResultPolyDimTerminate(dim);
		return true;//当たっている
	}

	MV1CollResultPolyDimTerminate(dim);
	return false;//当たっていない
}

void Player::OnDamage(int attack)
{
	//すでに攻撃を受けている場合は処理をしない
	if (m_isDamage)	return;
	m_isDamage = true;
	//プレイヤーが敵に当たった音
	SoundManager::GetInstance().Play(SoundId::PlayeyHit);
	//プレイヤーのHPが減る
	m_hp->Damage(attack);
	//プレイヤーが点滅する
	m_flashing = 10 * 4 + 1;
}

const int Player::GetHp() const
{
	return m_hp->GetHp();
}

void Player::Attack()
{
	//上に上がる
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
	//アニメーションがジャンプしているとき
	if (m_animType == PlayerAnimation::WalkJump)
	{
		return true;//ジャンプしている
	}
	return false;//ジャンプしていない
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
	//キャラクターの位置が1.0fよりも小さいとき && 二つの座標であらわされるボックスがカメラの視界に入っていないかどうかを判定する
	if (m_pos.y <= 1.0f && CheckCameraViewClip_Box(VAdd(m_pos, VGet(kSizeX, kSizeY, kSizeX)),
		VAdd(m_pos, VGet(-kSizeX, 0.0f, -kSizeX))))//true:視界に入っていない false:視界に入っている
	{
		return false;
	}
	return true;
}

void Player::NormalUpdate(const InputState& input)
{
	m_dir = VGet(0.0f, 0.0f, 0.0f);//初期化
	m_vel = VGet(0.0f, 0.0f, 0.0f);//初期化
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

		//キーを押している時
		if (isPrees)
		{
			//歩きアニメーションに変更
			m_animType = PlayerAnimation::Walk;
			SwitchAnimation(PlayerAnimation::Walk, true, false, 4);
		}
		else
		{
			//押していないとき待機アニメーションに変更する
			m_animType = PlayerAnimation::Idle;
			SwitchAnimation(PlayerAnimation::Idle, true, false, 4);
		}
		
		//ジャンプ状態ではなく、ジャンプボタンが押されたら
		if (input.IsTriggered(InputType::junp))
		{
			//ジャンプ音
			SoundManager::GetInstance().Play(SoundId::PlayerJump);
			//ジャンプ状態にする
			m_animType = PlayerAnimation::WalkJump;
			//アニメーションの変更
			SwitchAnimation(m_animType, true, false, 4);
			//Y軸方向の速度をセット
			m_jumpPower = kJumpSpeed;
			m_vel.y = m_jumpPower;
			//ジャンプのアップデートへ移動
			m_update = &Player::JumpUpdate;
		}
	}

	// ゼロ除算避け
	if (VSquareSize(m_dir) > 0)
	{
		// 正規化
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

	//状態がジャンプの場合は
	if (m_animType == PlayerAnimation::WalkJump && m_jumpPower > 0.0f)
	{
		//重力分減少
		m_jumpPower -= kGravity;

		//移動ベクトルのY成分に代入する
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
	bool isMoveFlag;						// 水平方向に移動したかどうかのフラグ( ０:移動していない  １:移動した )
	bool isHitFlag;						// ポリゴンに当たったかどうかを記憶しておくのに使う変数( ０:当たっていない  １:当たった )
	MV1_COLL_RESULT_POLY_DIM HitDim;			// プレイヤーの周囲にあるポリゴンを検出した結果が代入される当たり判定結果構造体
	int wallCollisionNum;						// 壁ポリゴンと判断されたポリゴンの数
	int floorCollisionNum;						// 床ポリゴンと判断されたポリゴンの数
	MV1_COLL_RESULT_POLY* wallCol[kMaxHitColl];	// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* floorCol[kMaxHitColl];	// 床ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	HITRESULT_LINE LineRes;				// 線分とポリゴンとの当たり判定の結果を代入する構造体
	VECTOR oldPos;						// 移動前の座標	
	VECTOR nowPos;						// 移動後の座標

	// 移動前の座標を保存
	oldPos = m_pos;

	// 移動後の座標を算出
	nowPos = VAdd(m_pos, m_vel);
	//m_mapから当たり判定用のVECTORを取得する
	for (auto& model : m_map->GetPiece())
	{
		if (!model.model->IsExist())	continue;
		// プレイヤーの周囲にあるステージポリゴンを取得する
		// ( 検出する範囲は移動距離も考慮する )
		HitDim = MV1CollCheck_Sphere(model.model->GetModelHandle(), -1, VAdd(m_pos, VGet(0.0f, kSizeY / 3, 0.0f)), kCapsuleRadius + VSize(m_dir));

		//HitNumが0の時(どこにも当たっていない時)処理をしない
		if (HitDim.HitNum == 0)	continue;

		// x軸かz軸方向に 0.01f 以上移動した場合は「移動した」フラグを１にする
		if (fabs(m_vel.x) > 0.01f || fabs(m_vel.z) > 0.01f)
		{
			isMoveFlag = true;
		}
		else
		{
			isMoveFlag = false;
		}

		// 検出されたポリゴンが壁ポリゴン( ＸＺ平面に垂直なポリゴン )か床ポリゴン( ＸＺ平面に垂直ではないポリゴン )かを判断する
		{
			// 壁ポリゴンと床ポリゴンの数を初期化する
			wallCollisionNum = 0;
			floorCollisionNum = 0;

			// 検出されたポリゴンの数だけ繰り返し
			for (int i = 0; i < HitDim.HitNum; i++)
			{
				// ＸＺ平面に垂直かどうかはポリゴンの法線のＹ成分が０に限りなく近いかどうかで判断する
				if (HitDim.Dim[i].Normal.y < 0.000001f && HitDim.Dim[i].Normal.y > -0.000001f)
				{
					// 壁ポリゴンと判断された場合でも、プレイヤーのＹ座標＋1.0fより高いポリゴンのみ当たり判定を行う
					if (HitDim.Dim[i].Position[0].y > m_pos.y + 1.0f ||
						HitDim.Dim[i].Position[1].y > m_pos.y + 1.0f ||
						HitDim.Dim[i].Position[2].y > m_pos.y + 1.0f)
					{
						// ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
						if (wallCollisionNum < kMaxHitColl)
						{
							// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
							wallCol[wallCollisionNum] = &HitDim.Dim[i];

							// 壁ポリゴンの数を加算する
							wallCollisionNum++;
						}
					}
				}
				else
				{
					// ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
					if (floorCollisionNum < kMaxHitColl)
					{
						// ポリゴンの構造体のアドレスを床ポリゴンポインタ配列に保存する
						floorCol[floorCollisionNum] = &HitDim.Dim[i];

						// 床ポリゴンの数を加算する
						floorCollisionNum++;
					}
				}
			}
		}

		// 壁ポリゴンとの当たり判定処理
		if (wallCollisionNum != 0)
		{
			// 壁に当たったかどうかのフラグは初期状態では「当たっていない」にしておく
			isHitFlag = false;

			// 移動したかどうかで処理を分岐
			if (isMoveFlag)
			{
				// 壁ポリゴンの数だけ繰り返し
				for (int i = 0; i < wallCollisionNum; i++)
				{
					// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
					if (HitCheck_Capsule_Triangle(VAdd(nowPos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitCapsuleTop, 0.0f)),
													kCapsuleRadius, wallCol[i]->Position[0], wallCol[i]->Position[1], wallCol[i]->Position[2]) == FALSE) continue;

					// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
					isHitFlag = true;

					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
					nowPos = VAdd(nowPos, VScale(VGet(wallCol[i]->Normal.x, 0.0f, 0.0f), kSlideMoveSpeed));
					nowPos.z = 0.0f;

					int j = 0;
					// 新たな移動座標で壁ポリゴンと当たっていないかどうかを判定する
					for (j = 0; j < wallCollisionNum; j++)
					{
						// 当たっていたらループから抜ける
						if (HitCheck_Capsule_Triangle(VAdd(nowPos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitCapsuleTop, 0.0f)),
							kCapsuleRadius, wallCol[j]->Position[0], wallCol[j]->Position[1], wallCol[j]->Position[2]) == TRUE) break;
					}

					// j が KabeNum だった場合はどのポリゴンとも当たらなかったということなので
					// 壁に当たったフラグを倒した上でループから抜ける
					if (j == wallCollisionNum)
					{
						isHitFlag = false;
						break;
					}
				}
			}
			else
			{
				// 移動していない場合の処理

				// 壁ポリゴンの数だけ繰り返し
				for (int i = 0; i < wallCollisionNum; i++)
				{
					// ポリゴンに当たっていたら当たったフラグを立てた上でループから抜ける
					if (HitCheck_Capsule_Triangle(VAdd(nowPos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitCapsuleTop, 0.0f)), kCapsuleRadius, wallCol[i]->Position[0], wallCol[i]->Position[1], wallCol[i]->Position[2]) == TRUE)
					{
						isHitFlag = true;
						break;
					}
				}
			}

			// 壁に当たっていたら壁から押し出す処理を行う
			if (isHitFlag)
			{
				// 壁からの押し出し処理を試みる最大数だけ繰り返し
				for (int k = 0; k < kSlideTryNum; k++)
				{
					int i = 0;
					// 壁ポリゴンの数だけ繰り返し
					for (i = 0; i < wallCollisionNum; i++)
					{
						// プレイヤーと当たっているかを判定
						if (HitCheck_Capsule_Triangle(VAdd(nowPos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitCapsuleTop, 0.0f)), kCapsuleRadius, wallCol[i]->Position[0], wallCol[i]->Position[1], wallCol[i]->Position[2]) == FALSE) continue;

						// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
						nowPos = VAdd(nowPos, VScale(wallCol[i]->Normal, kSlideMoveSpeed));
						nowPos.z = 0.0f;

						int j = 0;
						// 移動した上で壁ポリゴンと接触しているかどうかを判定
						for (j = 0; j < wallCollisionNum; j++)
						{
							if (HitCheck_Capsule_Triangle(VAdd(nowPos, VGet(0.0f, kHitCapsuleBottom, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitCapsuleTop, 0.0f)), kCapsuleRadius, wallCol[j]->Position[0], wallCol[j]->Position[1], wallCol[j]->Position[2]) == TRUE) break;
						}

						// 全てのポリゴンと当たっていなかったらここでループ終了
						if (j == wallCollisionNum) break;
					}

					// i が KabeNum ではない場合は全部のポリゴンで押し出しを試みる前に全ての壁ポリゴンと接触しなくなったということなのでループから抜ける
					if (i != wallCollisionNum) break;
				}
			}
		}

		// 床ポリゴンとの当たり判定
		if (floorCollisionNum != 0)
		{
			// ジャンプ中且つ上昇中の場合は処理を分岐
			if (m_animType == PlayerAnimation::WalkJump && m_jumpPower > 0.0f)
			{
				float MinY;

				// 天井に頭をぶつける処理を行う

				// 一番低い天井にぶつける為の判定用変数を初期化
				MinY = 0.0f;

				// 当たったかどうかのフラグを当たっていないを意味する０にしておく
				isHitFlag = false;

				// 床ポリゴンの数だけ繰り返し
				for (int i = 0; i < floorCollisionNum; i++)
				{
					// 足先から頭の高さまでの間でポリゴンと接触しているかどうかを判定
					LineRes = HitCheck_Line_Triangle(VAdd(nowPos, VGet(0.0f, kHitLineTop + 0.5f, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitLineBottom+1.0f, 0.0f)),
						floorCol[i]->Position[0], floorCol[i]->Position[1], floorCol[i]->Position[2]);

					// 接触していなかったら何もしない
					if (LineRes.HitFlag == FALSE) continue;

					// 既にポリゴンに当たっていて、且つ今まで検出した天井ポリゴンより高い場合は何もしない
					if (isHitFlag && MinY < LineRes.Position.y) continue;

					// ポリゴンに当たったフラグを立てる
					isHitFlag = true;

					// 接触したＹ座標を保存する
					MinY = LineRes.Position.y;
				}

				// 接触したポリゴンがあったかどうかで処理を分岐
				if (isHitFlag)
				{
					// 接触した場合はプレイヤーのＹ座標を接触座標を元に更新
					nowPos.y = MinY - (kHitCapsuleTop + 0.5f);

					// Ｙ軸方向の速度は反転
					m_jumpPower = -m_jumpPower;
				}
			}
			else
			{
				float MaxY;

				// 下降中かジャンプ中ではない場合の処理

				// 床ポリゴンに当たったかどうかのフラグを倒しておく
				isHitFlag = false;

				// 一番高い床ポリゴンにぶつける為の判定用変数を初期化
				MaxY = 0.0f;

				// 床ポリゴンの数だけ繰り返し
				for (int i = 0; i < floorCollisionNum; i++)
				{
					// ジャンプ中かどうかで処理を分岐
					if (m_animType == PlayerAnimation::WalkJump)
					{
						// ジャンプ中の場合は頭の先から足先より少し低い位置の間で当たっているかを判定
						LineRes = HitCheck_Line_Triangle(VAdd(nowPos, VGet(0.0f, kHitLineTop, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitLineBottom, 0.0f)), 
							floorCol[i]->Position[0], floorCol[i]->Position[1], floorCol[i]->Position[2]);
					}
					else
					{
						// 走っている場合は頭の先からそこそこ低い位置の間で当たっているかを判定( 傾斜で落下状態に移行してしまわない為 )
						LineRes = HitCheck_Line_Triangle(VAdd(nowPos, VGet(0.0f, kHitLineTop, 0.0f)), VAdd(nowPos, VGet(0.0f, kHitLineBottom - 0.2f, 0.0f)),
							floorCol[i]->Position[0], floorCol[i]->Position[1], floorCol[i]->Position[2]);
					}

					// 当たっていなかったら何もしない
					if (LineRes.HitFlag == FALSE) continue;

					// 既に当たったポリゴンがあり、且つ今まで検出した床ポリゴンより低い場合は何もしない
					if (isHitFlag && MaxY > LineRes.Position.y) continue;

					// ポリゴンに当たったフラグを立てる
					isHitFlag = true;

					// 接触したＹ座標を保存する
					MaxY = LineRes.Position.y;

					if (MaxY < -0.38f)
					{
						MaxY = -0.38f;
					}
				}

				// 床ポリゴンに当たったかどうかで処理を分岐
				if (isHitFlag)
				{
					// 当たった場合
					m_isAttack = false;

					// 接触したポリゴンで一番高いＹ座標をプレイヤーのＹ座標にする
					nowPos.y = MaxY;

					// Ｙ軸方向の移動速度は０に
					m_jumpPower = 0.0f;

					// もしジャンプ中だった場合は着地状態にする
					if (m_animType == PlayerAnimation::WalkJump)
					{
						// 移動していたかどうかで着地後の状態と再生するアニメーションを分岐する
						if (isMoveFlag)
						{
							// 移動している場合は走り状態に
							m_animType = PlayerAnimation::Run;
							SwitchAnimation(m_animType, true, false, 4);
						}
						else
						{
							// 移動していない場合は立ち止り状態に
							m_animType = PlayerAnimation::Idle;
							SwitchAnimation(m_animType, true, false, 4);
						}
					}
				}
				//else
				//{
				//	// 床コリジョンに当たっていなくて且つジャンプ状態ではなかった場合は
				//	if (m_animType != PlayerAnimation::WalkJump)
				//	{
				//		// ジャンプ中にする
				//		m_animType = PlayerAnimation::WalkJump;

				//		// ちょっとだけジャンプする
				//		//m_jumpPower = kJumpSpeed / 2;

				//		// アニメーションは落下中のものにする
				//		SwitchAnimation(m_animType,true, false, 4);
				//	}
				//}
			}
		}


	}

	// 新しい座標を保存する
	m_pos = nowPos;
}

