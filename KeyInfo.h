#pragma once

/// <summary>
/// ���̓^�C�v
/// </summary>
enum class InputType
{
	next,//���֍s���{�^��
	prev,//�߂�
	pause,//�|�[�Y�{�^��
	up,//��
	down,//��
	left,//��
	right,//�E

	junp,//�W�����v

	max
};

/// <summary>
/// ���͑��u�J�e�S��
/// </summary>
enum class InputCategory
{
	keybd,
	pad,
	mouse
};