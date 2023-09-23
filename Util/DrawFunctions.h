#pragma once
#include<tchar.h>
#include "Geometry.h"

//自分用ライブラリ用名前空間
namespace my
{
	/// <summary>
	/// グラフィックをロードする
	/// </summary>
	/// <param name="path">パス文字列</param>
	/// <returns>画像のハンドル</returns>
	int MyLoadGraph(const TCHAR* path);

	/// <summary>
	/// グラフィックのサイズを得る
	/// </summary>
	/// <param name="handle">画像のハンドル</param>
	/// <returns>画像のサイズ</returns>
	Position2 MyGetGraphSize(const int handle);

	/// <summary>
	/// グラフィックの描画
	/// </summary>
	/// <param name="x">左上座標x</param>
	/// <param name="y">左上座標y</param>
	/// <param name="handle">画像のハンドル</param>
	/// <param name="trasnsFlg">透明度指定</param>
	/// <returns>画像表示</returns>
	int MyDrawGraph(int x, int y, int handle, bool trasnsFlg);

	/// <summary>
	/// グラフィックを表示する
	/// </summary>
	/// <param name="x">描画中心座標x</param>
	/// <param name="x">描画中心座標y</param>
	/// <param name="left">描画したい矩形の左座標</param>
	/// <param name="top">描画したい矩形の左座標</param>
	/// <param name="width">描画するグラフィックのサイズ</param>
	/// <param name="height">描画するグラフィックのサイズ</param>
	/// <param name="scale">拡大率</param>
	/// <param name="angle">描画角度</param>
	/// <param name="handle">描画するグラフィックのハンドル</param>
	/// <param name="transFlg">画像透明度の有無</param>
	/// <param name="turnFlg">画像反転</param>
	/// <returns>画像表示</returns>
	int MyDrawRectRotaGraph(int x, int y, int left, int top, int width, int height, float scale, float angle, int handle, bool transFlg, bool turnFlg);

	/// <summary>
	/// グラフィックを表示する
	/// </summary>
	/// <param name="x">描画中心座標x</param>
	/// <param name="x">描画中心座標y</param>
	/// <param name="left">描画したい矩形の左座標</param>
	/// <param name="top">描画したい矩形の左座標</param>
	/// <param name="width">描画するグラフィックのサイズ</param>
	/// <param name="height">描画するグラフィックのサイズ</param>
	/// <param name="scale">拡大率</param>
	/// <param name="angle">描画角度</param>
	/// <param name="handle">描画するグラフィックのハンドル</param>
	/// <param name="transFlg">画像透明度の有無</param>
	/// <param name="turnFlg">画像反転</param>
	/// <returns>画像表示</returns>
	int MyDrawRectRotaGraph(float x, float y, int left, int top, int width, int height, float scale, float angle, int handle, bool transFlg, bool turnFlg);
}
