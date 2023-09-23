#pragma once
#include<tchar.h>
#include "Geometry.h"

//�����p���C�u�����p���O���
namespace my
{
	/// <summary>
	/// �O���t�B�b�N�����[�h����
	/// </summary>
	/// <param name="path">�p�X������</param>
	/// <returns>�摜�̃n���h��</returns>
	int MyLoadGraph(const TCHAR* path);

	/// <summary>
	/// �O���t�B�b�N�̃T�C�Y�𓾂�
	/// </summary>
	/// <param name="handle">�摜�̃n���h��</param>
	/// <returns>�摜�̃T�C�Y</returns>
	Position2 MyGetGraphSize(const int handle);

	/// <summary>
	/// �O���t�B�b�N�̕`��
	/// </summary>
	/// <param name="x">������Wx</param>
	/// <param name="y">������Wy</param>
	/// <param name="handle">�摜�̃n���h��</param>
	/// <param name="trasnsFlg">�����x�w��</param>
	/// <returns>�摜�\��</returns>
	int MyDrawGraph(int x, int y, int handle, bool trasnsFlg);

	/// <summary>
	/// �O���t�B�b�N��\������
	/// </summary>
	/// <param name="x">�`�撆�S���Wx</param>
	/// <param name="x">�`�撆�S���Wy</param>
	/// <param name="left">�`�悵������`�̍����W</param>
	/// <param name="top">�`�悵������`�̍����W</param>
	/// <param name="width">�`�悷��O���t�B�b�N�̃T�C�Y</param>
	/// <param name="height">�`�悷��O���t�B�b�N�̃T�C�Y</param>
	/// <param name="scale">�g�嗦</param>
	/// <param name="angle">�`��p�x</param>
	/// <param name="handle">�`�悷��O���t�B�b�N�̃n���h��</param>
	/// <param name="transFlg">�摜�����x�̗L��</param>
	/// <param name="turnFlg">�摜���]</param>
	/// <returns>�摜�\��</returns>
	int MyDrawRectRotaGraph(int x, int y, int left, int top, int width, int height, float scale, float angle, int handle, bool transFlg, bool turnFlg);

	/// <summary>
	/// �O���t�B�b�N��\������
	/// </summary>
	/// <param name="x">�`�撆�S���Wx</param>
	/// <param name="x">�`�撆�S���Wy</param>
	/// <param name="left">�`�悵������`�̍����W</param>
	/// <param name="top">�`�悵������`�̍����W</param>
	/// <param name="width">�`�悷��O���t�B�b�N�̃T�C�Y</param>
	/// <param name="height">�`�悷��O���t�B�b�N�̃T�C�Y</param>
	/// <param name="scale">�g�嗦</param>
	/// <param name="angle">�`��p�x</param>
	/// <param name="handle">�`�悷��O���t�B�b�N�̃n���h��</param>
	/// <param name="transFlg">�摜�����x�̗L��</param>
	/// <param name="turnFlg">�摜���]</param>
	/// <returns>�摜�\��</returns>
	int MyDrawRectRotaGraph(float x, float y, int left, int top, int width, int height, float scale, float angle, int handle, bool transFlg, bool turnFlg);
}
