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
/// �Q�[�����V�[��
/// </summary>
class GameplayingScene : public Scene
{
public:
    GameplayingScene(SceneManager& manager, const wchar_t* const fileName, const int select, std::shared_ptr<Camera> camera);
    virtual ~GameplayingScene();

    virtual void Update(const InputState& input);
    virtual void Draw();
private:
     //��ʂ̃t�F�[�h�C��
     void FadeInUpdat(const InputState& input);
     //��ʂ̃t�F�[�h�A�E�g
     void FadeOutUpdat(const InputState& input);

     //�ʏ�X�V
     void NormalUpdat(const InputState& input);
     //�J�������v���C���[�ɋ߂Â���
     void MoveCameraCloser(const InputState& input);
     //�Q�[���I�[�o�[�V�[���ɍs���O�̃A�N�V����
     void GameoverUpdate(const InputState& input);
     //�Q�[���N���A�V�[���ɍs���O�̃A�N�V����
     void GameclearUpdate(const InputState& input);

     void NormalDraw();
     void GameoverDraw();
     void GameclearDraw();

     //�v���C���[�Ƃ̓����蔻��
     void CollisionPlayer();

     //�G�𐶐�����
     void CreateEnemy();

     //�A�C�e���𐶐�
     void CreateItem();
private:
    unsigned int m_fadeColor = 0x000000;//�t�F�[�h�̐F�i��
    
    void (GameplayingScene::* m_updateFunc)(const InputState& input);
    void (GameplayingScene::* m_drawFunc)();

    std::shared_ptr<Player> m_player;
    std::shared_ptr<EnemyFactory> m_enemyFactory;
    std::shared_ptr<ItemFactory> m_itemFactory;
    std::shared_ptr<Map> m_map;
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<FileInformation> m_file;
    
    int m_soundVolume = 0;//�T�E���h�̉���
    
    int m_crea = 0;//�ǂ��Ɉړ����邩
    
    bool m_isHit = false;//�}�b�v�Ɠ����������ǂ���

    int m_selectStage;

    int m_fade;
    int m_fadetimer;

    int m_score;
};
