#include "GameclearScene.h"
#include <DxLib.h>
#include "../game.h"
#include "../Camera.h"
#include "../Model.h"
#include "../Util/Sound.h"
#include "../InputState.h"
#include "../Util/DrawFunctions.h"
#include "../Util/FileInformation.h"
#include "../Game/Map.h"
#include "../Game/Item/ItemCoin.h"
#include "SceneManager.h"
#include "TitleScene.h"


namespace
{
	constexpr float kMoveNum = 2.0f;//�v���C���[�ړ��X�s�[�h

	const wchar_t* const kFileName = L"Data/Img/Scene.fmf";//�g�p����}�b�v

	//�����̑傫��
	static constexpr int kFontSize = 75;
	static constexpr int kMaxFontSize = static_cast<int>(20 * 1.5f);
	//�E�B���h�E�̑傫��
	static constexpr int kWindowWidth = 600;
	static constexpr int kWindowHeight = 450;
	//�E�B���h�E�̃X�^�[�g�ʒu
	static constexpr int kWindowStartX = 300;
	static constexpr int kWindowStartY = 150;

	constexpr float kScale = 0.002f;//�\������v���C���[���f���̑傫��

	constexpr int kWaitingTime = 60 * 2;//���삪�ł���悤�ɂȂ�܂ł̎���

	constexpr int kHighScoreDrawPosY = 130;//�n�C�X�R�A��\������ʒuY
}

GameclearScene::GameclearScene(SceneManager& manager,int selectStage, int score, std::shared_ptr<Camera> camera, std::shared_ptr<Model> model, bool getCoin1, bool getCoin2, bool getCoin3) :
	Scene(manager), m_updateFunc(&GameclearScene::FadeInUpdat),
	m_drawFunc(&GameclearScene::NormalDraw) , m_camera(camera), m_model(model)
{
	m_camera->CameraPosInit();//�J�����̈ʒu��������

	m_selectStage = selectStage;//�I�����Ă����X�e�[�W
	
	m_file = std::make_shared<FileInformation>();
	m_file->Load();//�e�X�e�[�W�̏��擾

	m_file->Clear(selectStage, score, getCoin1, getCoin2, getCoin3);//�I�������X�e�[�W�A�X�R�A�A�X�^�[�l����
	m_file->Save();

	m_map = std::make_shared<Map>(kFileName,0.0f,0);//�Q�[���N���A�Ŏg�p����}�b�v

	m_model->SetAnimation(13, true, true);//���f���̃A�j���[�V������ύX
	m_model->SetPos(VGet(14.0f,-0.4f,0.0f));//���f���̈ʒu��ύX
	m_model->SetRot(VGet(0.0f,95.0f,0.0f));//���f���̉�]��ύX
	m_model->SetScale(VGet(kScale, kScale, kScale));//���f���̑傫����ύX

	//���̃X�e�[�W�̏����擾
	Header header = m_file->GetHeader(m_selectStage);
	m_score = score;//����̃X�R�A
	m_highScore = header.highScore;//�n�C�X�R�A
	//�X�^�[�擾��
	m_getCoin[0] = getCoin1;
	m_getCoin[1] = getCoin2;
	m_getCoin[2] = getCoin3;

	//�X�^�[���f��
	int handle1 = MV1LoadModel(L"Data/Model/Star.mv1");
	int handle2 = MV1LoadModel(L"Data/Model/Star_Outline.mv1");
	float x = 5.0f;//�\���ʒu
	int index = 1;//�X�^�[�̃C���f�b�N�X
	for (auto& star : m_starHandle)
	{
		if (m_getCoin[index - 1])
		{
			star = std::make_shared<ItemStar>(VGet(x, 5.0f, 0.0f), handle1, index);
		}
		else
		{
			star = std::make_shared<ItemStar>(VGet(x, 5.0f, 0.0f), handle2, index);
		}
		x += 3.0f;//�ړ�
		index++;
	}
	MV1DeleteModel(handle1);
	MV1DeleteModel(handle2);

	m_waitingTime = kWaitingTime;//���ɑ���ł���悤�ɂȂ�܂ł̎���

	//BGM
	m_BgmH = LoadSoundMem(L"Data/Sound/BGM/clearSound.mp3");
	ChangeVolumeSoundMem(0, m_BgmH);
	PlaySoundMem(m_BgmH, DX_PLAYTYPE_LOOP, true);
}

GameclearScene::~GameclearScene()
{
	DeleteSoundMem(m_BgmH);
	m_model->DeleteModel();
	m_model.reset();
	m_map.reset();
	for (auto& coin : m_starHandle)
	{
		coin.reset();
	}
}

void GameclearScene::Update(const InputState& input)
{
	m_map->Update();
	m_model->Update();
	(this->*m_updateFunc)(input);
}

void GameclearScene::Draw()
{
	m_map->Draw();
	(this->*m_drawFunc)();
	m_model->Draw();

	//�t�F�[�h�C���A�A�E�g�p
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeValue);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_fadeColor, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameclearScene::FadeInUpdat(const InputState& input)
{
	m_fadeValue = 255 * m_fadeTimer / kFadeInterval;//���񂾂񖾂邭
	ChangeVolumeSoundMem(SoundManager::GetInstance().GetBGMVolume() - m_fadeValue, m_BgmH);//BGM�����񂾂�傫������
	if (--m_fadeTimer == 0)
	{
		m_updateFunc = &GameclearScene::NormalUpdat;
		m_fadeValue = 0;
		SoundManager::GetInstance().Play(SoundId::Gameclear);//�Q�[���N���A�̉���炷
	}
}
void GameclearScene::FadeOutUpdat(const InputState& input)
{
	m_fadeValue = 255 * m_fadeTimer / kFadeInterval;//���񂾂�Â�
	ChangeVolumeSoundMem(SoundManager::GetInstance().GetBGMVolume() - m_fadeValue, m_BgmH);//BGM�����񂾂񏬂�������
	if (++m_fadeTimer == kFadeInterval)
	{
		m_manager.ChangeScene(new TitleScene(m_manager, false));
		StopSoundMem(m_BgmH);//��ʂ�ύX����ہABGM���~�߂�
		return;
	}
}

void GameclearScene::NormalUpdat(const InputState& input)
{
	//�X�^�[
	for (int i = 0;i<3;i++)
	{
		//�擾���Ă��Ȃ��Ƃ��͏������Ȃ�
		if (!m_getCoin[i])	continue;
		m_starHandle[i]->Update();
	}

	//�҂����Ԃ��߂��āAnext���������Ƃ�
	if (m_waitingTime-- <= 0 && input.IsTriggered(InputType::next))
	{
		m_fadeColor = 0x000000;//���F�Ńt�F�[�h�A�E�g����
		m_updateFunc = &GameclearScene::FadeOutUpdat;
		return;
	}
}

void GameclearScene::NormalDraw()
{
	//�X�R�A�\��
	for (int i = 0; i < 3; i++)
	{
		m_starHandle[i]->Draw();
	}

	int x = 300;
	//����̃X�R�A���n�C�X�R�A�̎�
	if (m_file->IsHighScore())
	{
		SetFontSize(kFontSize);
		DrawFormatString(x, Game::kScreenHeight / 2, 0xf0f000, L"�n�C�X�R�A : %d", m_highScore);
	}
	else
	{
		SetFontSize(kFontSize - 20);
		DrawFormatString(x, kHighScoreDrawPosY, 0xf0f000, L"�n�C�X�R�A : %d", m_highScore);
		SetFontSize(kFontSize);
		DrawFormatString(x, Game::kScreenHeight / 2, 0x00f0f0, L"�@�X�R�A : %d", m_score);
	}
	SetFontSize(0);
}