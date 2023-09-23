#pragma once
#include <DxLib.h>
#include <memory>
#include "../../CollisionInfo.h"

class Model;

class ItemBase
{
public:
	ItemBase(VECTOR pos,int modelHandle,float scale);
	virtual ~ItemBase();

	virtual void Update() = 0;
	virtual void Draw() = 0;

	const CollisionInfo& GetColl()const { return m_collision; }

	/// <summary>
	/// 存在しているかどうかを取得する
	/// </summary>
	/// <returns>true:存在している false:存在していない</returns>
	bool IsExist()const { return m_isExist; }

	//モデルを取得
	const int GetModel()const;

	/// <summary>
	/// 当たったかどうか
	/// </summary>
	virtual void IsHit() = 0;

	//得点を取得する
	virtual int Score() = 0;

	//コイン番号を取得する
	int GetCoin() const { return m_coinNumber; }
protected:
	void OnCamera();

	VECTOR m_dir;//表示方向
	VECTOR m_rot;//回転方向

	std::shared_ptr<Model> m_model;//モデル

	CollisionInfo m_collision;//当たり判定

	bool m_isExist;//存在するかどうか

	int m_coinNumber;//何番目のコインか
};

