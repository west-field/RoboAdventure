#pragma once
#include "Secne.h"
#include "../Util/Geometry.h"
#include <memory>

class FileInformation;
class Map;
class Camera;
class Model;
class ItemStar;

/// <summary>
/// ゲームクリアシーン
/// </summary>
class GameclearScene : public Scene
{
public:
    GameclearScene(SceneManager& manager,const int selectStage,const int score, std::shared_ptr<Camera> camera, std::shared_ptr<Model> model, bool getCoin1, bool getCoin2, bool getCoin3);
    virtual ~GameclearScene();

    void Update(const InputState& input);
    void Draw();
private:
    unsigned int m_fadeColor = 0x000000;//フェードの色（黒

    //フェードイン
    void FadeInUpdat(const InputState& input);
    //フェードアウト
    void FadeOutUpdat(const InputState& input);

    //通常更新
    void NormalUpdat(const InputState& input);

    //通常表示
    void NormalDraw();

    void (GameclearScene::* m_updateFunc)(const InputState&);
    void (GameclearScene::* m_drawFunc)();

    int m_soundVolume = 0;//音の大きさ

    std::shared_ptr<FileInformation> m_file;//ファイル

    std::shared_ptr<Map> m_map;//マップ
    std::shared_ptr<Camera> m_camera;//カメラ

    std::shared_ptr<Model> m_model;//表示するプレイヤーモデルモデル

    std::shared_ptr<ItemStar> m_starHandle[3];//収集物表示
    bool m_getCoin[3];//取得状況

    int m_waitingTime;//待ち時間 次操作できるようになるまでの時間

    int m_selectStage;//選択しているステージ

    int m_score;//スコア
    int m_highScore;//ハイスコア
};

