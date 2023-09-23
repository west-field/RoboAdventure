#pragma once
#include "Secne.h"
#include <memory>

class InputState;
class Map;
class FileInformation;
class Camera;

/// <summary>
/// �^�C�g���V�[��
/// </summary>
class TitleScene : public Scene
{
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="manager"></param>
	/// <param name="start">true:�^�C�g����ʂ���n�߂� false:�X�e�[�W�I����ʂ���n�߂�</param>
	TitleScene(SceneManager& manager,bool start);
	virtual ~TitleScene();

	void Update(const InputState& input);
	virtual void Draw() override;

private:
	//�t�F�[�h�C���̎���Update�֐�
	void FadeInUpdat(const InputState& input);
	//�t�F�[�h�A�E�g�̎���Update�֐�
	void FadeOutUpdat(const InputState& input);

	//�ʏ��Ԃ̎���Update�֐�
	void NormalUpdat(const InputState& input);
	//�J���������Ɉړ�����
	void MoveDropUpdate(const InputState& input);
	//�J��������Ɉړ�����
	void MoveUpUpdate(const InputState& input);
	//�X�e�[�W�I�����
	void StageSelectUpdate(const InputState& input);

	//�^�C�g�����S��\��
	void TitleRogoDraw();
	//�����\�����Ȃ�
	void NoneDraw();
	//�X�e�[�W�I���̂��߂̉摜��\��
	void StageSelectDraw();

	void (TitleScene::* m_updateFunc)(const InputState& input);//Update�p�����o�֐��|�C���^
	void (TitleScene::* m_drawFunc)();//Draw�p�����o�֐��|�C���^

	//�X�e�[�W����
	enum StageSelect
	{
		First,		//1-1
		Second,		//1-2
		Third,		//1-3

		stageNum			//�X�e�[�W�̐�
	};
	//�X�^�[
	struct Star
	{
		int x, y;//�ʒu
		bool isGet;//�擾���Ă��邩
	};
	//�X�e�[�W�v�f
	struct StageElement
	{
		int x,y;//���W
		int handle;//�\������摜
		float scale;//�g�嗦
		bool isSelect;//�I���ł��邩�ǂ���
		int fade;//�Â�����

		Star star[3];//�X�^�[
		int highScore;//�n�C�X�R�A
	};
	
	int m_titleRogoHandle;//�^�C�g�����S

	int m_flashingTitleText;//������_�ł�����
	int m_flashingTitleTextTime;//������_�ł����鎞��

	std::shared_ptr<Map> m_map;//�}�b�v
	std::shared_ptr<Camera> m_camera;//�J����
	std::shared_ptr<FileInformation> m_file;//�t�@�C��

	StageElement m_selectStage[StageSelect::stageNum];//�X�e�[�W���

	int m_starHandle;//�X�^�[�摜
	int m_starOutlineHandle;//�X�^�[�A�E�g���C���摜

	int  m_selectNum;//���I�����Ă���X�e�[�W

	bool m_isStart;//�Q�[���J�n���ď��߂ă^�C�g����\�������Ƃ�
};
