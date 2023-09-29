#pragma once
#include "Secne.h"
#include "../game.h"
#include "../Util/Info.h"

class InputState;
class SceneManager;
/// <summary>
/// �|�[�Y�V�[��
/// </summary>
class PauseScene : public Scene
{
public:
	PauseScene(SceneManager& manager, int soundH);
	virtual ~PauseScene();

	virtual void Update(const InputState& input) override;
	void Draw();
private:
	void WindowDraw();

	enum Item
	{
		pauseKeyconfig,//�L�[�ݒ�
		pauseSound,//���ʐݒ�
		//pauseFullscreen,//��ʕ\����؂�ւ���
		pauseBack,//����
		pauseTitle,//�^�C�g���ɖ߂�
		pauseGameEnd,//�Q�[�����I���

		pauseMax
	};

	Element m_pauseMenu[Item::pauseMax];
	int m_selectNum;

	int m_soundH;

	int m_bg;
};

