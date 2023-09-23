#pragma once
#include "Secne.h"
#include "../game.h"
#include <memory>
#include <array>
#include "../Util/Geometry.h"

class InputState;
/// <summary>
/// ���m���[�O�V�[��
/// </summary>
class MonologueScene : public Scene
{
public:
	MonologueScene(SceneManager& manager);
	virtual ~MonologueScene();

	void Update(const InputState& input);
	virtual void Draw() override;

private:
	int m_selectNum = 0;//�I�����Ă��郁�j���[����
	unsigned int m_color = 0x000000;
	//�t�F�[�h�C���̎���Update�֐�
	void FadeInUpdat(const InputState& input);
	//�ʏ��Ԃ̎���Update�֐�
	void NormalUpdat(const InputState& input);
	//�t�F�[�h�A�E�g�̎���Update�֐�
	void FadeOutUpdat(const InputState& input);

	//Update�p�����o�֐��|�C���^
	void (MonologueScene::* m_updateFunc)(const InputState& input);
};
//x224,y160
