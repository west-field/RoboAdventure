#pragma once
#include "DxLib.h"
#include <memory>
#include <vector>
#include <list>
#include "../CollisionInfo.h"

class Model;
class WorldSprite;

/// <summary>
/// ���C���[
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
/// �}�b�v�`�b�v
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
/// �C�x���g�`�b�v
/// </summary>
enum class EventChipType
{
	no,

	hit,//��
	deth,//��
	crea,//�Q�[���N���A

	max
};

/// <summary>
/// �G�`�b�v
/// </summary>
enum class EnemyChipType
{
	no,
	
	straightLeft,//�܂������ړ����鍶�������Ă���G
	straightRight,//�܂������ړ�����E�������Ă���G
	straightFly,//�܂������ړ������ԓG

	max
};

/// <summary>
/// �A�C�e���`�b�v
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
	std::shared_ptr<Model> model;//���f��
	CollisionInfo coll;//�����蔻��
	EventChipType eventType;
};

class Map
{
public :
	//�R���X�g���N�^
	Map(const wchar_t* const fileName, float bgAdd, int selectStage);
	//�f�X�g���N�^
	virtual ~Map();

	//�A�b�v�f�[�g
	void Update();
	void UpdateBg(bool move, bool playerJump);
	//�\��
	void Draw();

	/// <summary>
	/// �w�b�_�[�ɓǂݍ���
	/// </summary>
	/// <param name="filePath">�ǂݍ��ރt�@�C����</param>
	void Load(const wchar_t* filePath);

	/// <summary>
	/// �}�b�v�̃T�C�Y���擾����
	/// </summary>
	/// <param name="width">����</param>
	/// <param name="height">�c��</param>
	void GetMapSize(int& width, int& height);

	/// <summary>
	/// �}�b�v��ID���擾����
	/// </summary>
	/// <param name="layerId">���C���[</param>
	/// <param name="chipX">X���W</param>
	/// <param name="chipY">Y���W</param>
	/// <returns></returns>
	const int GetChipId(LayerType layerId, int chipX, int chipY);

	/// <summary>
	/// ���݂̈ʒu�ɂ���C�x���g�`�b�v���擾����
	/// </summary>
	/// <param name="x">���݂�X���W</param>
	/// <param name="y">���݂�Y���W</param>
	/// <param name="posx">X���W</param>
	/// <param name="posy">Y���W</param>
	/// <returns></returns>
	int GetEventParam(float x, float y, float& posx, float& posy);

	int GetMapChipParam(float x, float y);

	/// <summary>
	/// ���݂̈ʒu�ɂ���G�l�~�[�`�b�v���擾����
	/// </summary>
	/// <param name="x">���݂�X���W</param>
	/// <param name="y">���݂�Y���W</param>
	/// <returns></returns>
	int GetEnemyParam(int x, int y);
	/// <summary>
	/// ���݂̈ʒu�ɂ���A�C�e���`�b�v���擾����
	/// </summary>
	/// <param name="x">���݂�X���W</param>
	/// <param name="y">���݂�Y���W</param>
	/// <returns></returns>
	int GetItemParam(int x, int y);

	/// <summary>
	/// �����蔻��
	/// </summary>
	/// <returns>�����蔻��</returns>
	const std::vector<Piece>& GetPiece() const;

	//const std::vector<Line> GetLinePos()const;

	/// <summary>
	/// ���݂��邩�ǂ���
	/// </summary>
	/// <returns>true:���݂��� false:���݂��Ȃ�</returns>
	const bool IsExist(int index)const;

private:
	/// <summary>
	/// �f�[�^�̓��e��XY�]�u����
	/// </summary>
	/// <param name="layerId">���C���[ID</param>
	void TransposeMapData(int layerId);

	/// <summary>
	/// ���f���̃��[�h
	/// </summary>
	void LoadModel(float add);

	using MapData_t = std::vector<std::vector<unsigned char>>;//�}�b�v�̍L���~���C���[��
	MapData_t m_mapData;//�}�b�v�S�̂̃f�[�^
	int m_mapWidth;//�}�b�v�̉���
	int m_mapHeight;//�}�b�v�̏c��

	std::vector<Piece> m_piece;//�t�B�[���h�Ɏg�p����
	std::vector<VECTOR> m_pos;//�`�b�v�擾�̂��߂Ɏg�p����

	std::vector<std::shared_ptr<Model>> m_bgModel;//�w�i�p���f��

	//2D�w�i
	struct Bg
	{
		int handle;
		float scroll;
		float scrollSpeed;
	};
	Bg m_bg[4];//�e�摜��2��
	float m_bgHandleW;//�w�i�摜�̃T�C�YW
	float m_bgHandleH;//�w�i�摜�̃T�C�YH
};

