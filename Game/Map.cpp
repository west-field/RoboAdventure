#include "Map.h"
#include "../Model.h"
#include <string>
#include <cassert>
#include "../WorldSprite.h"
#include "../Util/DrawFunctions.h"
#include "../game.h"

namespace
{
	constexpr float kJumpPower = 0.1f;//ジャンプ力
	
	constexpr float kGravity = -0.5f;//重力
	
	constexpr float kScale = 0.0065f;//大きさ設定
	
	constexpr float kDrawScale = 0.65f;//表示する大きさ
	
	constexpr float kStartPos = 8.0f;//表示開始位置(左上座標)//斜めから

	
}

//コンストラクタ
Map::Map(const wchar_t* const fileName, float bgAdd, int selectStage) :m_mapHeight(0), m_mapWidth(0)
{
	Load(fileName);
	//ファイルをロードする
	LoadModel(bgAdd);//モデルのロード

	//背景
	switch (selectStage)
	{
	case 0:
	case 2:
	default:
		m_bg[0].handle = my::MyLoadGraph(L"Data/Img/bg/bg2.png");
		m_bg[0].scrollSpeed = 0.5f;
		GetGraphSizeF(m_bg[0].handle, &m_bgHandleW, &m_bgHandleH);
		m_bg[1].handle = my::MyLoadGraph(L"Data/Img/bg/bg2.png");
		m_bg[1].scrollSpeed = 0.5f;
		m_bg[2].handle = my::MyLoadGraph(L"Data/Img/bg/bg1.png");
		m_bg[2].scrollSpeed = 1.0f;
		m_bg[3].handle = my::MyLoadGraph(L"Data/Img/bg/bg1.png");
		m_bg[3].scrollSpeed = 1.0f;
		break;
	case 1:
		m_bg[0].handle = my::MyLoadGraph(L"Data/Img/bg/bgWallback.png");
		m_bg[0].scrollSpeed = 0.5f;
		GetGraphSizeF(m_bg[0].handle, &m_bgHandleW, &m_bgHandleH);
		m_bg[1].handle = my::MyLoadGraph(L"Data/Img/bg/bgWallback.png");
		m_bg[1].scrollSpeed = 0.5f;
		m_bg[2].handle = my::MyLoadGraph(L"Data/Img/bg/bgWall.png");
		m_bg[2].scrollSpeed = 1.0f;
		m_bg[3].handle = my::MyLoadGraph(L"Data/Img/bg/bgWall.png");
		m_bg[3].scrollSpeed = 1.0f;
		break;
	}

	for (int i = 0; i < 4; i++)
	{
		m_bg[i].scroll = 0;
		if (i % 2 == 1)
		{
			m_bg[i].scroll = m_bgHandleW;
		}
	}
}
//デストラクタ
Map::~Map()
{
	m_piece.clear();
	m_pos.clear();

	for (auto& bg : m_bg)
	{
		DeleteGraph(bg.handle);
	}
}

//更新
void Map::Update()
{
	////必要なくなったピースを消す
	//auto rmIt = std::remove_if(m_piece.begin(), m_piece.end(),
	//	[](const std::shared_ptr<Model>& model)
	//	{
	//		return !model->IsExist();
	//	});
	//m_piece.erase(rmIt, m_piece.end());

	VECTOR LeftTopFlont,RightBottomBack;//左上前、右下奥
	VECTOR LeftTop = m_piece.back().coll.rect[static_cast<int>(CollisionInfo::LTRB::leftTop)];
	VECTOR RightBottom = m_piece.back().coll.rect[static_cast<int>(CollisionInfo::LTRB::rightBottom)];
	for (auto& piece : m_piece)
	{
		//左上と右下
		LeftTopFlont = VAdd(piece.coll.center, LeftTop);
		RightBottomBack = VAdd(piece.coll.center, RightBottom);
		//手前、奥
		LeftTopFlont = VAdd(LeftTopFlont, VGet(0.0f, 0.0f, kDrawScale));
		RightBottomBack = VAdd(RightBottomBack, VGet(0.0f, 0.0f, -kDrawScale));
		//if (!m_model[i]->IsExist())	continue;//表示していないものは判定しない
		//二つの座標であらわされるボックスがカメラの視界に入っていないかどうかを判定する
		if (CheckCameraViewClip_Box(LeftTopFlont, RightBottomBack))//true:視界に入っていない false:視界に入っている
		{
			if (!piece.model->IsExist()) continue;
			piece.model->SetExist(false);
			if (piece.coll.draw == 1)
			{
				piece.coll.draw = 0;
				//piece.model->DeleteModel();//モデルを消す
			}
		}
		else
		{
			if (piece.model->IsExist()) continue;
			piece.coll.draw = 1;
			piece.model->SetExist(true);
		}
		
	}
	for (auto& bg : m_bgModel)
	{
		//左上と右下
		LeftTopFlont = VAdd(bg->GetPos(), LeftTop);
		RightBottomBack = VAdd(bg->GetPos(), RightBottom);
		//手前、奥
		LeftTopFlont = VAdd(LeftTopFlont, VGet(0.0f, 0.0f, kDrawScale));
		RightBottomBack = VAdd(RightBottomBack, VGet(0.0f, 0.0f, -kDrawScale));
		//if (!m_model[i]->IsExist())	continue;//表示していないものは判定しない
		//二つの座標であらわされるボックスがカメラの視界に入っていないかどうかを判定する
		if (CheckCameraViewClip_Box(LeftTopFlont, RightBottomBack))//true:視界に入っていない false:視界に入っている
		{
			if (!bg->IsExist()) continue;
			bg->SetExist(false);
		}
		else
		{
			if (bg->IsExist()) continue;
			bg->SetExist(true);
		}
	}


	for (auto& piece : m_piece)
	{
		if (!piece.model->IsExist()) continue;
		piece.model->UpdateCollision();
	}
}
void Map::UpdateBg(bool move,bool playerJump)
{
	if (!move)	return;
	for (auto& bg : m_bg)
	{
		if (playerJump)
		{
			bg.scroll -= (bg.scrollSpeed - 0.2f);
			if (bg.scroll <= -m_bgHandleW)
			{
				bg.scroll += m_bgHandleW * 2;
			}
		}
		else
		{
			bg.scroll -= bg.scrollSpeed;
			if (bg.scroll <= -m_bgHandleW)
			{
				bg.scroll += m_bgHandleW * 2;
			}
		}
	}
}
//描画
void Map::Draw()
{
	//背景
	for (auto& bg : m_bg)
	{
		DrawGraphF(bg.scroll, 0.0f, bg.handle, true);
	}
	//背景
	for (auto& bg : m_bgModel)
	{
		if (!bg->IsExist())continue;
		bg->Draw();
	}
	//ステージ
	for (auto& piece : m_piece)
	{
		if (!piece.model->IsExist())	continue;
		piece.model->Draw();
	}
#ifdef _DEBUG

	////m_line
	//for (auto& line : m_line)
	//{
	//	DrawLine3D(line.lineFarst, line.lineSecond, 0xff0000);
	//}

	int i = 0;
	for (auto& piece : m_piece)
	{
		if (!piece.model->IsExist())	continue;
		i++;
	}
	for (auto& bg : m_bgModel)
	{
		if (!bg->IsExist())continue;
		i++;
	}
	DrawFormatString(200, 50, 0x000000, L"%d", i);
#endif
}

//マップのロード
void Map::Load(const wchar_t* filePath)
{
	m_mapData.clear();
	//FMFヘッダー（Platinumのドキュメントに書いてある）
	struct Header
	{
		int8_t id[4];			//　ファイル識別子(FMF_)　4バイト
		uint32_t size;			//　ヘッダをのぞいたデータサイズ　4バイト
		uint32_t mapWidth;		//　マップの横幅　4バイト
		uint32_t mapHeight;		//　マップの縦幅　4バイト
		uint8_t chipWidth;		//　パーツ(セル1個)の横幅　1バイト
		uint8_t chipHeight;		//　パーツ(セル1個)の縦幅　1バイト
		uint8_t layerCount;		//　レイヤー数　1バイト
		uint8_t bitCount;		//　1セル当たりのビット数(÷8でバイト数)　1バイト
	};//20バイト
	Header header;
	int handle = FileRead_open(filePath);
	FileRead_read(&header, sizeof(header), handle);

	std::string strId;
	strId.resize(4);
	std::copy_n(header.id, 4, strId.begin());
	if (strId != "FMF_")
	{
		//ロード失敗
		assert(0);
	}

	m_mapWidth = header.mapWidth;
	m_mapHeight = header.mapHeight;
	int layerCount = header.layerCount;
	//レイヤー1個当たりのサイズを計算する
	//マップの幅×マップの高さ×（マップ一個当たりのバイト数
	int layerDataSize = header.mapWidth * header.mapHeight * (header.bitCount / 8);

	m_mapData.resize(layerCount);//  resizeデータを大きくしたり小さくしたり
	for (auto& layer : m_mapData)
	{
		layer.resize(layerDataSize);
		FileRead_read(layer.data(), layerDataSize, handle);
	}

	FileRead_close(handle);

	//実行時メモリ効率化のために、データを加工
	for (int layerIdx = 0; layerIdx < layerCount; layerIdx++)
	{
		TransposeMapData(layerIdx);
	}
}

void Map::GetMapSize(int& width, int& height)
{
	width = m_mapWidth;
	height = m_mapHeight;
}

//マップのIDを取得する
const int Map::GetChipId(LayerType layerId, int chipX, int chipY)
{
	assert(chipX < m_mapWidth);
	assert(chipY < m_mapHeight);
	auto index = chipY + m_mapHeight * chipX;
	int layer = static_cast<int>(layerId);
	return m_mapData[layer][index];
}

//現在の位置にあるイベントチップを取得する
int Map::GetEventParam(float x, float y)
{
	//位置からその場所のイベントチップを取得する
	int X = 0,Y = 0;

	for (int i = 0; i < m_mapWidth * m_mapHeight; i++)
	{
		if (m_pos[i].y - kDrawScale < y && m_pos[i].y + kDrawScale > y)
		{
			if (m_pos[i].x - kDrawScale < x && m_pos[i].x + kDrawScale > x)
			{
				X = i / m_mapHeight;
				Y = i % m_mapHeight;
				break;
			}
		}
	}

	return GetChipId(LayerType::Event, X, Y);
}

int Map::GetEnemyParam(int x, int y)
{
	int enemyChip = 0;
	assert(x < m_mapWidth);
	assert(y < m_mapHeight);
	auto index = y + m_mapHeight * x;
	int layer = static_cast<int>(LayerType::Enemy);
	enemyChip = m_mapData[layer][index];

	m_mapData[layer][index] = 0;

	return enemyChip;
}

int Map::GetItemParam(int x, int y)
{
	int itemChip = 0;
	assert(x < m_mapWidth);
	assert(y < m_mapHeight);
	auto index = y + m_mapHeight * x;
	int layer = static_cast<int>(LayerType::Item);
	itemChip = m_mapData[layer][index];

	m_mapData[layer][index] = 0;

	return itemChip;
}

const std::vector<Piece>& Map::GetPiece() const
{
	return m_piece;
}

const bool Map::IsExist(int index) const
{
	return m_piece[index].model->IsExist();
}

//データの内容をXY転置する
void Map::TransposeMapData(int layerId)
{
	auto temp = m_mapData[layerId];//いったんコピーしておく　コピーせずに転置しようとすると元のデータが消える
	for (int Y = 0; Y < m_mapHeight; Y++)
	{
		for (int X = 0; X < m_mapWidth; X++)
		{
			//通常のXY指定の場合	Y*mapWidth_+Xという風になる。　さらに、横縦は維持する必要がある。
			//縦に並べていきたい　0の隣は一段下にしたい
			int idxSrc = Y * m_mapWidth + X;//Source Indexもとになるデータ
			int idxDst = Y + m_mapHeight * X;//Destination Indexコピーするデータの場所

			m_mapData[layerId][idxDst] = temp[idxSrc];
		}
	}
}

//モデルのロード
void Map::LoadModel(float add)
{
	VECTOR size = VGet(kDrawScale * 2, kDrawScale * 2, 0.0f);//当たり判定用、四角いモデルのサイズ
	//使用するモデル
	enum ModelType
	{
		GrassModel,
		GrassModel2,
		dirtModel,
		dirtModel2,
		block,
		bush,
		woodenBox,
		rock1,
		rock2,
		tree,
		goalFlag,
		soilGround,
		soilGround2,

		max
	};
	int model[ModelType::max] = {};
	model[GrassModel] = MV1LoadModel(L"Data/Model/Cube_Grass_Side_Tall.mv1");//地面、草、前のみ
	model[GrassModel2] = MV1LoadModel(L"Data/Model/Cube_Grass_Corner_Tall.mv1");//地面、草、前と横
	model[dirtModel] = MV1LoadModel(L"Data/Model/Cube_Grass_SideCenter_Tall.mv1");//地面、前のみ
	model[dirtModel2] = MV1LoadModel(L"Data/Model/Cube_Grass_CornerCenter_Tall.mv1");//地面、前と横
	model[block] = MV1LoadModel(L"Data/Model/Cube_Bricks.mv1");//ブロック
	model[bush] = MV1LoadModel(L"Data/Model/Bush.mv1");//草
	model[woodenBox] = MV1LoadModel(L"Data/Model/Cube_Crate.mv1");//木箱
	model[rock1] = MV1LoadModel(L"Data/Model/Rock_1.mv1");//岩1つ
	model[rock2] = MV1LoadModel(L"Data/Model/Rock_2.mv1");//岩2つ
	model[tree] = MV1LoadModel(L"Data/Model/Tree.mv1");//木
	model[goalFlag] = MV1LoadModel(L"Data/Model/Goal_Flag.mv1");//フラグ
	model[soilGround] = MV1LoadModel(L"Data/Model/Cube_Dirt_Side_Tall.mv1");//地面,上と前のみ
	model[soilGround2] = MV1LoadModel(L"Data/Model/Cube_Dirt_Corner_Tall.mv1");//地面,上と前と横

	float rot = -90.0f * (DX_PI_F / 180.0f);//横があるブロックを反対向きにさせる

	bool isCol = false;//当たり判定があるかどうか
	int mapChip = 0;//マップチップ
	int	eventChip = 0;//イベントチップ
	int index = 0;//インデックス
	 
	float scaleSize = 1.1f;
	m_pos.clear();
	m_piece.clear();
	for (int x = 0; x < m_mapWidth; x++)
	{
		for (int y = 0; y < m_mapHeight; y++)
		{
			m_pos.push_back(VGet((kDrawScale * 2) * x, 7.5f - (kDrawScale * 2) * y, 0.0f));//全ての位置を取得する
			
			index = y + x * m_mapHeight;//インデックス

			mapChip = m_mapData[static_cast<int>(LayerType::Map)][index];
			//マップチップが0の時はスキップ
			if (mapChip == 0)	continue;

			eventChip = m_mapData[static_cast<int>(LayerType::Event)][index];
			//イベントチップが壁、ブロックの時当たり判定をつける
			if (eventChip == static_cast<int>(EventChipType::plan) || eventChip == static_cast<int>(EventChipType::block))
			{
				isCol = true;
			}
			else
			{
				isCol = false;
			}

			if (mapChip == MapChipType::grassGround)
			{
				m_piece.push_back(Piece{ std::make_shared<Model>(model[GrassModel], kScale * scaleSize,isCol) ,
					CollisionInfo(isCol, VGet((kDrawScale * 2) * x, kStartPos - (kDrawScale * 2) * y, 0.0f), size),
					static_cast<EventChipType>(eventChip) });
			}
			else if (mapChip == MapChipType::underground)
			{
				m_piece.push_back(Piece{ std::make_shared<Model>(model[dirtModel], kScale * scaleSize,isCol) ,
					CollisionInfo(isCol, VGet((kDrawScale * 2) * x, kStartPos - (kDrawScale * 2) * y, 0.0f), size),
					static_cast<EventChipType>(eventChip) });
			}
			else if (mapChip == MapChipType::block)
			{
				m_piece.push_back(Piece{ std::make_shared<Model>(model[block], kScale,isCol) ,
					CollisionInfo(isCol, VGet((kDrawScale * 2) * x, kStartPos - (kDrawScale * 2) * y, 0.0f), size),
					static_cast<EventChipType>(eventChip) });
			}
			else if (mapChip == MapChipType::grassGroundRight)
			{
				m_piece.push_back(Piece{ std::make_shared<Model>(model[GrassModel2], kScale * scaleSize,isCol) ,
					CollisionInfo(isCol, VGet((kDrawScale * 2) * x, kStartPos - (kDrawScale * 2) * y, 0.0f), size),
					static_cast<EventChipType>(eventChip) });
				m_piece.back().model->SetRot(VGet(0.0f, rot, 0.0f));
			}
			else if (mapChip == MapChipType::undergroundRight)
			{
				m_piece.push_back(Piece{ std::make_shared<Model>(model[dirtModel2], kScale * scaleSize,isCol) ,
					CollisionInfo(isCol, VGet((kDrawScale * 2) * x, kStartPos - (kDrawScale * 2) * y, 0.0f), size),
					static_cast<EventChipType>(eventChip) });
				m_piece.back().model->SetRot(VGet(0.0f, rot, 0.0f));
			}
			else if (mapChip == MapChipType::grassGroundLeft)
			{
				m_piece.push_back(Piece{ std::make_shared<Model>(model[GrassModel2], kScale * scaleSize,isCol) ,
					CollisionInfo(isCol, VGet((kDrawScale * 2) * x, kStartPos - (kDrawScale * 2) * y, 0.0f), size),
					static_cast<EventChipType>(eventChip) });
			}
			else if (mapChip == MapChipType::undergroundLeft)
			{
				m_piece.push_back(Piece{ std::make_shared<Model>(model[dirtModel2], kScale * scaleSize,isCol) ,
					CollisionInfo(isCol, VGet((kDrawScale * 2) * x, kStartPos - (kDrawScale * 2) * y, 0.0f), size),
					static_cast<EventChipType>(eventChip) });
			}
			else if (mapChip == MapChipType::bush)
			{
				m_piece.push_back(Piece{ std::make_shared<Model>(model[bush], 0.004f,isCol) ,
					CollisionInfo(false, VGet((kDrawScale * 2) * x, kStartPos - (kDrawScale * 2) * y - 0.65f, 0.0f), size),
					static_cast<EventChipType>(eventChip) });
			}
			else if (mapChip == MapChipType::woodenBox)
			{
				m_piece.push_back(Piece{ std::make_shared<Model>(model[woodenBox], kScale * scaleSize,isCol) ,
					CollisionInfo(isCol, VGet((kDrawScale * 2) * x, kStartPos - (kDrawScale * 2) * y, 0.0f), size),
					static_cast<EventChipType>(eventChip) });
			}
			else if (mapChip == MapChipType::rock1)
			{
				m_piece.push_back(Piece{ std::make_shared<Model>(model[rock1], kScale * scaleSize,isCol) ,
					CollisionInfo(isCol, VGet((kDrawScale * 2) * x, kStartPos - (kDrawScale * 2) * y - 0.65f, 0.0f), size),
					static_cast<EventChipType>(eventChip) });
			}
			else if (mapChip == MapChipType::rock2)
			{
				m_piece.push_back(Piece{ std::make_shared<Model>(model[rock2], kScale * scaleSize,isCol) ,
					CollisionInfo(isCol, VGet((kDrawScale * 2) * x, kStartPos - (kDrawScale * 2) * y - 0.65f, 0.0f), size),
					static_cast<EventChipType>(eventChip) });
			}
			else if (mapChip == MapChipType::tree)
			{
				m_piece.push_back(Piece{ std::make_shared<Model>(model[tree], kScale * scaleSize,isCol) ,
					CollisionInfo(isCol, VGet((kDrawScale * 2) * x, kStartPos - (kDrawScale * 2) * y - 0.65f, 0.0f), size),
					static_cast<EventChipType>(eventChip) });
			}
			else if (mapChip == MapChipType::goalFlag)
			{
				m_piece.push_back(Piece{ std::make_shared<Model>(model[goalFlag], kScale * 2,isCol) ,
					CollisionInfo(isCol, VGet((kDrawScale * 2) * x, kStartPos - (kDrawScale * 2) * y - 0.65f, 0.0f), size),
					static_cast<EventChipType>(eventChip) });
			}
			else if (mapChip == MapChipType::soilGroundRight)
			{
				m_piece.push_back(Piece{ std::make_shared<Model>(model[soilGround2], kScale * scaleSize,isCol) ,
					CollisionInfo(isCol, VGet((kDrawScale * 2) * x, kStartPos - (kDrawScale * 2) * y, 0.0f), size),
					static_cast<EventChipType>(eventChip) });
				m_piece.back().model->SetRot(VGet(0.0f, rot, 0.0f));
			}
			else if (mapChip == MapChipType::soilGroundLeft)
			{
				m_piece.push_back(Piece{ std::make_shared<Model>(model[soilGround2], kScale * scaleSize,isCol) ,
					CollisionInfo(isCol, VGet((kDrawScale * 2) * x, kStartPos - (kDrawScale * 2) * y, 0.0f), size),
					static_cast<EventChipType>(eventChip) });
			}
			else if (mapChip == MapChipType::soilGround)
			{
				m_piece.push_back(Piece{ std::make_shared<Model>(model[soilGround], kScale * scaleSize,isCol) ,
					CollisionInfo(isCol, VGet((kDrawScale * 2) * x, kStartPos - (kDrawScale * 2) * y, 0.0f), size),
					static_cast<EventChipType>(eventChip) });
			}

			//当たり判定の作成
			m_piece.back().coll.isCenter = true;
			m_piece.back().coll.SetLTRB(VGet(-kDrawScale, kDrawScale, 0.0f), VGet(-kDrawScale, -kDrawScale, 0.0f),
				VGet(kDrawScale, kDrawScale, 0.0f), VGet(kDrawScale, -kDrawScale, 0.0f));
			////モデルの位置を指定
			m_piece.back().model->SetPos(m_piece.back().coll.GetCenter());
		}
	}

	m_bgModel.clear();
	//3D背景
	for (int x = 0; x < m_mapWidth; x++)
	{
		for (int y = 0; y < m_mapHeight; y++)
		{
			index = y + x * m_mapHeight;//インデックス

			mapChip = m_mapData[static_cast<int>(LayerType::Bg)][index];
			//マップチップが0の時はスキップ
			if (mapChip == 0)	continue;
			if (mapChip == MapChipType::grassGround)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[GrassModel], kScale, false));
			}
			else if (mapChip == MapChipType::underground)
			{
				m_bgModel.push_back( std::make_shared<Model>(model[dirtModel], kScale, false));
			}
			else if (mapChip == MapChipType::block)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[block], kScale, false));
			}
			else if (mapChip == MapChipType::grassGroundRight)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[GrassModel2], kScale, false));
				m_bgModel.back()->SetRot(VGet(0.0f, rot, 0.0f));
			}
			else if (mapChip == MapChipType::undergroundRight)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[dirtModel2], kScale, false));
				m_bgModel.back()->SetRot(VGet(0.0f, rot, 0.0f));
			}
			else if (mapChip == MapChipType::grassGroundLeft)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[GrassModel2], kScale, false));
			}
			else if (mapChip == MapChipType::undergroundLeft)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[dirtModel2], kScale, false));
			}
			else if (mapChip == MapChipType::bush)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[bush], 0.004f, false));
				m_bgModel.back()->SetPos(VGet((kDrawScale * 2)* x, kStartPos - (kDrawScale * 2) * y - 0.65f, (kDrawScale * 2) + add));
				continue;
			}
			else if (mapChip == MapChipType::woodenBox)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[woodenBox], kScale, false));
			}
			else if (mapChip == MapChipType::rock1)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[rock1], kScale, false));
				m_bgModel.back()->SetPos(VGet((kDrawScale * 2)* x, kStartPos - (kDrawScale * 2) * y - 0.65f, (kDrawScale * 2) + add));
				continue;
			}
			else if (mapChip == MapChipType::rock2)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[rock2], kScale, false));
				m_bgModel.back()->SetPos(VGet((kDrawScale * 2)* x, kStartPos - (kDrawScale * 2) * y - 0.65f, (kDrawScale * 2) + add));
				continue;
			}
			else if (mapChip == MapChipType::tree)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[tree], kScale, false));
				m_bgModel.back()->SetPos(VGet((kDrawScale * 2)* x, kStartPos - (kDrawScale * 2) * y - 0.65f, (kDrawScale * 2) + add));
				continue;
			}
			else if (mapChip == MapChipType::goalFlag)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[goalFlag], kScale, false));
				m_bgModel.back()->SetPos(VGet((kDrawScale * 2)* x, kStartPos - (kDrawScale * 2) * y - 0.65f, (kDrawScale * 2) + add));
				continue;
			}
			else if (mapChip == MapChipType::soilGroundRight)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[soilGround2], kScale, false));
				m_bgModel.back()->SetRot(VGet(0.0f, rot, 0.0f));
			}
			else if (mapChip == MapChipType::soilGroundLeft)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[soilGround2], kScale, false));
			}
			else if (mapChip == MapChipType::soilGround)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[soilGround], kScale, false));
			}

			m_bgModel.back()->SetPos(VGet((kDrawScale * 2)* x, kStartPos - (kDrawScale * 2) * y, (kDrawScale * 2) + add));
		}
	}

	//モデルの開放
	for (auto& mod : model)
	{
		MV1DeleteModel(mod);
	}

#if false
	//当たり判定の生成
	float startY = -0.5f;
	//最初の直進　□□□□□□□□□□□□□□□□□□□□□□□□□□□□
	m_line.push_back(Line{ VGet(0.0f,startY,0.0f),
							VGet(((kDrawScale * 2) * 28) + kDrawScale,startY,0.0f),true });
	//ブロック　□□
	//		　　□□
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 28) + kDrawScale,startY,0.0f),
							VGet(((kDrawScale * 2) * 28) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),false });

	
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 28) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),
							VGet(((kDrawScale * 2) * 30) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),true });

	
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 30) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),
							VGet(((kDrawScale * 2) * 30) + kDrawScale,startY ,0.0f),false });

	//直進　□□□□□□□□□□□□□□
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 30) + kDrawScale,startY,0.0f),
							VGet(((kDrawScale * 2) * 44) + kDrawScale,startY,0.0f),true });

	//落とし穴　□
	//　　　　　□
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 44) + kDrawScale,startY,0.0f),
							VGet(((kDrawScale * 2) * 44) + kDrawScale,startY - ((kDrawScale * 2) * 2),0.0f),false });

	//落とし穴　□
	//　　　　　□
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 45) + kDrawScale,startY,0.0f),
							VGet(((kDrawScale * 2) * 45) + kDrawScale,startY - ((kDrawScale * 2) * 2),0.0f),false });

	//直進　□□□□□□□□□□□□
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 45) + kDrawScale,startY,0.0f),
							VGet(((kDrawScale * 2) * 57) + kDrawScale,startY,0.0f),true });

	//階段　	  □
	//			□□
	//        □□□
	//		□□□□
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 57) + kDrawScale,startY ,0.0f),
							VGet(((kDrawScale * 2) * 57) + kDrawScale,startY + (kDrawScale * 2),0.0f),false });//1縦

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 57) + kDrawScale,startY + (kDrawScale * 2),0.0f),
							VGet(((kDrawScale * 2) * 58) + kDrawScale,startY + (kDrawScale * 2),0.0f),true });//1横

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 58) + kDrawScale,startY + (kDrawScale * 2),0.0f),
							VGet(((kDrawScale * 2) * 58) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),false});//2縦

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 58) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),
							VGet(((kDrawScale * 2) * 59) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),true });//2横

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 59) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),
							VGet(((kDrawScale * 2) * 59) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),false });//3縦

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 59) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),
							VGet(((kDrawScale * 2) * 60) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),true });//3横

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 60) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),
							VGet(((kDrawScale * 2) * 60) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),false });//4縦

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 60) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),
							VGet(((kDrawScale * 2) * 61) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),true });//4横

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 61) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),
							VGet(((kDrawScale * 2) * 61) + kDrawScale,startY - ((kDrawScale * 2) * 2),0.0f),false });//縦

	//階段　□
	//		□□
	//      □□□
	//		□□□□
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 62) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),
							VGet(((kDrawScale * 2) * 62) + kDrawScale,startY - ((kDrawScale * 2) * 2),0.0f),false });//縦

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 63) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),
							VGet(((kDrawScale * 2) * 62) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),true });//4横

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 63) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),
							VGet(((kDrawScale * 2) * 63) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),false });//4縦

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 64) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),
							VGet(((kDrawScale * 2) * 63) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),true });//3横

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 64) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),
							VGet(((kDrawScale * 2) * 64) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),false });//3縦

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 65) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),
							VGet(((kDrawScale * 2) * 64) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),true });//2横

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 65) + kDrawScale,startY + (kDrawScale * 2),0.0f),
							VGet(((kDrawScale * 2) * 65) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),false });//2縦

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 66) + kDrawScale,startY + (kDrawScale * 2),0.0f),
							VGet(((kDrawScale * 2) * 65) + kDrawScale,startY + (kDrawScale * 2),0.0f),true });//1横

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 66) + kDrawScale,startY ,0.0f),
							VGet(((kDrawScale * 2) * 66) + kDrawScale,startY + (kDrawScale * 2),0.0f),false });//1縦


	//直進　□□□□□□□□□□□□□□□□□□
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 66) + kDrawScale,startY ,0.0f),
							VGet(((kDrawScale * 2) * 84) + kDrawScale,startY,0.0f),true });

	//階段　	  □
	//			□□
	//        □□□
	//		□□□□
	//	  □□□□□

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 84) + kDrawScale,startY ,0.0f),
							VGet(((kDrawScale * 2) * 84) + kDrawScale,startY + (kDrawScale * 2),0.0f),false });//1縦

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 84) + kDrawScale,startY + (kDrawScale * 2),0.0f),
							VGet(((kDrawScale * 2) * 85) + kDrawScale,startY + (kDrawScale * 2),0.0f),true });//1横

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 85) + kDrawScale,startY + (kDrawScale * 2),0.0f),
							VGet(((kDrawScale * 2) * 85) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),false });//2縦

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 85) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),
							VGet(((kDrawScale * 2) * 86) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),true });//2横

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 86) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),
							VGet(((kDrawScale * 2) * 86) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),false });//3縦

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 86) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),
							VGet(((kDrawScale * 2) * 87) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),true });//3横

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 87) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),
							VGet(((kDrawScale * 2) * 87) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),false });//4縦

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 87) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),
							VGet(((kDrawScale * 2) * 88) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),true });//4横

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 88) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),
							VGet(((kDrawScale * 2) * 88) + kDrawScale,startY + ((kDrawScale * 2) * 5),0.0f),false });//4縦

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 88) + kDrawScale,startY + ((kDrawScale * 2) * 5),0.0f),
							VGet(((kDrawScale * 2) * 89) + kDrawScale,startY + ((kDrawScale * 2) * 5),0.0f),true });//4横

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 89) + kDrawScale,startY + ((kDrawScale * 2) * 5),0.0f),
							VGet(((kDrawScale * 2) * 89) + kDrawScale,startY,0.0f),false });//縦

	//直進　□□□□□□□□□
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 89) + kDrawScale,startY ,0.0f),
							VGet(((kDrawScale * 2) * 98) + kDrawScale,startY,0.0f),true });
#endif
}

