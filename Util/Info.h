#pragma once
#include "../game.h"

//�|�[�Y��ݒ�Ȃǂŕ\������E�B���h�E�Ɏg�p����萔
namespace PushSceneWindow
{
	//�E�B���h�E�̑傫��
	constexpr int pw_width = 450;//��
	constexpr int pw_height = 400;//�c
	//�E�B���h�E�̃X�^�[�g�ʒu
	constexpr int pw_start_x = (Game::kScreenWidth - pw_width) / 2;//��
	constexpr int pw_start_y = (Game::kScreenHeight - pw_height) / 2;//�c

	//�����X�^�[�g�ʒu
	constexpr int kPosX = pw_start_x + 10;//��
	constexpr int kPosY = pw_start_y + 10;//�c

	constexpr unsigned int kNormalColor = 0x808080;//�ʏ�̃J���[
	constexpr unsigned int kSelectColor = 0x000080;//�I�����̃J���[
}

//�|�[�Y��ݒ�ȂǂŕK�v�� ����
struct Element
{
	int x, y;//���W
	int color;//�F
	const wchar_t* name;//���O
	int nameNum;//������
	int fontSize;//�t�H���g�̃T�C�Y
};
