#pragma once
#include "Secne.h"
#include <string>
#include "../KeyInfo.h"

class InputState;
class SceneManager;

/// <summary>
/// キーコンフィグシーン
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
    int m_currentInputIndex;//現在選択中のインデックス
    bool m_isEditing = false;//現在特定のにゅうりょくは編集中です。

    struct Element
    {
        int x, y;//座標
        int color;//色
        std::wstring name;//名前
        int nameNum;//文字数
        int fontSize;//フォントのサイズ
        InputType type;
    };

    Element m_keyConfing[static_cast<int>(InputType::max) + 2];

    int m_bg;//ウィンドウ画像

#ifdef _DEBUG
    int m_numSize = 0;
#endif
};

