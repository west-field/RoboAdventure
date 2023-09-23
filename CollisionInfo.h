#pragma once
#include "DxLib.h"

/// <summary>
/// 当たり判定
/// </summary>
struct CollisionInfo
{
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="isCol">当たり判定をするかどうか</param>
	CollisionInfo(bool isCol);
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="isCol">当たり判定をするかどうか</param>
	/// <param name="pos">中心位置</param>
	/// <param name="si">サイズ</param>
	CollisionInfo(bool isCol,const VECTOR pos,const VECTOR si);
	virtual ~CollisionInfo();//デストラクタ

	/// <summary>
	/// 上下左右を指定することで矩形を構築
	/// </summary>
	/// <param name="leftTop"></param>
	/// <param name="leftBottom"></param>
	/// <param name="rightTop"></param>
	/// <param name="rightBottom"></param>
	void SetLTRB(VECTOR leftTop, VECTOR leftBottom, VECTOR rightTop, VECTOR rightBottom);

	/// <summary>
	/// 現在の中心位置を取得
	/// </summary>
	/// <returns>中心位置</returns>
	const VECTOR& GetCenter()const;
	/// <summary>
	/// 大きさを取得する
	/// </summary>
	/// <returns>大きさ</returns>
	const VECTOR& GetSize()const;
	/// <summary>
	/// 移動力を取得
	/// </summary>
	/// <returns>移動力</returns>
	const VECTOR& GetVel()const;

	/// <summary>
	/// デバッグ用矩形表示
	/// </summary>
	/// <param name="color">色を指定できる(なくてもok)</param>
	void Draw(unsigned int color = 0xffffff)const;

	/// <summary>
	/// もう一つの矩形と当たったかどうか
	/// </summary>
	/// <param name="col">当たり判定</param>
	/// <returns>true:当たった　false:外れた</returns>
	bool IsHit(const CollisionInfo& col)const;

	/// <summary>
	/// 当たり判定をするかどうか
	/// </summary>
	/// <returns>true:当たり判定をする false:当たり判定をしない</returns>
	const bool GetCollider() const { return isCollider; }

	VECTOR center;//中心位置
	VECTOR size;//サイズ
	VECTOR velocity;//移動力

	bool isCenter;//モデル位置が中心にあるかどうか

	bool isCollider;//当たり判定をするかどうか
	
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

	int draw = 0;
};

