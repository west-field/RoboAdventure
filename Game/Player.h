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

class Player
{
public:
	Player(std::shared_ptr<Map> map);
	virtual ~Player();

	void Update(const InputState& input);
	void Update();

	void Draw();

	//位置を取得
	const VECTOR GetPos() const;

	/// <summary>
	/// モデルと当たっているかどうか
	/// </summary>
	/// <param name="model">モデル</param>
	bool HitModel(int model);
	
	//ダメージを受けた
	void OnDamage(int attack);
	//ダメージを受けているか
	bool IsDamage() { return m_isDamage; }
	//現在のHpを取得
	const int GetHp()const;

	//アタック
	void Attack();
	//攻撃できるか
	bool IsAttack();

	//ジャンプしているかどうか
	bool IsJump();

	//モデルを取得
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

	//通常アップデート
	void NormalUpdate(const InputState& input);
	//ジャンプする
	void JumpUpdate(const InputState& input);

	//プレイヤー移動
	void Move();

	VECTOR m_pos;//表示位置
	VECTOR m_vel;//移動量
	VECTOR m_dir;//移動方向、回転方向
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


