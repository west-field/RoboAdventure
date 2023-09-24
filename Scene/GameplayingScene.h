#pragma once
#include "Secne.h"
#include <memory>

class Player;
class EnemyFactory;
class ItemFactory;
class Map;
class Camera;
class FileInformation;

/// <summary>
/// ゲーム中シーン
/// </summary>
class GameplayingScene : public Scene
{
public:
    GameplayingScene(SceneManager& manager, const wchar_t* const fileName, const int select, std::shared_ptr<Camera> camera);
    virtual ~GameplayingScene();

    virtual void Update(const InputState& input);
    virtual void Draw();
private:
     //画面のフェードイン
     void FadeInUpdat(const InputState& input);
     //画面のフェードアウト
     void FadeOutUpdat(const InputState& input);

     //通常更新
     void NormalUpdat(const InputState& input);
     //カメラをプレイヤーに近づける
     void MoveCameraCloser(const InputState& input);
     //ゲームオーバーシーンに行く前のアクション
     void GameoverUpdate(const InputState& input);
     //ゲームクリアシーンに行く前のアクション
     void GameclearUpdate(const InputState& input);

     void NormalDraw();
     void GameoverDraw();
     void GameclearDraw();

     //プレイヤーとの当たり判定
     void CollisionPlayer();

     //敵を生成する
     void CreateEnemy();

     //アイテムを生成
     void CreateItem();
private:
    unsigned int m_fadeColor = 0x000000;//フェードの色（黒
    
    void (GameplayingScene::* m_updateFunc)(const InputState& input);
    void (GameplayingScene::* m_drawFunc)();

    std::shared_ptr<Player> m_player;//プレイヤー
    std::shared_ptr<EnemyFactory> m_enemyFactory;//敵工場
    std::shared_ptr<ItemFactory> m_itemFactory;//アイテム工場
    std::shared_ptr<Map> m_map;//マップ
    std::shared_ptr<Camera> m_camera;//カメラ
    std::shared_ptr<FileInformation> m_file;//ファイル
    
    int m_soundVolume = 0;//サウンドの音量
    
    int m_crea = 0;//どこに移動するか

    int m_selectStage;//選択したステージ

    int m_fade;//フェード
    int m_fadetimer;//フェードの時間

    int m_score;//スコア

    int m_starHandle;//スターを表示する
    int m_starOutlineHandle;//スター枠を表示する
};

