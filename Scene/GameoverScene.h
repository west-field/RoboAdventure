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
class FileInformation;

/// <summary>
/// �Q�[���I�[�o�[�V�[��
/// </summary>
class GameoverScene : public Scene
{
public:
    GameoverScene(SceneManager& manager, const int selectStage, const int score, std::shared_ptr<Camera> camera, std::shared_ptr<Model> model);
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

    unsigned int m_fadeColor;//�t�F�[�h�̐F�i��

    int  m_soundVolume = 0;

    int m_selectNum = 0;//�I�����Ă��郁�j���[����

    std::shared_ptr<Map> m_map;
    std::shared_ptr<Camera> m_camera;

    std::shared_ptr<Model> m_model;//�\������v���C���[���f�����f��

    std::shared_ptr<FileInformation> m_file;//���̃X�e�[�W�֍s����悤��

    std::shared_ptr<ItemStar> m_starHandle[3];//���W���\��
    bool m_getStar[3];//�擾��
    int m_score;//�X�R�A
};

