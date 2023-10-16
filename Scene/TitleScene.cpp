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
	constexpr int kTitleFontSize = 80;//タイトル文字のサイズ
	constexpr int kFontSize = 50;//通常文字のサイズ

	constexpr float kGraphScale = 0.5f;//表示拡大率
	constexpr float kMaxScale = 1.0f;//表示拡大率

	constexpr int kTitleGraphW = 2480;//タイトル画像の横幅
	constexpr int kTitleGraphH = 570;//タイトル画像の縦幅

	const wchar_t* const kFileName = L"Data/Img/Title.fmf";//タイトルで使う3Dマップ

	const wchar_t* const kFirstFileName = L"Data/Img/Map1.fmf";//1ステージ目で使う3Dマップ名
	const wchar_t* const kSecondFileName = L"Data/Img/Map2.fmf";//2ステージ目で使う3Dマップ名
	const wchar_t* const kThirdFileName = L"Data/Img/Map3.fmf";//3ステージ目で使う3Dマップ名

	constexpr float kMaxCameraPos = 3.0f;//カメラ移動　最大値
	constexpr float kMinCameraPos = -9.0f;//カメラ移動　最小値
	const VECTOR kStageSelectPos = VGet(10.4f, kMinCameraPos, -10.0f);//ステージ選択時のカメラの位置
	constexpr float kCameraSpeed = 0.2f;//カメラが移動するときのスピード

	constexpr int kFadeMax = 255;//描画ブレンドのパラメータ最大(Palの値が255に近いほどグラフィックが濃く表示)
	constexpr int kFadeNoSelectGraph = 200;//選択不可なステージの画像を表示する時のブレンドパラメータ

	const wchar_t* const kTitleNext = L"Press Key or Button";//タイトルの下に表示する次へ促す文字

	constexpr int kStageSelectGraphSize = 512;//ステージ選択画像のサイズ
	constexpr int kDrawBoxSize = static_cast<int>(kStageSelectGraphSize * 0.5f) + 10;//画像の後ろに表示する四角のサイズ

	constexpr int kStarHandleSize = 65;//スター画像のサイズ
}


TitleScene::TitleScene(SceneManager& manager, bool start) : 
	Scene(manager), m_updateFunc(&TitleScene::FadeInUpdat), 
	m_isStart(start)
{
	//カメラ
	m_camera = std::make_shared<Camera>();
	//ゲーム開始して初めてタイトルを表示したとき
	if (m_isStart)
	{
		m_camera->CameraPosInit();//カメラの位置を初期化する
		m_drawFunc = &TitleScene::TitleRogoDraw;
	}
	else
	{
		m_camera->CameraPosInit(kStageSelectPos,true);//セレクト画面を表示できる位置に変更する
		m_drawFunc = &TitleScene::StageSelectDraw;//ステージセレクト画面から始める
	}

	m_titleRogoHandle = my::MyLoadGraph(L"Data/Img/title.png");//タイトルロゴ

	m_flashingTitleText = 0;//テキストを点滅させる
	m_flashingTitleTextTime = 5;//テキストを点滅させる時間

	//タイトルで使用するマップ
	m_map = std::make_shared<Map>(kFileName, 0.0f, 0);

	//ファイル
	m_file = std::make_shared<FileInformation>();
#if false
	m_file->Init();//各ステージ情報を初期化する
	m_file->Load();//各ステージ情報の取得
	Header header[static_cast<int>(StageSelect::stageNum)] = {};

	//ステージの数情報を取得
	for (int i = 0; i < static_cast<int>(StageSelect::stageNum); i++)
	{
		header[i] = m_file->GetHeader(i);//丸ごとコピー
		m_selectStage[i].isSelect = header[i].select;//選択できるかどうかをコピー
		m_selectStage[i].highScore = header[i].highScore;//ハイスコア

		//選択できるとき
		if (m_selectStage[i].isSelect)
		{
			m_selectStage[i].fade = kFadeMax;//MAX
		}
		else
		{
			m_selectStage[i].fade = kFadeNoSelectGraph;//すこし薄く
		}

		//ステージにある星の数
		for (int j = 0; j < 3; j++)
		{
			//すでにとっているかどうか
			m_selectStage[i].star[j].isGet = header[i].isCoinGet[j];
		}
	}
#else
	m_file->Load();//各ステージ情報の取得
	Header header[static_cast<int>(StageSelect::stageNum)] = {};
	
	//ステージの数情報を取得
	for (int i = 0; i < static_cast<int>(StageSelect::stageNum); i++)
	{
		header[i] = m_file->GetHeader(i);//丸ごとコピー
		m_selectStage[i].isSelect = header[i].select;//選択できるかどうかをコピー
		m_selectStage[i].highScore = header[i].highScore;//ハイスコア

		//選択できるとき
		if (m_selectStage[i].isSelect)
		{
			m_selectStage[i].fade = kFadeMax;//MAX
		}
		else
		{
			m_selectStage[i].fade = kFadeNoSelectGraph;//すこし薄く
		}

		//ステージにある星の数
		for (int j = 0; j < 3; j++)
		{
			//すでにとっているかどうか
			m_selectStage[i].star[j].isGet = header[i].isCoinGet[j];
		}
	}
#endif
	//ステージ選択をするときのステージ画像
	m_selectStage[StageSelect::First].handle = my::MyLoadGraph(L"Data/Img/stage1.jpg");//1ステージ目
	m_selectStage[StageSelect::Second].handle = my::MyLoadGraph(L"Data/Img/stage2.jpg");//２ステージ目
	m_selectStage[StageSelect::Third].handle = my::MyLoadGraph(L"Data/Img/stage3.jpg");//３ステージ目

	//ステージ画像を表示する位置
	int x = Game::kScreenWidth / (StageSelect::stageNum + 1);
	int y = Game::kScreenHeight / 2;

	int posX = x;//位置を移動させていくための変数
	for (auto& select : m_selectStage)
	{
		//位置を代入
		select.x = posX;
		select.y = y;

		//画像の拡大率を代入
		select.scale = kGraphScale;

		//スター表示位置を代入
		for (int j = 0; j < 3; j++)
		{
			//ステージ画像位置 * 拡大率 + スターのサイズ + スターの○番目の位置
			select.star[j].x = posX - static_cast<int>(kStageSelectGraphSize * kGraphScale) + kStarHandleSize + static_cast<int>(kStarHandleSize * j);
			select.star[j].y = y - static_cast<int>(kStageSelectGraphSize * kGraphScale)+ kStarHandleSize;
		}

		//右へ移動
		posX += x;
	}

	//最初に選択されているステージの場所だけ大きさを変える
	m_selectStage[StageSelect::First].scale = kMaxScale;

	//BGM
	m_BgmH = LoadSoundMem(L"Data/Sound/BGM/titleSound.mp3");
	ChangeVolumeSoundMem(0, m_BgmH);
	PlaySoundMem(m_BgmH, DX_PLAYTYPE_LOOP, true);

	//表示するスターの画像
	m_starHandle = my::MyLoadGraph(L"Data/Img/star.png");
	//表示するスター輪郭線の画像
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
	//メンバ関数ポインタを呼び出す
	(this->*m_updateFunc)(input);

	//タイトルロゴの下に表示する「Push Key or Button」の文字を点滅させる
	if (m_flashingTitleText < 0 || m_flashingTitleText > kFadeMax)
	{
		m_flashingTitleTextTime *= -1;
	}
	m_flashingTitleText += m_flashingTitleTextTime;

	//ポーズボタンが押されたら
	if (input.IsTriggered(InputType::pause))
	{
		SoundManager::GetInstance().Play(SoundId::MenuOpen);//ポーズ画面が開く音を鳴らす
		m_manager.PushScene(new SettingScene(m_manager, m_BgmH));
		return;
	}
}

void TitleScene::Draw()
{
	m_map->Draw();

	(this->*m_drawFunc)();

#ifdef _DEBUG
	//実際のカメラの位置
	VECTOR pos = GetCameraPosition();
	DrawFormatString(0, 0, 0x000000, L"  camera x.%lf,y.%lf,z.%lf", pos.x, pos.y, pos.z);
	//設定したカメラの位置
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
	m_fadeValue = kFadeMax * static_cast<int>(m_fadeTimer) / static_cast<int>(kFadeInterval);//どんどん明るくなる
	ChangeVolumeSoundMem(SoundManager::GetInstance().GetBGMVolume() - m_fadeValue, m_BgmH);//BGMの音をだんだん大きくする

	//フェードインし終わったら
	if (--m_fadeTimer == 0)
	{
		//初めてタイトル画面を表示したとき
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
	m_fadeValue = kFadeMax * static_cast<int>(m_fadeTimer) / static_cast<int>(kFadeInterval);//どんどん暗くなる
	ChangeVolumeSoundMem(SoundManager::GetInstance().GetBGMVolume() - m_fadeValue, m_BgmH);//BGMの音をだんだん小さくする
	if (++m_fadeTimer == kFadeInterval)
	{
		//選択しているステージへ移動する
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
		StopSoundMem(m_BgmH);//音を止める
		return;
	}
}

//タイトルロゴが表示されている時のアップデート関数
void TitleScene::NormalUpdat(const InputState& input)
{
	//「次へ」ボタンが押されたら次シーンへ移行する
	if (input.IsTriggered(InputType::next))
	{
		SoundManager::GetInstance().Play(SoundId::Determinant);	
		m_updateFunc = &TitleScene::MoveDropUpdate;
		m_drawFunc = &TitleScene::NoneDraw;
	}
}

//カメラを下に移動させる
void TitleScene::MoveDropUpdate(const InputState& input)
{
	//カメラの位置が-9.0fよりも下に移動したとき次へ
	if (GetCameraPosition().y < kMinCameraPos)
	{
		m_updateFunc = &TitleScene::StageSelectUpdate;
		m_drawFunc = &TitleScene::StageSelectDraw;
		return;
	}
	m_camera->moveUpdate(VGet(0.0f, -kCameraSpeed, 0.0f));
}

//カメラを上に移動させる
void TitleScene::MoveUpUpdate(const InputState& input)
{
	//カメラの位置が3.0fよりも上に移動したとき次へ
	if (GetCameraPosition().y > kMaxCameraPos)
	{
		m_updateFunc = &TitleScene::NormalUpdat;
		m_drawFunc = &TitleScene::TitleRogoDraw;
		return;
	}
	m_camera->moveUpdate(VGet(0.0f, kCameraSpeed, 0.0f));
}

//ステージセレクト
void TitleScene::StageSelectUpdate(const InputState& input)
{
	//メニュー
	bool isPress = false;//キーが押されたかどうか
	if (input.IsTriggered(InputType::left))
	{
		m_selectNum = (m_selectNum + (StageSelect::stageNum - 1)) % StageSelect::stageNum;//選択状態を一つ左へ
		isPress = true;
	}
	else if (input.IsTriggered(InputType::right))
	{
		m_selectNum = (m_selectNum + 1) % StageSelect::stageNum;//選択状態を一つ右へ
		isPress = true;
	}

	//キーを押した時
	if (isPress)
	{
		SoundManager::GetInstance().Play(SoundId::Cursor);//カーソル移動の音を鳴らす
		for (int i = 0; i < StageSelect::stageNum; i++)
		{
			//選択しているステージの時
			if (i == m_selectNum)
			{
				//大きく見せる
				m_selectStage[i].scale = kMaxScale;
			}
			else
			{
				//通常の大きさ
				m_selectStage[i].scale = kGraphScale;
			}
		}
	}

	//ボタンを押したら次のシーンへ移動する
	if (input.IsTriggered(InputType::next))
	{
		//選択できるステージを選んでいたら
		if (m_selectStage[m_selectNum].isSelect)
		{
			SoundManager::GetInstance().Play(SoundId::Determinant);//決定音を鳴らす
			m_updateFunc = &TitleScene::FadeOutUpdat;
			return;
		}
	}
	//「戻る」ボタンが押されたら
	if (input.IsTriggered(InputType::prev))
	{
		SoundManager::GetInstance().Play(SoundId::Back);//戻る音を鳴らす
		m_updateFunc = &TitleScene::MoveUpUpdate;//カメラを上に移動させる
		m_drawFunc = &TitleScene::NoneDraw;
		return;
	}
}

void TitleScene::TitleRogoDraw()
{
	//タイトルロゴ表示
	my::MyDrawRectRotaGraph((Game::kScreenWidth / 2), Game::kScreenHeight / 3, 0, 0, kTitleGraphW, kTitleGraphH, 0.5f, 0.0f, m_titleRogoHandle, true, false);

	//テキスト表示
	SetFontSize(kFontSize);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_flashingTitleText);
	DrawString(((Game::kScreenWidth / 2) - (kFontSize * 9 / 2)), Game::kScreenHeight - kTitleFontSize * 2, kTitleNext, 0xffffff);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetFontSize(0);
}

void TitleScene::NoneDraw()
{
	//何も表示しない
}

void TitleScene::StageSelectDraw()
{
	//ステージ画像を表示する
	int stageNum = 0;
	for (auto& selectStage : m_selectStage)
	{
		if (stageNum == m_selectNum)
		{
			//選択している位置の枠を表示する
			DrawBox(selectStage.x - kDrawBoxSize, selectStage.y - kDrawBoxSize, selectStage.x + kDrawBoxSize, selectStage.y + kDrawBoxSize, 0xff0000, true);
		}
		//画像を表示
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, selectStage.fade);
		my::MyDrawRectRotaGraph(selectStage.x, selectStage.y, 0, 0, kStageSelectGraphSize, kStageSelectGraphSize, selectStage.scale, 0.0f, selectStage.handle, false, false);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		//ステージを選択できないときは処理をしない
		if (!selectStage.isSelect) 
		{
			stageNum++;
			continue;
		}

		if (stageNum == m_selectNum)
		{
			//ステージを選択している時、ハイスコアを表示
			SetFontSize(kFontSize);
			DrawFormatString(selectStage.x - kFontSize * 5, selectStage.y + kFontSize * 4, 0xf0f000, L"ハイスコア　%d", selectStage.highScore);
			//スターを表示
			for (int j = 0; j < 3; j++)
			{
				//取得している時
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

	//テキストを表示
	SetFontSize(kFontSize);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_flashingTitleText);
	DrawString(((Game::kScreenWidth / 2) - (kFontSize * 9 / 2)), Game::kScreenHeight - kTitleFontSize * 2, kTitleNext, 0xffffff);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetFontSize(0);
}
