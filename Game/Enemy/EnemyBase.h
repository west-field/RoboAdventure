#pragma once
#include "Dxlib.h"
#include <memory>
#include "../../CollisionInfo.h"

class Model;
class Map;

class EnemyBase
{
public:
	EnemyBase(VECTOR pos,int modelHandle,float scale,bool isLeft, std::shared_ptr<Map> map);
	virtual ~EnemyBase();

	//更新
	virtual void Update() = 0;
	//表示
	virtual void Draw() = 0;

	//移動する
	virtual void Move() = 0;
	//スコアを取得する
	virtual int Score() = 0;

	const int GetModel()const;

	/// <summary>
	/// 存在しているかどうかを取得する
	/// </summary>
	/// <returns>true:存在している fasle:存在していない</returns>
	bool IsExist();

	/// <summary>
	/// 攻撃を受けた
	/// </summary>
	void OnDamage();

	/// <summary>
	/// 攻撃力を取得する
	/// </summary>
	/// <returns>攻撃力</returns>
	int AttackDamage();

	/// <summary>
	/// 攻撃をすでに受けている
	/// </summary>
	/// <returns>ture:攻撃を受けた false:攻撃を受けてない</returns>
	const bool IsBeAttack()const { return m_isHit; }

	//エフェクトを表示
	void EffectDraw();

	/// <summary>
	/// 反対を向く
	/// </summary>
	void Reverse();

	virtual const VECTOR GetEffectPos() const = 0;
protected:
	/// <summary>
	/// カメラの範囲内にいたら存在している
	/// </summary>
	void OnCamera();
	/// <summary>
	/// 上下左右を指定することで矩形を構築
	/// </summary>
	/// <param name="leftTop"></param>
	/// <param name="leftBottom"></param>
	/// <param name="rightTop"></param>
	/// <param name="rightBottom"></param>
	void SetLTRB(VECTOR leftTop, VECTOR leftBottom, VECTOR rightTop, VECTOR rightBottom);

	VECTOR m_pos;//表示位置
	VECTOR m_vel;//移動量
	VECTOR m_dir;//表示方向
	VECTOR m_rot;//回転方向

	//矩形
	enum class LTRB
	{
		leftTop,
		leftBottom,
		rightTop,
		rightBottom,
		Max
	};
	VECTOR rect[static_cast<int>(LTRB::Max)];
	VECTOR size;//サイズ

	VECTOR m_prevPos;//前の位置

	std::shared_ptr<Model> m_model;//モデル

	std::shared_ptr<Map> m_map;//当たり判定に用いるマップ

	bool m_isLeft;//左を向いているか

	bool m_isExist;//存在するかどうか

	bool m_isHit;//攻撃を受けたかどうか
};

