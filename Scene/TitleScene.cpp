#include "TitleScene.h"
#include <DxLib.h>

#include "../game.h"
#include "../Camera.h"

#include "../Util/Sound.h"
#include "../InputState.h"
#include "../Util/DrawFunctions.h"
#include "../Util/FileInformation.h"
#include "../Game/Map.h"

#include "SceneManager.h"
#include "GameplayingScene.h"
#include "SettingScene.h"

namespace
{
	constexpr int kTitleFontSize = 80;//�^�C�g�������̃T�C�Y
	constexpr int kFontSize = 50;//�ʏ핶���̃T�C�Y

	constexpr float kGraphScale = 0.5f;//�\���g�嗦
	constexpr float kMaxScale = 1.0f;//�\���g�嗦

	constexpr int kTitleGraphW = 2480;//�^�C�g���摜�̉���
	constexpr int kTitleGraphH = 570;//�^�C�g���摜�̏c��

	const wchar_t* const kFileName = L"Data/Img/Title.fmf";//�^�C�g���Ŏg��3D�}�b�v

	const wchar_t* const kFirstFileName = L"Data/Img/Map1.fmf";//1�X�e�[�W�ڂŎg��3D�}�b�v��
	const wchar_t* const kSecondFileName = L"Data/Img/Map2.fmf";//2�X�e�[�W�ڂŎg��3D�}�b�v��
	const wchar_t* const kThirdFileName = L"Data/Img/Map3.fmf";//3�X�e�[�W�ڂŎg��3D�}�b�v��

	constexpr float kMaxCameraPos = 3.0f;//�J�����ړ��@�ő�l
	constexpr float kMinCameraPos = -9.0f;//�J�����ړ��@�ŏ��l
	const VECTOR kStageSelectPos = VGet(10.4f, kMinCameraPos, -10.0f);//�X�e�[�W�I�����̃J�����̈ʒu
	constexpr float kCameraSpeed = 0.2f;//�J�������ړ�����Ƃ��̃X�s�[�h

	constexpr int kFadeMax = 255;//�`��u�����h�̃p�����[�^�ő�(Pal�̒l��255�ɋ߂��قǃO���t�B�b�N���Z���\��)
	constexpr int kFadeNoSelectGraph = 200;//�I��s�ȃX�e�[�W�̉摜��\�����鎞�̃u�����h�p�����[�^

	const wchar_t* const kTitleNext = L"Press Key or Button";//�^�C�g���̉��ɕ\�����鎟�֑�������

	constexpr int kStageSelectGraphSize = 512;//�X�e�[�W�I���摜�̃T�C�Y
	constexpr int kDrawBoxSize = static_cast<int>(kStageSelectGraphSize * 0.5f) + 10;//�摜�̌��ɕ\������l�p�̃T�C�Y

	constexpr int kStarHandleSize = 65;//�X�^�[�摜�̃T�C�Y
}


TitleScene::TitleScene(SceneManager& manager, bool start) : 
	Scene(manager), m_updateFunc(&TitleScene::FadeInUpdat), 
	m_isStart(start)
{
	//�J����
	m_camera = std::make_shared<Camera>();
	//�Q�[���J�n���ď��߂ă^�C�g����\�������Ƃ�
	if (m_isStart)
	{
		m_camera->CameraPosInit();//�J�����̈ʒu������������
		m_drawFunc = &TitleScene::TitleRogoDraw;
	}
	else
	{
		m_camera->CameraPosInit(kStageSelectPos,true);//�Z���N�g��ʂ�\���ł���ʒu�ɕύX����
		m_drawFunc = &TitleScene::StageSelectDraw;//�X�e�[�W�Z���N�g��ʂ���n�߂�
	}

	m_titleRogoHandle = my::MyLoadGraph(L"Data/Img/title.png");//�^�C�g�����S

	m_flashingTitleText = 0;//�e�L�X�g��_�ł�����
	m_flashingTitleTextTime = 5;//�e�L�X�g��_�ł����鎞��

	//�^�C�g���Ŏg�p����}�b�v
	m_map = std::make_shared<Map>(kFileName, 0.0f, 0);

	//�t�@�C��
	m_file = std::make_shared<FileInformation>();
#if false
	m_file->Init();//�e�X�e�[�W��������������
	m_file->Load();//�e�X�e�[�W���̎擾
	Header header[static_cast<int>(StageSelect::stageNum)] = {};

	//�X�e�[�W�̐������擾
	for (int i = 0; i < static_cast<int>(StageSelect::stageNum); i++)
	{
		header[i] = m_file->GetHeader(i);//�ۂ��ƃR�s�[
		m_selectStage[i].isSelect = header[i].select;//�I���ł��邩�ǂ������R�s�[
		m_selectStage[i].highScore = header[i].highScore;//�n�C�X�R�A

		//�I���ł���Ƃ�
		if (m_selectStage[i].isSelect)
		{
			m_selectStage[i].fade = kFadeMax;//MAX
		}
		else
		{
			m_selectStage[i].fade = kFadeNoSelectGraph;//����������
		}

		//�X�e�[�W�ɂ��鐯�̐�
		for (int j = 0; j < 3; j++)
		{
			//���łɂƂ��Ă��邩�ǂ���
			m_selectStage[i].star[j].isGet = header[i].isCoinGet[j];
		}
	}
#else
	m_file->Load();//�e�X�e�[�W���̎擾
	Header header[static_cast<int>(StageSelect::stageNum)] = {};
	
	//�X�e�[�W�̐������擾
	for (int i = 0; i < static_cast<int>(StageSelect::stageNum); i++)
	{
		header[i] = m_file->GetHeader(i);//�ۂ��ƃR�s�[
		m_selectStage[i].isSelect = header[i].select;//�I���ł��邩�ǂ������R�s�[
		m_selectStage[i].highScore = header[i].highScore;//�n�C�X�R�A

		//�I���ł���Ƃ�
		if (m_selectStage[i].isSelect)
		{
			m_selectStage[i].fade = kFadeMax;//MAX
		}
		else
		{
			m_selectStage[i].fade = kFadeNoSelectGraph;//����������
		}

		//�X�e�[�W�ɂ��鐯�̐�
		for (int j = 0; j < 3; j++)
		{
			//���łɂƂ��Ă��邩�ǂ���
			m_selectStage[i].star[j].isGet = header[i].isCoinGet[j];
		}
	}
#endif
	//�X�e�[�W�I��������Ƃ��̃X�e�[�W�摜
	m_selectStage[StageSelect::First].handle = my::MyLoadGraph(L"Data/Img/stage1.jpg");//1�X�e�[�W��
	m_selectStage[StageSelect::Second].handle = my::MyLoadGraph(L"Data/Img/stage2.jpg");//�Q�X�e�[�W��
	m_selectStage[StageSelect::Third].handle = my::MyLoadGraph(L"Data/Img/stage3.jpg");//�R�X�e�[�W��

	//�X�e�[�W�摜��\������ʒu
	int x = Game::kScreenWidth / (StageSelect::stageNum + 1);
	int y = Game::kScreenHeight / 2;

	int posX = x;//�ʒu���ړ������Ă������߂̕ϐ�
	for (auto& select : m_selectStage)
	{
		//�ʒu����
		select.x = posX;
		select.y = y;

		//�摜�̊g�嗦����
		select.scale = kGraphScale;

		//�X�^�[�\���ʒu����
		for (int j = 0; j < 3; j++)
		{
			//�X�e�[�W�摜�ʒu * �g�嗦 + �X�^�[�̃T�C�Y + �X�^�[�́��Ԗڂ̈ʒu
			select.star[j].x = posX - static_cast<int>(kStageSelectGraphSize * kGraphScale) + kStarHandleSize + static_cast<int>(kStarHandleSize * j);
			select.star[j].y = y - static_cast<int>(kStageSelectGraphSize * kGraphScale)+ kStarHandleSize;
		}

		//�E�ֈړ�
		posX += x;
	}

	//�ŏ��ɑI������Ă���X�e�[�W�̏ꏊ�����傫����ς���
	m_selectStage[StageSelect::First].scale = kMaxScale;

	//BGM
	m_BgmH = LoadSoundMem(L"Data/Sound/BGM/titleSound.mp3");
	ChangeVolumeSoundMem(0, m_BgmH);
	PlaySoundMem(m_BgmH, DX_PLAYTYPE_LOOP, true);

	//�\������X�^�[�̉摜
	m_starHandle = my::MyLoadGraph(L"Data/Img/star.png");
	//�\������X�^�[�֊s���̉摜
	m_starOutlineHandle = my::MyLoadGraph(L"Data/Img/star_outline.png");
}

TitleScene::~TitleScene()
{
	DeleteSoundMem(m_BgmH);
	DeleteGraph(m_titleRogoHandle);
	DeleteGraph(m_starHandle);
	DeleteGraph(m_starOutlineHandle);
	m_map.reset();

	for (auto& graph : m_selectStage)
	{
		DeleteGraph(graph.handle);
	}
}

void
TitleScene::Update(const InputState& input)
{
	m_map->Update();
	//�����o�֐��|�C���^���Ăяo��
	(this->*m_updateFunc)(input);

	//�^�C�g�����S�̉��ɕ\������uPush Key or Button�v�̕�����_�ł�����
	if (m_flashingTitleText < 0 || m_flashingTitleText > kFadeMax)
	{
		m_flashingTitleTextTime *= -1;
	}
	m_flashingTitleText += m_flashingTitleTextTime;

	//�|�[�Y�{�^���������ꂽ��
	if (input.IsTriggered(InputType::pause))
	{
		SoundManager::GetInstance().Play(SoundId::MenuOpen);//�|�[�Y��ʂ��J������炷
		m_manager.PushScene(new SettingScene(m_manager, m_BgmH));
		return;
	}
}

void TitleScene::Draw()
{
	m_map->Draw();

	(this->*m_drawFunc)();

#ifdef _DEBUG
	//���ۂ̃J�����̈ʒu
	VECTOR pos = GetCameraPosition();
	DrawFormatString(0, 0, 0x000000, L"  camera x.%lf,y.%lf,z.%lf", pos.x, pos.y, pos.z);
	//�ݒ肵���J�����̈ʒu
	VECTOR cameraPos = m_camera->GetPos();
	DrawFormatString(0, 20, 0x000000, L"m_camera x.%lf,y.%lf,z.%lf", cameraPos.x, cameraPos.y, cameraPos.z);

	DrawFormatString(0, 40, 0x000000, L"select.%d", m_selectNum);
#endif

	//FadeIn,Out
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeValue);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void TitleScene::FadeInUpdat(const InputState& input)
{
	m_fadeValue = kFadeMax * static_cast<int>(m_fadeTimer) / static_cast<int>(kFadeInterval);//�ǂ�ǂ񖾂邭�Ȃ�
	ChangeVolumeSoundMem(SoundManager::GetInstance().GetBGMVolume() - m_fadeValue, m_BgmH);//BGM�̉������񂾂�傫������

	//�t�F�[�h�C�����I�������
	if (--m_fadeTimer == 0)
	{
		//���߂ă^�C�g����ʂ�\�������Ƃ�
		if (m_isStart)
		{
			m_updateFunc = &TitleScene::NormalUpdat;
		}
		else
		{
			m_updateFunc = &TitleScene::StageSelectUpdate;
		}
		m_fadeValue = 0;
	}
}
void TitleScene::FadeOutUpdat(const InputState& input)
{
	m_fadeValue = kFadeMax * static_cast<int>(m_fadeTimer) / static_cast<int>(kFadeInterval);//�ǂ�ǂ�Â��Ȃ�
	ChangeVolumeSoundMem(SoundManager::GetInstance().GetBGMVolume() - m_fadeValue, m_BgmH);//BGM�̉������񂾂񏬂�������
	if (++m_fadeTimer == kFadeInterval)
	{
		//�I�����Ă���X�e�[�W�ֈړ�����
		switch (m_selectNum)
		{
		case StageSelect::First:
			m_manager.ChangeScene(new GameplayingScene(m_manager, kFirstFileName, m_selectNum, m_camera));
			break;
		case StageSelect::Second:
			m_manager.ChangeScene(new GameplayingScene(m_manager, kSecondFileName, m_selectNum, m_camera));
			break;
		case StageSelect::Third:
			m_manager.ChangeScene(new GameplayingScene(m_manager, kThirdFileName, m_selectNum, m_camera));
			break;
		default:
			break;
		}
		StopSoundMem(m_BgmH);//�����~�߂�
		return;
	}
}

//�^�C�g�����S���\������Ă��鎞�̃A�b�v�f�[�g�֐�
void TitleScene::NormalUpdat(const InputState& input)
{
	//�u���ցv�{�^���������ꂽ�玟�V�[���ֈڍs����
	if (input.IsTriggered(InputType::next))
	{
		SoundManager::GetInstance().Play(SoundId::Determinant);	
		m_updateFunc = &TitleScene::MoveDropUpdate;
		m_drawFunc = &TitleScene::NoneDraw;
	}
}

//�J���������Ɉړ�������
void TitleScene::MoveDropUpdate(const InputState& input)
{
	//�J�����̈ʒu��-9.0f�������Ɉړ������Ƃ�����
	if (GetCameraPosition().y < kMinCameraPos)
	{
		m_updateFunc = &TitleScene::StageSelectUpdate;
		m_drawFunc = &TitleScene::StageSelectDraw;
		return;
	}
	m_camera->moveUpdate(VGet(0.0f, -kCameraSpeed, 0.0f));
}

//�J��������Ɉړ�������
void TitleScene::MoveUpUpdate(const InputState& input)
{
	//�J�����̈ʒu��3.0f������Ɉړ������Ƃ�����
	if (GetCameraPosition().y > kMaxCameraPos)
	{
		m_updateFunc = &TitleScene::NormalUpdat;
		m_drawFunc = &TitleScene::TitleRogoDraw;
		return;
	}
	m_camera->moveUpdate(VGet(0.0f, kCameraSpeed, 0.0f));
}

//�X�e�[�W�Z���N�g
void TitleScene::StageSelectUpdate(const InputState& input)
{
	//���j���[
	bool isPress = false;//�L�[�������ꂽ���ǂ���
	if (input.IsTriggered(InputType::left))
	{
		m_selectNum = (m_selectNum + (StageSelect::stageNum - 1)) % StageSelect::stageNum;//�I����Ԃ������
		isPress = true;
	}
	else if (input.IsTriggered(InputType::right))
	{
		m_selectNum = (m_selectNum + 1) % StageSelect::stageNum;//�I����Ԃ���E��
		isPress = true;
	}

	//�L�[����������
	if (isPress)
	{
		SoundManager::GetInstance().Play(SoundId::Cursor);//�J�[�\���ړ��̉���炷
		for (int i = 0; i < StageSelect::stageNum; i++)
		{
			//�I�����Ă���X�e�[�W�̎�
			if (i == m_selectNum)
			{
				//�傫��������
				m_selectStage[i].scale = kMaxScale;
			}
			else
			{
				//�ʏ�̑傫��
				m_selectStage[i].scale = kGraphScale;
			}
		}
	}

	//�{�^�����������玟�̃V�[���ֈړ�����
	if (input.IsTriggered(InputType::next))
	{
		//�I���ł���X�e�[�W��I��ł�����
		if (m_selectStage[m_selectNum].isSelect)
		{
			SoundManager::GetInstance().Play(SoundId::Determinant);//���艹��炷
			m_updateFunc = &TitleScene::FadeOutUpdat;
			return;
		}
	}
	//�u�߂�v�{�^���������ꂽ��
	if (input.IsTriggered(InputType::prev))
	{
		SoundManager::GetInstance().Play(SoundId::Back);//�߂鉹��炷
		m_updateFunc = &TitleScene::MoveUpUpdate;//�J��������Ɉړ�������
		m_drawFunc = &TitleScene::NoneDraw;
		return;
	}
}

void TitleScene::TitleRogoDraw()
{
	//�^�C�g�����S�\��
	my::MyDrawRectRotaGraph((Game::kScreenWidth / 2), Game::kScreenHeight / 3, 0, 0, kTitleGraphW, kTitleGraphH, 0.5f, 0.0f, m_titleRogoHandle, true, false);

	//�e�L�X�g�\��
	SetFontSize(kFontSize);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_flashingTitleText);
	DrawString(((Game::kScreenWidth / 2) - (kFontSize * 9 / 2)), Game::kScreenHeight - kTitleFontSize * 2, kTitleNext, 0xffffff);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetFontSize(0);
}

void TitleScene::NoneDraw()
{
	//�����\�����Ȃ�
}

void TitleScene::StageSelectDraw()
{
	//�X�e�[�W�摜��\������
	int stageNum = 0;
	for (auto& selectStage : m_selectStage)
	{
		if (stageNum == m_selectNum)
		{
			//�I�����Ă���ʒu�̘g��\������
			DrawBox(selectStage.x - kDrawBoxSize, selectStage.y - kDrawBoxSize, selectStage.x + kDrawBoxSize, selectStage.y + kDrawBoxSize, 0xff0000, true);
		}
		//�摜��\��
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, selectStage.fade);
		my::MyDrawRectRotaGraph(selectStage.x, selectStage.y, 0, 0, kStageSelectGraphSize, kStageSelectGraphSize, selectStage.scale, 0.0f, selectStage.handle, false, false);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		//�X�e�[�W��I���ł��Ȃ��Ƃ��͏��������Ȃ�
		if (!selectStage.isSelect) 
		{
			stageNum++;
			continue;
		}

		if (stageNum == m_selectNum)
		{
			//�X�e�[�W��I�����Ă��鎞�A�n�C�X�R�A��\��
			SetFontSize(kFontSize);
			DrawFormatString(selectStage.x - kFontSize * 5, selectStage.y + kFontSize * 4, 0xf0f000, L"�n�C�X�R�A�@%d", selectStage.highScore);
			//�X�^�[��\��
			for (int j = 0; j < 3; j++)
			{
				//�擾���Ă��鎞
				if (selectStage.star[j].isGet)
				{
					my::MyDrawGraph(selectStage.star[j].x, selectStage.star[j].y, m_starHandle, true);
				}
				else
				{
					my::MyDrawGraph(selectStage.star[j].x, selectStage.star[j].y, m_starOutlineHandle, true);
				}
			}
		}
		
		stageNum++;
	}

	//�e�L�X�g��\��
	SetFontSize(kFontSize);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_flashingTitleText);
	DrawString(((Game::kScreenWidth / 2) - (kFontSize * 9 / 2)), Game::kScreenHeight - kTitleFontSize * 2, kTitleNext, 0xffffff);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetFontSize(0);
}
