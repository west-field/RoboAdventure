#pragma once

/// <summary>
/// 入力タイプ
/// </summary>
enum class InputType
{
	next,//次へ行くボタン
	prev,//戻る
	pause,//ポーズボタン
	up,//上
	down,//下
	left,//左
	right,//右

	junp,//ジャンプ

	max
};

/// <summary>
/// 入力装置カテゴリ
/// </summary>
enum class InputCategory
{
	keybd,
	pad,
	mouse
};