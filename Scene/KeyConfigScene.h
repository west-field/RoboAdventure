#pragma once
#include "Secne.h"
#include <string>
#include "../KeyInfo.h"

class InputState;
class SceneManager;

/// <summary>
/// �L�[�R���t�B�O�V�[��
/// </summary>
class KeyConfigScene : public Scene
{
public:
    KeyConfigScene(SceneManager& manager, const InputState& input);
    virtual ~KeyConfigScene();
    void Update(const InputState& input);
    void Draw();
private:
    void WindowDraw();

    std::wstring PadName(int id);
    std::wstring KeyName(int id);
    std::wstring MouseName(int id);

    const InputState& m_inputState;
    int m_currentInputIndex;//���ݑI�𒆂̃C���f�b�N�X
    bool m_isEditing = false;//���ݓ���̂ɂイ��傭�͕ҏW���ł��B

    struct Element
    {
        int x, y;//���W
        int color;//�F
        std::wstring name;//���O
        int nameNum;//������
        int fontSize;//�t�H���g�̃T�C�Y
        InputType type;
    };

    Element m_keyConfing[static_cast<int>(InputType::max) + 2];

    int m_bg;//�E�B���h�E�摜

#ifdef _DEBUG
    int m_numSize = 0;
#endif
};

