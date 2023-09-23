#include "DrawFunctions.h"
#include <DxLib.h>
#include <cassert>

namespace my {
	/// <summary>
	/// �O���t�B�b�N�����[�h����
	/// </summary>
	/// <param name="path">�摜�t�@�C���̃p�X</param>
	/// <returns>�摜�̃n���h��</returns>
	int MyLoadGraph(const TCHAR* path)
	{
		int handle = LoadGraph(path);
		assert(handle >= 0);//���藧���Ȃ��Ƃ��̓N���b�V��
		return handle;
	}

	Position2 MyGetGraphSize(const int handle)
	{
		Position2 size = {};

		GetGraphSizeF(handle, &size.x, &size.y);

		return size;
	}

	int MyDrawGraph(int x, int y, int handle, bool trasnsFlg)
	{
		return DrawGraph(x, y,handle, trasnsFlg);
	}

	int MyDrawRectRotaGraph(int x, int y, int left, int top, int width, int height, float scale, float angle, int handle, bool transFlg, bool turnFlg)
	{
		return DrawRectRotaGraph(x, y, left, top, width, height, scale, angle, handle, transFlg, turnFlg);
	}

	int MyDrawRectRotaGraph(float x, float y, int left, int top, int width, int height, float scale, float angle, int handle, bool transFlg, bool turnFlg)
	{
		int X = static_cast<int>(x);
		int Y = static_cast<int>(y);
		return DrawRectRotaGraph(X, Y, left, top, width, height, scale, angle, handle, transFlg, turnFlg);
	}

}//end of namespace my