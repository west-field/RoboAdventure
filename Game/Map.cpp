#include "Map.h"
#include "../Model.h"
#include <string>
#include <cassert>
#include "../WorldSprite.h"
#include "../Util/DrawFunctions.h"
#include "../game.h"

namespace
{
	constexpr float kJumpPower = 0.1f;//�W�����v��
	
	constexpr float kGravity = -0.5f;//�d��
	
	constexpr float kScale = 0.0065f;//�傫���ݒ�
	
	constexpr float kDrawScale = 0.65f;//�\������傫��
	
	constexpr float kStartPos = 8.0f;//�\���J�n�ʒu(������W)//�΂߂���

	
}

//�R���X�g���N�^
Map::Map(const wchar_t* const fileName, float bgAdd, int selectStage) :m_mapHeight(0), m_mapWidth(0)
{
	Load(fileName);
	//�t�@�C�������[�h����
	LoadModel(bgAdd);//���f���̃��[�h

	//�w�i
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
//�f�X�g���N�^
Map::~Map()
{
	m_piece.clear();
	m_pos.clear();

	for (auto& bg : m_bg)
	{
		DeleteGraph(bg.handle);
	}
}

//�X�V
void Map::Update()
{
	////�K�v�Ȃ��Ȃ����s�[�X������
	//auto rmIt = std::remove_if(m_piece.begin(), m_piece.end(),
	//	[](const std::shared_ptr<Model>& model)
	//	{
	//		return !model->IsExist();
	//	});
	//m_piece.erase(rmIt, m_piece.end());

	VECTOR LeftTopFlont,RightBottomBack;//����O�A�E����
	VECTOR LeftTop = m_piece.back().coll.rect[static_cast<int>(CollisionInfo::LTRB::leftTop)];
	VECTOR RightBottom = m_piece.back().coll.rect[static_cast<int>(CollisionInfo::LTRB::rightBottom)];
	for (auto& piece : m_piece)
	{
		//����ƉE��
		LeftTopFlont = VAdd(piece.coll.center, LeftTop);
		RightBottomBack = VAdd(piece.coll.center, RightBottom);
		//��O�A��
		LeftTopFlont = VAdd(LeftTopFlont, VGet(0.0f, 0.0f, kDrawScale));
		RightBottomBack = VAdd(RightBottomBack, VGet(0.0f, 0.0f, -kDrawScale));
		//if (!m_model[i]->IsExist())	continue;//�\�����Ă��Ȃ����͔̂��肵�Ȃ�
		//��̍��W�ł���킳���{�b�N�X���J�����̎��E�ɓ����Ă��Ȃ����ǂ����𔻒肷��
		if (CheckCameraViewClip_Box(LeftTopFlont, RightBottomBack))//true:���E�ɓ����Ă��Ȃ� false:���E�ɓ����Ă���
		{
			if (!piece.model->IsExist()) continue;
			piece.model->SetExist(false);
			if (piece.coll.draw == 1)
			{
				piece.coll.draw = 0;
				//piece.model->DeleteModel();//���f��������
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
		//����ƉE��
		LeftTopFlont = VAdd(bg->GetPos(), LeftTop);
		RightBottomBack = VAdd(bg->GetPos(), RightBottom);
		//��O�A��
		LeftTopFlont = VAdd(LeftTopFlont, VGet(0.0f, 0.0f, kDrawScale));
		RightBottomBack = VAdd(RightBottomBack, VGet(0.0f, 0.0f, -kDrawScale));
		//if (!m_model[i]->IsExist())	continue;//�\�����Ă��Ȃ����͔̂��肵�Ȃ�
		//��̍��W�ł���킳���{�b�N�X���J�����̎��E�ɓ����Ă��Ȃ����ǂ����𔻒肷��
		if (CheckCameraViewClip_Box(LeftTopFlont, RightBottomBack))//true:���E�ɓ����Ă��Ȃ� false:���E�ɓ����Ă���
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
//�`��
void Map::Draw()
{
	//�w�i
	for (auto& bg : m_bg)
	{
		DrawGraphF(bg.scroll, 0.0f, bg.handle, true);
	}
	//�w�i
	for (auto& bg : m_bgModel)
	{
		if (!bg->IsExist())continue;
		bg->Draw();
	}
	//�X�e�[�W
	for (auto& piece : m_piece)
	{
		if (!piece.model->IsExist())	continue;
		piece.model->Draw();
	}
#ifdef _DEBUG
	VECTOR m_first, m_second;
	//�L���[�u�̃��C���������@4

	for (auto& pos : m_pos)
	{
		//���ォ��E��
		m_first = VAdd(pos, VGet(-kDrawScale, kDrawScale, -1));
		m_second = VAdd(pos, VGet(kDrawScale, kDrawScale, -1));
		DrawLine3D(m_first, m_second, 0x000000);
		//���ォ�獶��
		m_first = VAdd(pos, VGet(-kDrawScale, kDrawScale, -1));
		m_second = VAdd(pos, VGet(-kDrawScale, -kDrawScale, -1));
		DrawLine3D(m_first, m_second, 0x000000);
		//�E�ォ��E��
		m_first = VAdd(pos, VGet(kDrawScale, kDrawScale, -1));
		m_second = VAdd(pos, VGet(kDrawScale, -kDrawScale, -1));
		DrawLine3D(m_first, m_second, 0x000000);
		//��������E��
		m_first = VAdd(pos, VGet(-kDrawScale, -kDrawScale, -1));
		m_second = VAdd(pos, VGet(kDrawScale, -kDrawScale, -1));
		DrawLine3D(m_first, m_second, 0x000000);
	}
	for (auto& piece : m_piece)
	{
		if (!piece.model->IsExist())	continue;
		if (piece.eventType != EventChipType::hit)	continue;
		//���ォ��E��
		m_first = VAdd(piece.model->GetPos(), VGet(-kDrawScale, kDrawScale, -1));
		m_second = VAdd(piece.model->GetPos(), VGet(kDrawScale, kDrawScale, -1));
		DrawLine3D(m_first, m_second, 0xff0000);
		//���ォ�獶��
		m_first = VAdd(piece.model->GetPos(), VGet(-kDrawScale, kDrawScale, -1));
		m_second = VAdd(piece.model->GetPos(), VGet(-kDrawScale, -kDrawScale, -1));
		DrawLine3D(m_first, m_second, 0xff0000);
		//�E�ォ��E��
		m_first = VAdd(piece.model->GetPos(), VGet(kDrawScale, kDrawScale, -1));
		m_second = VAdd(piece.model->GetPos(), VGet(kDrawScale, -kDrawScale, -1));
		DrawLine3D(m_first, m_second, 0xff0000);
		//��������E��
		m_first = VAdd(piece.model->GetPos(), VGet(-kDrawScale, -kDrawScale, -1));
		m_second = VAdd(piece.model->GetPos(), VGet(kDrawScale, -kDrawScale, -1));
		DrawLine3D(m_first, m_second, 0xff0000);
	}
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

//�}�b�v�̃��[�h
void Map::Load(const wchar_t* filePath)
{
	m_mapData.clear();
	//FMF�w�b�_�[�iPlatinum�̃h�L�������g�ɏ����Ă���j
	struct Header
	{
		int8_t id[4];			//�@�t�@�C�����ʎq(FMF_)�@4�o�C�g
		uint32_t size;			//�@�w�b�_���̂������f�[�^�T�C�Y�@4�o�C�g
		uint32_t mapWidth;		//�@�}�b�v�̉����@4�o�C�g
		uint32_t mapHeight;		//�@�}�b�v�̏c���@4�o�C�g
		uint8_t chipWidth;		//�@�p�[�c(�Z��1��)�̉����@1�o�C�g
		uint8_t chipHeight;		//�@�p�[�c(�Z��1��)�̏c���@1�o�C�g
		uint8_t layerCount;		//�@���C���[���@1�o�C�g
		uint8_t bitCount;		//�@1�Z��������̃r�b�g��(��8�Ńo�C�g��)�@1�o�C�g
	};//20�o�C�g
	Header header;
	int handle = FileRead_open(filePath);
	FileRead_read(&header, sizeof(header), handle);

	std::string strId;
	strId.resize(4);
	std::copy_n(header.id, 4, strId.begin());
	if (strId != "FMF_")
	{
		//���[�h���s
		assert(0);
	}

	m_mapWidth = header.mapWidth;
	m_mapHeight = header.mapHeight;
	int layerCount = header.layerCount;
	//���C���[1������̃T�C�Y���v�Z����
	//�}�b�v�̕��~�}�b�v�̍����~�i�}�b�v�������̃o�C�g��
	int layerDataSize = header.mapWidth * header.mapHeight * (header.bitCount / 8);

	m_mapData.resize(layerCount);//  resize�f�[�^��傫�������菬����������
	for (auto& layer : m_mapData)
	{
		layer.resize(layerDataSize);
		FileRead_read(layer.data(), layerDataSize, handle);
	}

	FileRead_close(handle);

	//���s���������������̂��߂ɁA�f�[�^�����H
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

//�}�b�v��ID���擾����
const int Map::GetChipId(LayerType layerId, int chipX, int chipY)
{
	assert(chipX < m_mapWidth);
	assert(chipY < m_mapHeight);
	auto index = chipY + m_mapHeight * chipX;
	int layer = static_cast<int>(layerId);
	return m_mapData[layer][index];
}

//���݂̈ʒu�ɂ���C�x���g�`�b�v���擾����
int Map::GetEventParam(float x, float y, float& posx, float& posy)
{
	//�ʒu���炻�̏ꏊ�̃C�x���g�`�b�v���擾����
	int X = 0,Y = 0;

	for (int i = 0; i < m_mapWidth * m_mapHeight; i++)
	{
		if (m_pos[i].y - kDrawScale < y && m_pos[i].y + kDrawScale > y)
		{
			if (m_pos[i].x - kDrawScale < x && m_pos[i].x + kDrawScale > x)
			{
				posx = m_pos[i].x;
				posy = m_pos[i].y;

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

//�f�[�^�̓��e��XY�]�u����
void Map::TransposeMapData(int layerId)
{
	auto temp = m_mapData[layerId];//��������R�s�[���Ă����@�R�s�[�����ɓ]�u���悤�Ƃ���ƌ��̃f�[�^��������
	for (int Y = 0; Y < m_mapHeight; Y++)
	{
		for (int X = 0; X < m_mapWidth; X++)
		{
			//�ʏ��XY�w��̏ꍇ	Y*mapWidth_+X�Ƃ������ɂȂ�B�@����ɁA���c�͈ێ�����K�v������B
			//�c�ɕ��ׂĂ��������@0�ׂ̗͈�i���ɂ�����
			int idxSrc = Y * m_mapWidth + X;//Source Index���ƂɂȂ�f�[�^
			int idxDst = Y + m_mapHeight * X;//Destination Index�R�s�[����f�[�^�̏ꏊ

			m_mapData[layerId][idxDst] = temp[idxSrc];
		}
	}
}

//���f���̃��[�h
void Map::LoadModel(float add)
{
	VECTOR size = VGet(kDrawScale * 2, kDrawScale * 2, 0.0f);//�����蔻��p�A�l�p�����f���̃T�C�Y
	//�g�p���郂�f��
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
	model[GrassModel] = MV1LoadModel(L"Data/Model/Cube_Grass_Side_Tall.mv1");//�n�ʁA���A�O�̂�
	model[GrassModel2] = MV1LoadModel(L"Data/Model/Cube_Grass_Corner_Tall.mv1");//�n�ʁA���A�O�Ɖ�
	model[dirtModel] = MV1LoadModel(L"Data/Model/Cube_Grass_SideCenter_Tall.mv1");//�n�ʁA�O�̂�
	model[dirtModel2] = MV1LoadModel(L"Data/Model/Cube_Grass_CornerCenter_Tall.mv1");//�n�ʁA�O�Ɖ�
	model[block] = MV1LoadModel(L"Data/Model/Cube_Bricks.mv1");//�u���b�N
	model[bush] = MV1LoadModel(L"Data/Model/Bush.mv1");//��
	model[woodenBox] = MV1LoadModel(L"Data/Model/Cube_Crate.mv1");//�ؔ�
	model[rock1] = MV1LoadModel(L"Data/Model/Rock_1.mv1");//��1��
	model[rock2] = MV1LoadModel(L"Data/Model/Rock_2.mv1");//��2��
	model[tree] = MV1LoadModel(L"Data/Model/Tree.mv1");//��
	model[goalFlag] = MV1LoadModel(L"Data/Model/Goal_Flag.mv1");//�t���O
	model[soilGround] = MV1LoadModel(L"Data/Model/Cube_Dirt_Side_Tall.mv1");//�n��,��ƑO�̂�
	model[soilGround2] = MV1LoadModel(L"Data/Model/Cube_Dirt_Corner_Tall.mv1");//�n��,��ƑO�Ɖ�

	float rot = -90.0f * (DX_PI_F / 180.0f);//��������u���b�N�𔽑Ό����ɂ�����

	bool isCol = false;//�����蔻�肪���邩�ǂ���
	int mapChip = 0;//�}�b�v�`�b�v
	int	eventChip = 0;//�C�x���g�`�b�v
	int index = 0;//�C���f�b�N�X
	 
	float scaleSize = 1.1f;
	m_pos.clear();
	m_piece.clear();
	for (int x = 0; x < m_mapWidth; x++)
	{
		for (int y = 0; y < m_mapHeight; y++)
		{
			m_pos.push_back(VGet((kDrawScale * 2) * x, kStartPos - (kDrawScale * 2) * y, 0.0f));//�S�Ă̈ʒu���擾����
			
			index = y + x * m_mapHeight;//�C���f�b�N�X

			mapChip = m_mapData[static_cast<int>(LayerType::Map)][index];
			//�}�b�v�`�b�v��0�̎��̓X�L�b�v
			if (mapChip == 0)	continue;

			eventChip = m_mapData[static_cast<int>(LayerType::Event)][index];
			//�C�x���g�`�b�v���ǁA�u���b�N�̎������蔻�������
			if (eventChip == static_cast<int>(EventChipType::hit))
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
				m_piece.push_back(Piece{ std::make_shared<Model>(model[woodenBox], kScale,isCol) ,
					CollisionInfo(isCol, VGet((kDrawScale * 2) * x, kStartPos - (kDrawScale * 2) * y, 0.0f), size),
					static_cast<EventChipType>(eventChip) });
			}
			else if (mapChip == MapChipType::rock1)
			{
				m_piece.push_back(Piece{ std::make_shared<Model>(model[rock1], kScale,isCol) ,
					CollisionInfo(isCol, VGet((kDrawScale * 2) * x, kStartPos - (kDrawScale * 2) * y - 0.65f, 0.0f), size),
					static_cast<EventChipType>(eventChip) });
			}
			else if (mapChip == MapChipType::rock2)
			{
				m_piece.push_back(Piece{ std::make_shared<Model>(model[rock2], kScale,isCol) ,
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

			//�����蔻��̍쐬
			m_piece.back().coll.isCenter = true;
			m_piece.back().coll.SetLTRB(VGet(-kDrawScale, kDrawScale, 0.0f), VGet(-kDrawScale, -kDrawScale, 0.0f),
				VGet(kDrawScale, kDrawScale, 0.0f), VGet(kDrawScale, -kDrawScale, 0.0f));
			////���f���̈ʒu���w��
			m_piece.back().model->SetPos(m_piece.back().coll.GetCenter());
		}
	}

	m_bgModel.clear();
	//3D�w�i
	for (int x = 0; x < m_mapWidth; x++)
	{
		for (int y = 0; y < m_mapHeight; y++)
		{
			index = y + x * m_mapHeight;//�C���f�b�N�X

			mapChip = m_mapData[static_cast<int>(LayerType::Bg)][index];
			//�}�b�v�`�b�v��0�̎��̓X�L�b�v
			if (mapChip == 0)	continue;
			if (mapChip == MapChipType::grassGround)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[GrassModel], kScale* scaleSize, false));
			}
			else if (mapChip == MapChipType::underground)
			{
				m_bgModel.push_back( std::make_shared<Model>(model[dirtModel], kScale* scaleSize, false));
			}
			else if (mapChip == MapChipType::block)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[block], kScale, false));
			}
			else if (mapChip == MapChipType::grassGroundRight)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[GrassModel2], kScale* scaleSize, false));
				m_bgModel.back()->SetRot(VGet(0.0f, rot, 0.0f));
			}
			else if (mapChip == MapChipType::undergroundRight)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[dirtModel2], kScale* scaleSize, false));
				m_bgModel.back()->SetRot(VGet(0.0f, rot, 0.0f));
			}
			else if (mapChip == MapChipType::grassGroundLeft)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[GrassModel2], kScale* scaleSize, false));
			}
			else if (mapChip == MapChipType::undergroundLeft)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[dirtModel2], kScale* scaleSize, false));
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
				m_bgModel.push_back(std::make_shared<Model>(model[goalFlag], kScale * 2, false));
				m_bgModel.back()->SetPos(VGet((kDrawScale * 2)* x, kStartPos - (kDrawScale * 2) * y - 0.65f, (kDrawScale * 2) + add));
				continue;
			}
			else if (mapChip == MapChipType::soilGroundRight)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[soilGround2], kScale * scaleSize, false));
				m_bgModel.back()->SetRot(VGet(0.0f, rot, 0.0f));
			}
			else if (mapChip == MapChipType::soilGroundLeft)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[soilGround2], kScale * scaleSize, false));
			}
			else if (mapChip == MapChipType::soilGround)
			{
				m_bgModel.push_back(std::make_shared<Model>(model[soilGround], kScale * scaleSize, false));
			}

			m_bgModel.back()->SetPos(VGet((kDrawScale * 2)* x, kStartPos - (kDrawScale * 2) * y, (kDrawScale * 2) + add));
		}
	}

	//���f���̊J��
	for (auto& mod : model)
	{
		MV1DeleteModel(mod);
	}

#if false
	//�����蔻��̐���
	float startY = -0.5f;
	//�ŏ��̒��i�@��������������������������������������������������������
	m_line.push_back(Line{ VGet(0.0f,startY,0.0f),
							VGet(((kDrawScale * 2) * 28) + kDrawScale,startY,0.0f),true });
	//�u���b�N�@����
	//		�@�@����
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 28) + kDrawScale,startY,0.0f),
							VGet(((kDrawScale * 2) * 28) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),false });

	
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 28) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),
							VGet(((kDrawScale * 2) * 30) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),true });

	
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 30) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),
							VGet(((kDrawScale * 2) * 30) + kDrawScale,startY ,0.0f),false });

	//���i�@����������������������������
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 30) + kDrawScale,startY,0.0f),
							VGet(((kDrawScale * 2) * 44) + kDrawScale,startY,0.0f),true });

	//���Ƃ����@��
	//�@�@�@�@�@��
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 44) + kDrawScale,startY,0.0f),
							VGet(((kDrawScale * 2) * 44) + kDrawScale,startY - ((kDrawScale * 2) * 2),0.0f),false });

	//���Ƃ����@��
	//�@�@�@�@�@��
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 45) + kDrawScale,startY,0.0f),
							VGet(((kDrawScale * 2) * 45) + kDrawScale,startY - ((kDrawScale * 2) * 2),0.0f),false });

	//���i�@������������������������
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 45) + kDrawScale,startY,0.0f),
							VGet(((kDrawScale * 2) * 57) + kDrawScale,startY,0.0f),true });

	//�K�i�@	  ��
	//			����
	//        ������
	//		��������
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 57) + kDrawScale,startY ,0.0f),
							VGet(((kDrawScale * 2) * 57) + kDrawScale,startY + (kDrawScale * 2),0.0f),false });//1�c

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 57) + kDrawScale,startY + (kDrawScale * 2),0.0f),
							VGet(((kDrawScale * 2) * 58) + kDrawScale,startY + (kDrawScale * 2),0.0f),true });//1��

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 58) + kDrawScale,startY + (kDrawScale * 2),0.0f),
							VGet(((kDrawScale * 2) * 58) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),false});//2�c

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 58) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),
							VGet(((kDrawScale * 2) * 59) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),true });//2��

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 59) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),
							VGet(((kDrawScale * 2) * 59) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),false });//3�c

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 59) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),
							VGet(((kDrawScale * 2) * 60) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),true });//3��

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 60) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),
							VGet(((kDrawScale * 2) * 60) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),false });//4�c

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 60) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),
							VGet(((kDrawScale * 2) * 61) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),true });//4��

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 61) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),
							VGet(((kDrawScale * 2) * 61) + kDrawScale,startY - ((kDrawScale * 2) * 2),0.0f),false });//�c

	//�K�i�@��
	//		����
	//      ������
	//		��������
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 62) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),
							VGet(((kDrawScale * 2) * 62) + kDrawScale,startY - ((kDrawScale * 2) * 2),0.0f),false });//�c

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 63) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),
							VGet(((kDrawScale * 2) * 62) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),true });//4��

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 63) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),
							VGet(((kDrawScale * 2) * 63) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),false });//4�c

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 64) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),
							VGet(((kDrawScale * 2) * 63) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),true });//3��

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 64) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),
							VGet(((kDrawScale * 2) * 64) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),false });//3�c

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 65) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),
							VGet(((kDrawScale * 2) * 64) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),true });//2��

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 65) + kDrawScale,startY + (kDrawScale * 2),0.0f),
							VGet(((kDrawScale * 2) * 65) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),false });//2�c

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 66) + kDrawScale,startY + (kDrawScale * 2),0.0f),
							VGet(((kDrawScale * 2) * 65) + kDrawScale,startY + (kDrawScale * 2),0.0f),true });//1��

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 66) + kDrawScale,startY ,0.0f),
							VGet(((kDrawScale * 2) * 66) + kDrawScale,startY + (kDrawScale * 2),0.0f),false });//1�c


	//���i�@������������������������������������
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 66) + kDrawScale,startY ,0.0f),
							VGet(((kDrawScale * 2) * 84) + kDrawScale,startY,0.0f),true });

	//�K�i�@	  ��
	//			����
	//        ������
	//		��������
	//	  ����������

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 84) + kDrawScale,startY ,0.0f),
							VGet(((kDrawScale * 2) * 84) + kDrawScale,startY + (kDrawScale * 2),0.0f),false });//1�c

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 84) + kDrawScale,startY + (kDrawScale * 2),0.0f),
							VGet(((kDrawScale * 2) * 85) + kDrawScale,startY + (kDrawScale * 2),0.0f),true });//1��

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 85) + kDrawScale,startY + (kDrawScale * 2),0.0f),
							VGet(((kDrawScale * 2) * 85) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),false });//2�c

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 85) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),
							VGet(((kDrawScale * 2) * 86) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),true });//2��

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 86) + kDrawScale,startY + ((kDrawScale * 2) * 2),0.0f),
							VGet(((kDrawScale * 2) * 86) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),false });//3�c

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 86) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),
							VGet(((kDrawScale * 2) * 87) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),true });//3��

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 87) + kDrawScale,startY + ((kDrawScale * 2) * 3),0.0f),
							VGet(((kDrawScale * 2) * 87) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),false });//4�c

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 87) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),
							VGet(((kDrawScale * 2) * 88) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),true });//4��

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 88) + kDrawScale,startY + ((kDrawScale * 2) * 4),0.0f),
							VGet(((kDrawScale * 2) * 88) + kDrawScale,startY + ((kDrawScale * 2) * 5),0.0f),false });//4�c

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 88) + kDrawScale,startY + ((kDrawScale * 2) * 5),0.0f),
							VGet(((kDrawScale * 2) * 89) + kDrawScale,startY + ((kDrawScale * 2) * 5),0.0f),true });//4��

	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 89) + kDrawScale,startY + ((kDrawScale * 2) * 5),0.0f),
							VGet(((kDrawScale * 2) * 89) + kDrawScale,startY,0.0f),false });//�c

	//���i�@������������������
	m_line.push_back(Line{ VGet(((kDrawScale * 2) * 89) + kDrawScale,startY ,0.0f),
							VGet(((kDrawScale * 2) * 98) + kDrawScale,startY,0.0f),true });
#endif
}

