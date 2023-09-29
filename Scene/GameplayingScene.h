#pragma once
#include "Secne.h"
#include <memory>

class Player;
class EnemyFactory;
class ItemFactory;
class Map;
class Camera;

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
     /// <summary>
     /// 画面のフェードイン
     /// </summary>
     /// <param name="input">入力</param>
     void FadeInUpdat(const InputState& input);
     /// <summary>
     /// 画面のフェードアウト
     /// </summary>
     void FadeOutUpdat(const InputState& input);

     /// <summary>
     /// 通常更新
     /// </summary>
     void NormalUpdat(const InputState& input);
     /// <summary>
     /// カメラをプレイヤーに近づける
     /// </summary>
     void MoveCameraCloser(const InputState& input);
     /// <summary>
     /// ゲームオーバーシーンに行く前のアクション
     /// </summary>
     void GameoverUpdate(const InputState& input);
     /// <summary>
     /// ゲームクリアシーンに行く前のアクション
     /// </summary>
     void GameclearUpdate(const InputState& input);

     /// <summary>
     /// 通常表示
     /// </summary>
     void NormalDraw();
     /// <summary>
     /// ゲームオーバー表示
     /// </summary>
     void GameoverDraw();
     /// <summary>
     /// ゲームクリア表示
     /// </summary>
     void GameclearDraw();

     /// <summary>
     /// プレイヤーとの当たり判定
     /// </summary>
     void CollisionPlayer();

     /// <summary>
     /// 敵を生成する
     /// </summary>
     void CreateEnemy();

     /// <summary>
     /// アイテムを生成
     /// </summary>
     void CreateItem();
private:
    void PointDraw(int leftX, int y, int dispNum, int digit = -1);

    unsigned int m_fadeColor = 0x000000;//フェードの色（黒
    
    void (GameplayingScene::* m_updateFunc)(const InputState& input);
    void (GameplayingScene::* m_drawFunc)();

    std::shared_ptr<Player> m_player;//プレイヤー
    std::shared_ptr<EnemyFactory> m_enemyFactory;//敵工場
    std::shared_ptr<ItemFactory> m_itemFactory;//アイテム工場
    std::shared_ptr<Map> m_map;//マップ
    std::shared_ptr<Camera> m_camera;//カメラ
    
    int m_soundVolume = 0;//サウンドの音量
    
    int m_crea = 0;//どこに移動するか

    int m_selectStage;//選択したステージ

    int m_fade;//フェード
    int m_fadetimer;//フェードの時間

    int m_score;//スコア
    int m_addScore;//1ずつスコアを表示させるために取得したスコアを保持しておく

    int m_starHandle;//スターを表示する
    int m_starOutlineHandle;//スター枠を表示する

    int m_numGraphHandle;//数字の画像ハンドル
};

