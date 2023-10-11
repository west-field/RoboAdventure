#pragma once
#include "Secne.h"

class DebugScene : public Scene
{
public:
	DebugScene(SceneManager& manager);
	virtual ~DebugScene();

	void Update(const InputState& input);

	void Draw();

private:
	//��ʂ̃t�F�[�h�C��
	void FadeInUpdat(const InputState& input);
	//��ʂ̃t�F�[�h�A�E�g
	void FadeOutUpdat(const InputState& input);

	//�ʏ�X�V
	void NormalUpdat(const InputState& input);
	//�X�e�[�W�ύX
	void StageSelect(const InputState& input);

	unsigned int m_fadeColor = 0x000000;//�t�F�[�h�̐F�i��

	void (DebugScene::* m_updateFunc)(const InputState& input);



	enum class SelectScene
	{
		Title,
		Gameplaying,
		SelectStage,
		ClearScene,
		gameoverScene,

		SelectMax
	};
	int m_select;//���֍s���V�[����I������

	int m_stage;//�X�e�[�W��I������0����2(3�X�e�[�W)

	int m_drawY;//�I�����Ă���ʒu�ɖ���\������

	int m_color[static_cast<int>(SelectScene::SelectMax)];//�F
};

