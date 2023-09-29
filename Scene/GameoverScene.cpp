#include "GameoverScene.h"
#include <DxLib.h>
#include "../game.h"
#include "../Camera.h"
#include "../Model.h"
#include "../Game/Map.h"
#include "../Util/Sound.h"
#include "../InputState.h"
#include "../Util/DrawFunctions.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "GameplayingScene.h"
#include "../Game/Item/ItemCoin.h"

namespace
{
	constexpr float kMoveNum = 2.0f;//�v���C���[�ړ��X�s�[�h
	constexpr int kMojiSize = 90;//�����̑傫��

	const wchar_t* const kFileName = L"Data/Img/Scene.fmf";//�g�p����}�b�v

	constexpr float kScale = 0.002f;//�\������v���C���[���f���̑傫��

	constexpr int kFontSize = 75;
}

GameoverScene::GameoverScene(SceneManager& manager, const int selectStage, const int score, std::shared_ptr<Camera> camera, std::shared_ptr<Model> model, bool getCoin1, bool getCoin2, bool getCoin3) :
	Scene(manager),  m_updateFunc(&GameoverScene::FadeInUpdat),
	m_drawFunc(&GameoverScene::NormalDraw) , m_camera(camera), m_model(model)
{
	m_fadeColor = 0xff0000;//�t�F�[�h�̐F�i��

	m_camera->CameraPosInit();//�J�����̈ʒu��������
	
	m_map = std::make_shared<Map>(kFileName, 0.0f, 0);//�Q�[���I�[�o�[�Ŏg�p����}�b�v

	//���f���X�V
	m_model->SetAnimation(4, true, true);
	m_model->SetPos(VGet(14.0f, -0.4f, 0.0f));
	m_model->SetRot(VGet(0.0f, 95.0f, 0.0f));
	m_model->SetScale(VGet(kScale, kScale, kScale));

	m_score = score;
	//�X�^�[�擾��
	m_getStar[0] = getCoin1;
	m_getStar[1] = getCoin2;
	m_getStar[2] = getCoin3;

	//�X�^�[���f��
	int handle1 = MV1LoadModel(L"Data/Model/Star.mv1");
	int handle2 = MV1LoadModel(L"Data/Model/Star_Outline.mv1");
	float x = 5.0f;
	int index = 1;
	//�X�^�[
	for (auto& star : m_starHandle)
	{
		if (m_getStar[index - 1])
		{
			star = std::make_shared<ItemStar>(VGet(x, 5.0f, 0.0f), handle1, index);
		}
		else
		{
			star = std::make_shared<ItemStar>(VGet(x, 5.0f, 0.0f), handle2, index);
		}
		x += 3.0f;
		index++;
	}
	MV1DeleteModel(handle1);
	MV1DeleteModel(handle2);

	m_BgmH = LoadSoundMem(L"Data/Sound/BGM/gameoverSound.mp3");
	ChangeVolumeSoundMem(0, m_BgmH);
	PlaySoundMem(m_BgmH, DX_PLAYTYPE_LOOP, true);
}

GameoverScene::~GameoverScene()
{
	DeleteSoundMem(m_BgmH);
	m_map.reset();
	m_model->DeleteModel();
	m_model.reset();
}

void
GameoverScene::Update(const InputState& input)
{
	m_map->Update();
	m_model->Update();
	(this->*m_updateFunc)(input);
}

void
GameoverScene::Draw()
{
	m_map->Draw();
	(this->*m_drawFunc)();
	m_model->Draw();
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeValue);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameoverScene::FadeInUpdat(const InputState& input)
{
	m_fadeValue = 255 * m_fadeTimer / kFadeInterval;//���񂾂񖾂邭
	ChangeVolumeSoundMem(SoundManager::GetInstance().GetBGMVolume() - m_fadeValue, m_BgmH);//���񂾂�傫��
	if (--m_fadeTimer == 0)
	{
		m_updateFunc = &GameoverScene::NormalUpdat;
		m_fadeColor = 0x000000;//���F�Ńt�F�[�h�A�E�g����
		m_fadeValue = 0;
	}
}

void GameoverScene::FadeOutUpdat(const InputState& input)
{
	m_fadeValue = 255 * m_fadeTimer / kFadeInterval;//���񂾂�Â�
	ChangeVolumeSoundMem(SoundManager::GetInstance().GetBGMVolume() - m_fadeValue, m_BgmH);//���񂾂񏬂���
	if (++m_fadeTimer == kFadeInterval)
	{
		m_manager.ChangeScene(new TitleScene(m_manager, false));
		StopSoundMem(m_BgmH);//�����~�߂�
		return;
	}
}

void GameoverScene::NormalUpdat(const InputState& input)
{
	//�X�^�[
	for (int i = 0; i < 3; i++)
	{
		if (!m_getStar[i])	continue;
		m_starHandle[i]->Update();
	}

	if (input.IsTriggered(InputType::next))
	{
		m_updateFunc = &GameoverScene::FadeOutUpdat;
		return;
	}
}

void GameoverScene::NormalDraw()
{
	//�X�^�[�\��
	for (int i = 0; i < 3; i++)
	{
		m_starHandle[i]->Draw();
	}
	//�X�R�A�\��
	SetFontSize(kFontSize);
	DrawFormatString(300, Game::kScreenHeight / 2, 0x00f0f0, L"�@�X�R�A : %d", m_score);
	SetFontSize(0);
}
