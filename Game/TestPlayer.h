#pragma once
#include "DxLib.h"
#include <memory>
#include "../CollisionInfo.h"

class Model;
class InputState;
class Hp;

/// <summary>
/// プレイヤーアニメーション
/// </summary>
enum class PlayerAnimation
{
	Dance = 0,
	Death = 1,
	Idle = 2,
	Jump = 3,
	No = 4,
	Punch = 5,
	Run = 6,
	ThumbsUp = 9,
	Walk = 10,
	WalkJump = 11,
	Wave = 12,
	Yes = 13,
};

/// <summary>
/// プレイヤー
/// </summary>
class TestPlayer
{
public:
	TestPlayer();//コンストラクタ
	virtual ~TestPlayer();//デストラクタ

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="input">入力</param>
	void Update(const InputState& input);
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 現在の位置を取得する
	/// </summary>
	/// <returns>現在位置</returns>
	const VECTOR GetPos() const;
	/// <summary>
	/// 位置を設定する
	/// </summary>
	void SetPos(VECTOR pos);
	/// <summary>
	/// 現在のモデルの位置
	/// </summary>
	/// <returns>モデルの位置</returns>
	const VECTOR GetModelPos()const;
	/// <summary>
	/// サイズを取得する
	/// </summary>
	/// <returns>サイズ</returns>
	const VECTOR GetSize() const;
	/// <summary>
	/// 移動力
	/// </summary>
	/// <returns>移動</returns>
	const VECTOR& GetVel() const;
	/// <summary>
	/// 移動力を変更
	/// </summary>
	/// <param name="vel">移動</param>
	void SetVel(VECTOR vel);
	/// <summary>
	/// 移動力
	/// </summary>
	/// <returns>移動</returns>
	const VECTOR& GetDir() const;

	/// <summary>
	/// 移動する
	/// </summary>
	void Move();
	/// <summary>
	/// 移動しないとき元の位置に戻す
	/// </summary>
	void NoMove(int hit);

	/// <summary>
	/// 落下する
	/// </summary>
	void Drop();

	/// <summary>
	/// 当たり判定を取得する
	/// </summary>
	/// <returns>当たり判定</returns>
	const CollisionInfo& GetCol() const { return m_colision; }

	const int GetHp()const;

	/// <summary>
	/// カメラの範囲内にいるかどうか
	/// </summary>
	/// <returns>true:範囲内 false:範囲外</returns>
	bool OnCamera();

	/// <summary>
	/// ブロックに当たった時
	/// </summary>
	void HitBlockUp();

	/// <summary>
	/// ブロックに当たった時
	/// </summary>
	void HitBlockWidth();

	/// <summary>
	/// ジャンプしているか
	/// </summary>
	/// <returns>true:している　false:していない</returns>
	bool IsJump()const { return m_isJump; }

	/// <summary>
	/// どっちを向いているか
	/// </summary>
	/// <returns></returns>
	bool IsLeft()const { return m_isLeft; }

	/// <summary>
	/// 攻撃(エネミー)に当たった
	/// </summary>
	void OnDamage();

	/// <summary>
	/// 攻撃を受けている
	/// </summary>
	/// <returns></returns>
	bool IsDamage();

	/// <summary>
	/// プレイヤーの移動スピードを取得する
	/// </summary>
	/// <returns>移動スピード</returns>
	const float GetSpeed()const;

	/// <summary>
	/// 攻撃をしたかどうか
	/// </summary>
	void Attack();

	/// <summary>
	/// アニメーションを切り替える
	/// </summary>
	/// <param name="animNo">変更先アニメーション</param>
	/// <param name="isLoop">アニメーションをループさせるか</param>
	/// <param name="isForceChange">すでに指定されたアニメが再生されている場合も変更するか</param>
	/// <param name="changeFrame">フレーム数</param>
	void SwitchAnimation(PlayerAnimation animNo, bool isLoop, bool isForceChange, int changeFrame);

	const std::shared_ptr<Model> GetModel()const { return m_model; }
private:

	/// <summary>
	/// 左に移動
	/// </summary>
	void MoveLeft();
	/// <summary>
	/// 右に移動
	/// </summary>
	void MoveRight();

	/// <summary>
	/// 待機アニメーションアップデート
	/// </summary>
	/// <param name="input"></param>
	void UpdateIdle(const InputState& input);
	/// <summary>
	/// ジャンプアップデート
	/// </summary>
	/// <param name="input"></param>
	void UpdateJump(const InputState& input);

	void (TestPlayer::* m_animUpdate)(const InputState& input);//Update用メンバ関数ポインタ

	VECTOR	m_dir;//回転方向
	VECTOR m_rot;//表示方向

	std::shared_ptr<Model> m_model;//モデル
	CollisionInfo m_colision;//当たり判定
	std::shared_ptr<Hp> m_hp;//HP

	VECTOR m_prevPos;//元の位置

	PlayerAnimation m_animType;//アニメーションタイプ

	float m_jumpAdd;//ジャンプ
	bool m_isJump;//ジャンプしているか

	bool m_isLeft;//左を見ているか

	bool m_isDamage;
	int m_flashing;//攻撃に当たった時、点滅させる
};

