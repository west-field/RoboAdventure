#pragma once
#include "Secne.h"
#include <memory>

class Player;
class EnemyFactory;
class ItemFactory;
class Map;
class Camera;

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
     /// <summary>
     /// ��ʂ̃t�F�[�h�C��
     /// </summary>
     /// <param name="input">����</param>
     void FadeInUpdat(const InputState& input);
     /// <summary>
     /// ��ʂ̃t�F�[�h�A�E�g
     /// </summary>
     void FadeOutUpdat(const InputState& input);

     /// <summary>
     /// �ʏ�X�V
     /// </summary>
     void NormalUpdat(const InputState& input);
     /// <summary>
     /// �J�������v���C���[�ɋ߂Â���
     /// </summary>
     void MoveCameraCloser(const InputState& input);
     /// <summary>
     /// �Q�[���I�[�o�[�V�[���ɍs���O�̃A�N�V����
     /// </summary>
     void GameoverUpdate(const InputState& input);
     /// <summary>
     /// �Q�[���N���A�V�[���ɍs���O�̃A�N�V����
     /// </summary>
     void GameclearUpdate(const InputState& input);

     /// <summary>
     /// �ʏ�\��
     /// </summary>
     void NormalDraw();
     /// <summary>
     /// �Q�[���I�[�o�[�\��
     /// </summary>
     void GameoverDraw();
     /// <summary>
     /// �Q�[���N���A�\��
     /// </summary>
     void GameclearDraw();

     /// <summary>
     /// �v���C���[�Ƃ̓����蔻��
     /// </summary>
     void CollisionPlayer();

     /// <summary>
     /// �G�𐶐�����
     /// </summary>
     void CreateEnemy();

     /// <summary>
     /// �A�C�e���𐶐�
     /// </summary>
     void CreateItem();
private:
    void PointDraw(int leftX, int y, int dispNum, float size = 0.5f, int digit = -1);

    unsigned int m_fadeColor = 0x000000;//�t�F�[�h�̐F�i��
    
    void (GameplayingScene::* m_updateFunc)(const InputState& input);
    void (GameplayingScene::* m_drawFunc)();

    std::shared_ptr<Player> m_player;//�v���C���[
    std::shared_ptr<EnemyFactory> m_enemyFactory;//�G�H��
    std::shared_ptr<ItemFactory> m_itemFactory;//�A�C�e���H��
    std::shared_ptr<Map> m_map;//�}�b�v
    std::shared_ptr<Camera> m_camera;//�J����
    
    int m_soundVolume = 0;//�T�E���h�̉���
    
    int m_crea = 0;//�ǂ��Ɉړ����邩

    int m_selectStage;//�I�������X�e�[�W

    int m_fade;//�t�F�[�h
    int m_fadetimer;//�t�F�[�h�̎���

    int m_score;//�X�R�A
    int m_addScore;//1���X�R�A��\�������邽�߂Ɏ擾�����X�R�A��ێ����Ă���

    int m_starHandle;//�X�^�[��\������
    int m_starOutlineHandle;//�X�^�[�g��\������

    int m_numGraphHandle;//�����̉摜�n���h��
};

