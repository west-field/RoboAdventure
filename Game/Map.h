#pragma once
#include "DxLib.h"
#include <memory>
#include <vector>
#include <list>
#include "../CollisionInfo.h"

class Model;
class WorldSprite;

/// <summary>
/// レイヤー
/// </summary>
enum class LayerType
{
	Map,
	Event,
	Bg,
	Enemy,
	Item,
};
/// <summary>
/// マップチップ
/// </summary>
enum MapChipType
{
	no,

	grassGround,
	underground,
	block,
	grassGroundRight,
	undergroundRight,
	grassGroundLeft,
	undergroundLeft,
	bush,
	woodenBox,
	rock1,
	rock2,
	tree,
	goalFlag,
	soilGroundLeft,
	soilGroundRight,
	soilGround,

	max
};
/// <summary>
/// イベントチップ
/// </summary>
enum class EventChipType
{
	no,

	hit,//床
	deth,//死
	crea,//ゲームクリア

	max
};

/// <summary>
/// 敵チップ
/// </summary>
enum class EnemyChipType
{
	no,
	
	straightLeft,//まっすぐ移動する左を向いている敵
	straightRight,//まっすぐ移動する右を向いている敵
	straightFly,//まっすぐ移動する飛ぶ敵

	max
};

/// <summary>
/// アイテムチップ
/// </summary>
enum class ItemChipType
{
	no,

	coin1,
	coin2,
	coin3,

	max
};

struct Piece
{
	std::shared_ptr<Model> model;//モデル
	CollisionInfo coll;//当たり判定
	EventChipType eventType;
};

class Map
{
public :
	//コンストラクタ
	Map(const wchar_t* const fileName, float bgAdd, int selectStage);
	//デストラクタ
	virtual ~Map();

	//アップデート
	void Update();
	void UpdateBg(bool move, bool playerJump);
	//表示
	void Draw();

	/// <summary>
	/// ヘッダーに読み込む
	/// </summary>
	/// <param name="filePath">読み込むファイル名</param>
	void Load(const wchar_t* filePath);

	/// <summary>
	/// マップのサイズを取得する
	/// </summary>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	void GetMapSize(int& width, int& height);

	/// <summary>
	/// マップのIDを取得する
	/// </summary>
	/// <param name="layerId">レイヤー</param>
	/// <param name="chipX">X座標</param>
	/// <param name="chipY">Y座標</param>
	/// <returns></returns>
	const int GetChipId(LayerType layerId, int chipX, int chipY);

	/// <summary>
	/// 現在の位置にあるイベントチップを取得する
	/// </summary>
	/// <param name="x">現在のX座標</param>
	/// <param name="y">現在のY座標</param>
	/// <param name="posx">X座標</param>
	/// <param name="posy">Y座標</param>
	/// <returns></returns>
	int GetEventParam(float x, float y, float& posx, float& posy);

	/// <summary>
	/// 現在の位置にあるエネミーチップを取得する
	/// </summary>
	/// <param name="x">現在のX座標</param>
	/// <param name="y">現在のY座標</param>
	/// <returns></returns>
	int GetEnemyParam(int x, int y);
	/// <summary>
	/// 現在の位置にあるアイテムチップを取得する
	/// </summary>
	/// <param name="x">現在のX座標</param>
	/// <param name="y">現在のY座標</param>
	/// <returns></returns>
	int GetItemParam(int x, int y);

	/// <summary>
	/// 当たり判定
	/// </summary>
	/// <returns>当たり判定</returns>
	const std::vector<Piece>& GetPiece() const;

	//const std::vector<Line> GetLinePos()const;

	/// <summary>
	/// 存在するかどうか
	/// </summary>
	/// <returns>true:存在する false:存在しない</returns>
	const bool IsExist(int index)const;

private:
	/// <summary>
	/// データの内容をXY転置する
	/// </summary>
	/// <param name="layerId">レイヤーID</param>
	void TransposeMapData(int layerId);

	/// <summary>
	/// モデルのロード
	/// </summary>
	void LoadModel(float add);

	using MapData_t = std::vector<std::vector<unsigned char>>;//マップの広さ×レイヤー数
	MapData_t m_mapData;//マップ全体のデータ
	int m_mapWidth;//マップの横幅
	int m_mapHeight;//マップの縦幅

	std::vector<Piece> m_piece;//フィールドに使用する
	std::vector<VECTOR> m_pos;//チップ取得のために使用する

	std::vector<std::shared_ptr<Model>> m_bgModel;//背景用モデル

	//2D背景
	struct Bg
	{
		int handle;
		float scroll;
		float scrollSpeed;
	};
	Bg m_bg[4];//各画像を2枚
	float m_bgHandleW;//背景画像のサイズW
	float m_bgHandleH;//背景画像のサイズH
};

