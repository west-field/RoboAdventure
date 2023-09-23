#include "TestPlayer.h"
#include "../Model.h"
#include "../InputState.h"
#include <cassert>	
#include "Hp.h"
#include "../Util/Sound.h"

//TODO:やること
//FIXME:直さなきゃいけないこと
//HACK:所感

namespace
{
	// 速度（1=1m、60fps固定として、時速10km）10000m ÷ 時間 ÷ 分 ÷ 秒 ÷ フレーム
	constexpr float kSpeed = static_cast<float>(10000.0 / 60.0 / 60.0 / 60.0);
	
	constexpr float kScale = 0.002f;// スケール
	
	constexpr float kJumpPower = 0.25f;//ジャンプ力
	
	constexpr float kGravity = -0.1f;//重力

	constexpr int kMaxHitColl = 2048;		// 処理するコリジョンポリゴンの最大数
	constexpr float kEnumDefaultSize = 800.0f;		// 周囲のポリゴン検出に使用する球の初期サイズ

	constexpr float kHitWidth = 0.5f;		// 当たり判定カプセルの半径
	constexpr float kHitHeight = 0.9f;		// 当たり判定カプセルの高さ


	constexpr int kSlideTryNum = 16;		// 壁押し出し処理の最大試行回数


	constexpr float kHitSlideLength = kSpeed / 2;		// 一度の壁押し出し処理でスライドさせる距離

}

//コンストラクタ
TestPlayer::TestPlayer():m_animUpdate(&TestPlayer::UpdateIdle),m_colision(true)
{
	m_model = std::make_shared<Model>(L"Data/Model/Robot.mv1",kScale,true);
	
	m_animType = PlayerAnimation::Idle;
	m_model->SetAnimation(static_cast<int>(m_animType), true, true);

#if _DEBUG
	m_colision.center = VGet((0.65f * 2) * 84, 1.0f, 0);
#else
	m_colision.center = VGet(10.0f, 1.0f, 0);
#endif
	m_colision.size = VGet(0.5f, 0.9f, 0.0f);
	m_colision.isCenter = false;

	m_colision.SetLTRB(VGet(-0.3f, 0.9f, 0.0f), VGet(-0.3f, 0.0f, 0.0f),
		VGet(0.3f, 0.9f, 0.0f), VGet(0.3f, 0.0f, 0.0f));

	m_colision.velocity = VGet(0, 0, 0);

	m_hp = std::make_shared<Hp>();
	m_hp->MaxHp(3);

	m_dir = VGet(0, 0, 1);
	m_rot = VGet(0.0f, -95.0f, 0.0f);

	m_model->SetPos(m_colision.GetCenter());
	m_model->SetRot(m_rot);


	m_jumpAdd = kGravity * 2;
	m_isJump = false;

	m_isLeft = false;

	m_flashing = 0;
}
//デストラクタ
TestPlayer::~TestPlayer()
{
	m_model.reset();
}

//更新
void TestPlayer::Update(const InputState& input)
{
	if (m_isDamage && m_flashing == 0)
	{
		m_isDamage = false;
	}
	else if (m_flashing > 0)
	{
		m_flashing--;
	}
	//アニメーションを進める
	m_model->Update();
	
	//アニメーションの再生
	(this->*m_animUpdate)(input);
}

void TestPlayer::Update()
{
	if (m_flashing != 0)
	{
		m_flashing = 0;
	}

	m_model->Update();
}

//描画
void TestPlayer::Draw()
{
	m_hp->Draw();
	if (m_flashing % 10 != 0)	return;
	m_model->Draw();
#ifdef _DEBUG
	m_colision.Draw();
	DrawFormatString(0, 0, 0xffffff, L"%lf,%lf,%lf", m_colision.GetCenter().x, m_colision.GetCenter().y, m_colision.GetCenter().z);
	DrawFormatString(0, 20, 0xffffff, L"%lf,%lf,%lf", m_colision.GetSize().x, m_colision.GetSize().y, m_colision.GetSize().z);
	DrawFormatString(80, 40, 0xffffff, L"%lf,%lf,%lf", m_colision.GetVel().x, m_colision.GetVel().y, m_colision.GetVel().z);
#endif
}

//現在の位置を取得する
const VECTOR TestPlayer::GetPos() const
{
	return m_colision.GetCenter();
}
void TestPlayer::SetPos(VECTOR pos)
{
	m_colision.center = pos;
}
//現在のモデルの位置を取得する
const VECTOR TestPlayer::GetModelPos() const
{
	return m_model->GetPos();
}
//サイズを取得する
const VECTOR TestPlayer::GetSize() const
{
	return m_colision.GetSize();
}
//移動力
const VECTOR& TestPlayer::GetVel() const
{
	return m_colision.GetVel();
}
//移動力を変更
void TestPlayer::SetVel(VECTOR vel)
{
	m_colision.velocity = vel;
	m_colision.center = m_prevPos;
	//元の位置を取得しておく
	m_prevPos = m_colision.center;
	// ポジションを更新.
	m_colision.center = VAdd(m_colision.center, m_colision.velocity);
}

const VECTOR& TestPlayer::GetDir() const
{
	return m_dir;
}

//移動する
void TestPlayer::Move()
{
	// ポジションを更新.
	//m_coll.center = VAdd(m_coll.GetCenter(), m_coll.velocity);
	// 力をかけ終わったベロシティの方向にディレクションを調整.
	if (VSize(m_colision.velocity) != 0)
	{
		m_dir = VNorm(m_colision.velocity);
	}
	//printfDx("%f %f %f\n", dir.x, dir.y, dir.z);
	m_model->SetPos(m_colision.GetCenter());//移動させる
	m_model->SetRot(m_rot);
}
//移動しないとき元の位置に戻す
void TestPlayer::NoMove(int hit)
{
	switch (hit)
	{
	case 0://上下に当たっている
		m_colision.center.y = m_prevPos.y;//移動させずに元の位置に戻す
		break;
	case 1://左右に当たっている
		m_colision.center.x = m_prevPos.x;//移動させずに元の位置に戻す
		break;
	case 2: m_colision.center = m_prevPos;//移動させずに元の位置に戻す
		break;
	default:
		break;
	}
	//ジャンプしていて、ジャンプ力が2.5f以下の時
	if (m_isJump && m_jumpAdd <= 0.0f)
	{
		//初期化
		m_isJump = false;
		m_animUpdate = &TestPlayer::UpdateIdle;
	}
}

//落下する
void TestPlayer::Drop()
{
	if (m_isJump)	return;

	//元の位置を取得しておく
	m_prevPos = m_colision.center;
	// ポジションを更新.
	m_colision.center = VAdd(m_colision.center, VGet(0.0f, kGravity * 2, 0.0f));
	//m_model->SetPos(m_coll.GetCenter());
}

const int TestPlayer::GetHp()const
{
	return m_hp->GetHp();
}

//カメラの範囲内にいるかどうか
bool TestPlayer::OnCamera()
{
	//二つの座標であらわされるボックスがカメラの視界に入っていないかどうかを判定する
	if (CheckCameraViewClip_Box(VAdd(m_colision.center, m_colision.size),
		VSub(m_colision.center, m_colision.size)))//true:視界に入っていない false:視界に入っている
	{
		return false;
	}
	return true;
}

//ブロックに当たった時
void TestPlayer::HitBlockUp()
{
	//ジャンプしている
	if (m_isJump)
	{
		//初期化
		m_jumpAdd = kGravity;
		m_isJump = false;
		m_animUpdate = &TestPlayer::UpdateIdle;
	}
}

//ブロックに当たった時
void TestPlayer::HitBlockWidth()
{
	//横に移動しないように
	m_dir = VAdd(m_dir, VGet(0, 0, 0));
	// ゼロ除算避け
	if (VSquareSize(m_dir) > 0)
	{
		// 正規化
		m_dir = VNorm(m_dir);
	}
	m_colision.velocity = VScale(m_dir, kSpeed);
}

void TestPlayer::OnDamage()
{
	if (m_isDamage)	return;
	m_isDamage = true;
	//プレイヤーのHPが減る
	m_hp->Damage(1);
	//プレイヤーが点滅する
	m_flashing = 10 * 4 + 1;
}
bool TestPlayer::IsDamage()
{
	return m_isDamage;
}

const float TestPlayer::GetSpeed()const
{
	return kSpeed;
}

void TestPlayer::Attack()
{
	//ジャンプアニメーションに変更
	SwitchAnimation(PlayerAnimation::WalkJump, true, false, 4);
	//ジャンプの更新に変更
	m_animUpdate = &TestPlayer::UpdateJump;
	m_jumpAdd = kJumpPower / 2;
	m_isJump = true;
}

void TestPlayer::SwitchAnimation(PlayerAnimation animNo, bool isLoop, bool isForceChange, int changeFrame)
{
	int number = static_cast<int>(animNo);
	m_model->ChangeAnimstion(number, isLoop, isForceChange, changeFrame);
}

//------------------------------private----------------------------------

//左に移動
void TestPlayer::MoveLeft()
{
	m_dir = VAdd(m_dir, VGet(-1, 0, 0));
	// ゼロ除算避け
	if (VSquareSize(m_dir) > 0)
	{
		// 正規化
		m_dir = VNorm(m_dir);
	}
	m_rot = VGet(0.0f, 95.0f, 0.0f);
	m_isLeft = true;
	m_colision.velocity = VScale(m_dir, kSpeed);
}
//右に移動
void TestPlayer::MoveRight()
{
	m_dir = VAdd(m_dir, VGet(1, 0, 0));
	// ゼロ除算避け
	if (VSquareSize(m_dir) > 0)
	{
		// 正規化
		m_dir = VNorm(m_dir);
	}
	m_rot = VGet(0.0f, -95.0f, 0.0f);
	m_isLeft = false;
	m_colision.velocity = VScale(m_dir, kSpeed);
}

//待機アニメーションアップデート
void TestPlayer::UpdateIdle(const InputState& input)
{
	bool isPrees = false;//キーを押しているか
	// 単純に方向転換
	m_dir = VGet(0, 0, 0);
	m_colision.velocity = VScale(m_dir, kSpeed);

	if (input.IsPressed(InputType::up))
	{
		m_dir = VAdd(m_dir, VGet(0, 1, 0));
		isPrees = true;
	}
	else if (input.IsPressed(InputType::down))
	{
		m_dir = VAdd(m_dir, VGet(0, -1, 0));
		isPrees = true;
	}
	if (input.IsPressed(InputType::right))
	{
		MoveRight();
		isPrees = true;
	}
	else if (input.IsPressed(InputType::left))
	{
		MoveLeft();
		isPrees = true;
	}

	//キーを押している時
	if (isPrees)
	{
		//歩きアニメーションに変更
		SwitchAnimation(PlayerAnimation::Walk, true, false, 4);
	}
	else
	{
		//押していないとき待機アニメーションに変更する
		SwitchAnimation(PlayerAnimation::Idle, true, false, 4);
	}

	//ジャンプボタンを押したら
	if (!m_isJump && input.IsTriggered(InputType::junp))
	{
		//ジャンプ音を再生
		SoundManager::GetInstance().Play(SoundId::PlayerJump);
		//ジャンプアニメーションに変更
		SwitchAnimation(PlayerAnimation::WalkJump, true, false, 4);
		//ジャンプの更新に変更
		m_animUpdate = &TestPlayer::UpdateJump;
		m_jumpAdd = kJumpPower;
		m_isJump = true;
	}

	//元の位置を取得しておく
	m_prevPos = m_colision.center;
	m_colision.center = VAdd(m_colision.GetCenter(), m_colision.velocity);
}
//ジャンプアップデート
void TestPlayer::UpdateJump(const InputState& input)
{
	m_jumpAdd -= 0.01f;//ジャンプ

	//どれかが0でないとき
	if (m_dir.x != 0.0f || m_dir.y != 0.0f || m_dir.z != 0.0f)
	{
		//左右に移動
		if (input.IsPressed(InputType::right))
		{
			MoveRight();
		}
		else if (input.IsPressed(InputType::left))
		{
			MoveLeft();
		}
	}
	
	//元の位置を取得しておく
	m_prevPos = m_colision.center;
	//位置を変更
	m_colision.velocity = VGet(kSpeed * m_dir.x, m_jumpAdd, 0.0f);
	m_colision.center = VAdd(m_colision.GetCenter(), m_colision.velocity);
	//m_model->SetPos(m_coll.GetCenter());
}

