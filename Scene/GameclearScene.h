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
/// �Q�[���N���A�V�[��
/// </summary>
class GameclearScene : public Scene
{
public:
    GameclearScene(SceneManager& manager,const int selectStage,const int score, std::shared_ptr<Camera> camera, std::shared_ptr<Model> model, bool getCoin1, bool getCoin2, bool getCoin3);
    virtual ~GameclearScene();

    void Update(const InputState& input);
    void Draw();
private:
    unsigned int m_fadeColor = 0x000000;//�t�F�[�h�̐F�i��

    //�t�F�[�h�C��
    void FadeInUpdat(const InputState& input);
    //�t�F�[�h�A�E�g
    void FadeOutUpdat(const InputState& input);

    //�ʏ�X�V
    void NormalUpdat(const InputState& input);

    //�ʏ�\��
    void NormalDraw();

    void (GameclearScene::* m_updateFunc)(const InputState&);
    void (GameclearScene::* m_drawFunc)();

    int m_soundVolume = 0;//���̑傫��

    std::shared_ptr<FileInformation> m_file;//�t�@�C��

    std::shared_ptr<Map> m_map;//�}�b�v
    std::shared_ptr<Camera> m_camera;//�J����

    std::shared_ptr<Model> m_model;//�\������v���C���[���f�����f��

    std::shared_ptr<ItemStar> m_starHandle[3];//���W���\��
    bool m_getCoin[3];//�擾��

    int m_waitingTime;//�҂����� ������ł���悤�ɂȂ�܂ł̎���

    int m_selectStage;//�I�����Ă���X�e�[�W

    int m_score;//�X�R�A
    int m_highScore;//�n�C�X�R�A
};

