#pragma once
#include "DxLib.h"
#include <list>
#include <memory>
#include <map>

class EnemyBase;
class Map;

enum class EnemyType
{
	StraightLeft,//�܂������ړ����鍶�������Ă���G
	StraightRight,//�܂������ړ�����E�������Ă���G
	StraightFly,//�܂�������ԓG
};

/// <summary>
/// �G�H��
/// </summary>
class EnemyFactory
{
public:
	EnemyFactory(std::shared_ptr<Map> map);
	virtual ~EnemyFactory();

	/// <summary>
	/// �G�S���̍X�V
	/// </summary>
	void Update();
	/// <summary>
	/// �G�S���̕`��
	/// </summary>
	void Draw();

	/// <summary>
	/// �G�I�u�W�F�N�g�̐������s��
	/// �������āA�����̃��X�g�ŊǗ�����
	/// </summary>
	/// <param name="type">�G���</param>
	/// <param name="pos">�������W</param>
	/// <returns>�G�I�u�W�F�N�g</returns>
	std::shared_ptr<EnemyBase> Create(EnemyType type, const VECTOR pos);

	/// <summary>
	/// �G�S�Ă��擾����
	/// </summary>
	/// <returns>�G</returns>
	std::list<std::shared_ptr<EnemyBase>> GetEnemies();
private:
	std::list<std::shared_ptr<EnemyBase>> m_enemies;//�G

	std::map<EnemyType, int> m_handleMap;//�G�̃n���h��

	std::shared_ptr<Map> m_map;

	int m_effectHandle;//�G�t�F�N�g
	int m_playingEffect;//�G�t�F�N�g��\��
};

