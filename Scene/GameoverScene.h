#pragma once
#include "Secne.h"
#include "../Util/Geometry.h"
#include "../game.h"

#include <array>
#include <memory>

class Map;
class Camera;
class Model;
class ItemStar;

/// <summary>
/// ゲームオーバーシーン
/// </summary>
class GameoverScene : public Scene
{
public:
    GameoverScene(SceneManager& manager, const int selectStage, const int score, std::shared_ptr<Camera> camera, std::shared_ptr<Model> model,bool getCoin1, bool getCoin2, bool getCoin3);
    virtual ~GameoverScene();

    void Update(const InputState& input);
    void Draw();
private:
    void FadeInUpdat(const InputState& input);
    void FadeOutUpdat(const InputState& input);
    void NormalUpdat(const InputState& input);

    void NormalDraw();

    void (GameoverScene::*m_updateFunc )(const InputState&) ;
    void (GameoverScene::*m_drawFunc )() ;

    unsigned int m_fadeColor;//フェードの色（赤

    int  m_soundVolume = 0;

    int m_selectNum = 0;//選択しているメニュー項目

    std::shared_ptr<Map> m_map;
    std::shared_ptr<Camera> m_camera;

    std::shared_ptr<Model> m_model;//表示するプレイヤーモデルモデル

    std::shared_ptr<ItemStar> m_starHandle[3];//収集物表示
    bool m_getStar[3];//取得状況
    int m_score;//スコア
};

