#pragma once
#include <DxLib.h>
#include <memory>
#include "../../CollisionInfo.h"

class Model;

class ItemBase
{
public:
	ItemBase(VECTOR pos,int modelHandle,float scale);
	virtual ~ItemBase();

	virtual void Update() = 0;
	virtual void Draw() = 0;

	const CollisionInfo& GetColl()const { return m_collision; }

	/// <summary>
	/// ���݂��Ă��邩�ǂ������擾����
	/// </summary>
	/// <returns>true:���݂��Ă��� false:���݂��Ă��Ȃ�</returns>
	bool IsExist()const { return m_isExist; }

	//���f�����擾
	const int GetModel()const;

	/// <summary>
	/// �����������ǂ���
	/// </summary>
	virtual void IsHit() = 0;

	//���_���擾����
	virtual int Score() = 0;

	//�R�C���ԍ����擾����
	int GetCoin() const { return m_coinNumber; }
protected:
	void OnCamera();

	VECTOR m_dir;//�\������
	VECTOR m_rot;//��]����

	std::shared_ptr<Model> m_model;//���f��

	CollisionInfo m_collision;//�����蔻��

	bool m_isExist;//���݂��邩�ǂ���

	int m_coinNumber;//���Ԗڂ̃R�C����
};

