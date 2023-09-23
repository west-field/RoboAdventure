#pragma once
#include "../game.h"

//ポーズや設定などで表示するウィンドウに使用する定数
namespace PushSceneWindow
{
	//ウィンドウの大きさ
	constexpr int pw_width = 450;//横
	constexpr int pw_height = 400;//縦
	//ウィンドウのスタート位置
	constexpr int pw_start_x = (Game::kScreenWidth - pw_width) / 2;//横
	constexpr int pw_start_y = (Game::kScreenHeight - pw_height) / 2;//縦

	//文字スタート位置
	constexpr int kPosX = pw_start_x + 10;//横
	constexpr int kPosY = pw_start_y + 10;//縦

	constexpr unsigned int kNormalColor = 0x808080;//通常のカラー
	constexpr unsigned int kSelectColor = 0x000080;//選択時のカラー
}

//ポーズや設定などで必要な 項目
struct Element
{
	int x, y;//座標
	int color;//色
	const wchar_t* name;//名前
	int nameNum;//文字数
	int fontSize;//フォントのサイズ
};
