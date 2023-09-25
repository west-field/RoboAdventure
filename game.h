#pragma once

namespace Game
{
#ifdef _DEBUG
	constexpr bool kWindowMode = true;
#else
	constexpr bool kWindowMode = true;
#endif
	//�E�B���h�E��
	const wchar_t* const kTitleText = L"RoboAdventure";
	//�E�B���h�E�T�C�Y
	constexpr int kScreenWidth = 1600;//16:
	constexpr int kScreenHeight = 900;//9
	//�J���[���[�h
	constexpr int kColorDepth = 32;	//32 or 16

};