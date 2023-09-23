#pragma once

#include <deque>

//�v���g�^�C�v�錾
class Scene;//�V�[�����N���X
class InputState;//���̓X�e�[�g

/// <summary>
/// �V�[���Ǘ��N���X
/// </summary>
class SceneManager
{
private:
	//�����s���̃V�[����؂�ւ��Ă��������̂ŎQ�Ƃł͂Ȃ��|�C���^�Ƃ��Đ錾
	std::deque<Scene*> m_scenes;
	//�Q�[�����I��������
	bool m_isEnd = false;
	bool m_isWindouwMode;
public:
	/// <summary>
	/// �V�[���̐؂�ւ����s��
	/// </summary>
	/// <param name="scene">�؂�ւ��������V�[���̃A�h���X</param>
	void ChangeScene(Scene* scene);

	/// <summary>
	/// �V�[������ɐς�
	/// Update�Ŏ��s�����̂͏�ɐς܂ꂽ�V�[���̂�
	/// </summary>
	void PushScene(Scene* scene);
	
	/// <summary>
	/// ��ԏ�̃V�[�����폜����
	/// �������A���̌��ʃX�^�b�N����ɂȂ邱�Ƃ̂Ȃ��悤��
	/// </summary>
	void PopScene();

	/// <summary>
	/// �e�V�[���̍X�V���s��
	/// </summary>
	/// <param name="input">���̓X�e�[�g</param>
	void Update(const InputState& input);
	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

	void SetIsEnd() { m_isEnd = true; }
	bool GetIsEnd() const { return m_isEnd; }
	
	bool GetIsWindouMode()const { return m_isWindouwMode; }
	void SetIsWindouMode(bool isWindou) { m_isWindouwMode = isWindou; }
};
