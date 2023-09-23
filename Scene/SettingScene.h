#pragma once
#include "Secne.h"
#include "../game.h"
#include "../Util/Info.h"

class InputState;
class SceneManager;
/// <summary>
/// �ݒ�V�[��
/// </summary>
class SettingScene : public Scene
{
public:
	SettingScene(SceneManager& manager, int soundH);
	virtual ~SettingScene();

	virtual void Update(const InputState& input) override;
	void Draw();
private:
	void WindowDraw();

	enum Item
	{
		pauseKeyconfig,//�L�[�ݒ�
		pauseSound,//���ʐݒ�
		pauseFullScene,//�X�N���[�����[�h�ύX
		pauseBack,//����
		pauseGameEnd,//�Q�[�����I���

		pauseMax
	};

	Element m_pauseMenu[pauseMax];

	int m_selectNum = 0;

	int m_soundH;

	int m_bg;
};

