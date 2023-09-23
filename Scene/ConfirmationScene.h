#pragma once
#include "Secne.h"
#include "../Util/Info.h"
#include <string>

/// <summary>
/// ���ɉ�������̂�
/// </summary>
enum class SelectType
{
	End,
	BackTitle,
	SceneMode,
};

/// <summary>
/// �m�F�V�[��
/// </summary>
class ConfirmationScene : public Scene
{
public:
	ConfirmationScene(SceneManager& manager, const wchar_t* conf, SelectType type, int soundH);
	virtual ~ConfirmationScene();

	virtual void Update(const InputState& input) override;
	void Draw();
private:
	void WindowDraw();
	void FullSceneChange();

	enum class Item
	{
		yes,//���̏���������
		no,//�߂�

		Max
	};
	Element m_pauseMenu[static_cast<int>(Item::Max)];
	int m_selectNum = static_cast<int>(Item::no);

	const wchar_t* m_conf;//�\�����镶��
	int m_stringNum;//�\�����镶���̐�
	SelectType m_type;//���������邩���w��
	int m_soundH;//�������̃V�[���ɓn������

	bool m_isWindouwMode;//�E�B���h�E���[�h��ύX����

	int m_bg;//�E�B���h�E�w�i
};

