#pragma once
#include <DxLib.h>
#include <memory>

class InputState;
class Model;
class Map;
class Hp;

/// <summary>
/// プレイヤーアニメーション
/// </summary>
enum class PlayerAnimation
{
	Dance = 0,		//ダンス
	Death = 1,		//崩れる
	Idle = 2,		//立ち止まり
	Jump = 3,		//その場ジャンプ
	No = 4,			//首振り
	Punch = 5,		//パンチ
	Run = 6,		//走る
	ThumbsUp = 9,	//
	Walk = 10,		//歩く
	WalkJump = 11,	//歩きながらジャンプ
	Wave = 12,		//
	Yes = 13,		//うなずく
};

class Player
{
public:
	Player(std::shared_ptr<Map> map);
	virtual ~Player();

	void Update(const InputState& input);
	void Update();

	void Draw();

	/// <summary>
	/// 表示位置を取得
	/// </summary>
	/// <returns>表示位置</returns>
	const VECTOR GetPos() const;

	/// <summary>
	/// モデルと当たっているかどうか
	/// </summary>
	/// <param name="model">モデル</param>
	bool HitModel(int model);
	
	/// <summary>
	/// ダメージを受けた
	/// </summary>
	/// <param name="attack">ダメージ量</param>
	void OnDamage(int attack);
	//ダメージを受けているか
	
	/// <summary>
	/// すでにダメージを受けているか
	/// </summary>
	/// <returns>true:ダメージを受けている false:ダメージを受けていない</returns>
	bool IsDamage() { return m_isDamage; }

	/// <summary>
	/// 現在のHPを取得
	/// </summary>
	/// <returns>現在のHP</returns>
	const int GetHp()const;

	/// <summary>
	/// 攻撃する
	/// </summary>
	void Attack();

	/// <summary>
	/// 攻撃ができるかどうかを取得する
	/// </summary>
	/// <returns>true:攻撃できる false:攻撃できない</returns>
	bool IsAttack();

	/// <summary>
	/// ジャンプしているかどうかを取得する
	/// </summary>
	/// <returns>true:ジャンプしている false:ジャンプしていない</returns>
	bool IsJump();

	/// <summary>
	/// モデルを取得する
	/// </summary>
	/// <returns>モデル</returns>
	const std::shared_ptr<Model> GetModel() const;

	/// <summary>
	/// アニメーションを切り替える
	/// </summary>
	/// <param name="animNo">変更先アニメーション</param>
	/// <param name="isLoop">アニメーションをループさせるか</param>
	/// <param name="isForceChange">すでに指定されたアニメが再生されている場合も変更するか</param>
	/// <param name="changeFrame">フレーム数</param>
	void SwitchAnimation(PlayerAnimation animNo, bool isLoop, bool isForceChange, int changeFrame);

	/// <summary>
	/// カメラの範囲内にいるかどうか
	/// </summary>
	/// <returns>true:範囲内 false:範囲外</returns>
	bool OnCamera();
private:
	void (Player::* m_update)(const InputState& input);//Update用メンバ関数ポインタ

	/// <summary>
	/// 通常アップデート
	/// </summary>
	/// <param name="input">キー入力</param>
	void NormalUpdate(const InputState& input);
	
	/// <summary>
	/// ジャンプアップデート
	/// </summary>
	/// <param name="input">キー入力</param>
	void JumpUpdate(const InputState& input);

	/// <summary>
	/// 移動判定
	/// </summary>
	void Move();

	VECTOR m_pos;//表示位置
	VECTOR m_vel;//移動量
	VECTOR m_dir;//移動方向
	VECTOR m_rot;//表示方向

	std::shared_ptr<Model> m_model;//モデル
	PlayerAnimation m_animType;//現在再生しているアニメーション

	std::shared_ptr<Map> m_map;//マップ

	float m_jumpPower;//ジャンプ力
	bool m_isAttack;//攻撃できる

	bool m_isDamage;//攻撃を受けた
	int m_flashing;//ダメージを受けたとき点滅させる

	std::shared_ptr<Hp> m_hp;//HP
};


