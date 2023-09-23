#pragma once

namespace Game
{
#ifdef _DEBUG
	constexpr bool kWindowMode = true;
#else
	constexpr bool kWindowMode = true;
#endif
	//ウィンドウ名
	const wchar_t* const kTitleText = L"ウィンドウ名";
	//ウィンドウサイズ
	constexpr int kScreenWidth = 1600;//16:
	constexpr int kScreenHeight = 900;//9
	//カラーモード
	constexpr int kColorDepth = 32;	//32 or 16

};