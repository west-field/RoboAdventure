#include <DxLib.h>
#include "EffekseerForDxLib.h"

#include "game.h"
#include "Util/Sound.h"
#include "InputState.h"
#include "Scene/SceneManager.h"
#include "Scene/TitleScene.h"
#include "Scene/DebugScene.h"

//int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	SetUseDirectDrawDeviceIndex(1);
	// window���[�h�ݒ�
	ChangeWindowMode(Game::kWindowMode);
	// �E�C���h�E���ݒ�
	SetMainWindowText(Game::kTitleText);
	// ��ʃT�C�Y�̐ݒ�
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorDepth);

	//DxLib�̏�����
	if (DxLib_Init() == -1)
	{
		return -1;
	}
	//�G�t�F�N�V�A�̏�����
	if (Effekseer_Init(8000) == -1)
	{
		return -1;
	}
	// �t���X�N���[���E�C���h�E�̐؂�ւ��Ń��\�[�X��������̂�h���B
	SetChangeScreenModeGraphicsSystemResetFlag(false);//(Dxlib
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();//(Effekseer
	//�`���𗠉�ʂɕύX����B
	SetDrawScreen(DX_SCREEN_BACK);

	//Z�o�b�t�@��L���ɂ���
	SetUseZBuffer3D(true);//�O��\����L���ɂ���
	//Z�o�b�t�@�ɏ������݂�L���ɂ���
	SetWriteZBuffer3D(true);
	//���ʃ|���S����\�����Ȃ�
	SetUseBackCulling(true);

	auto& soundManager = SoundManager::GetInstance();
	InputState input;
	SceneManager sceneManeger;
#if _DEBUG
	sceneManeger.ChangeScene(new DebugScene(sceneManeger));
#else
	sceneManeger.ChangeScene(new TitleScene(sceneManeger, true));
#endif
	sceneManeger.SetIsWindouMode(Game::kWindowMode);

	//���s0.1�`1000�܂ł��J�����̕`��͈͂Ƃ���
	SetCameraNearFar(1.0f, 1000.0f);
	// �J�����̈ʒu
	SetCameraPositionAndTarget_UpVecY(VGet(10.4f, 3.0f, -10.0f), VGet(10.4f, 3.0f, -9.0f));
	// �W�����C�g���|�C���g���C�g�ɂ���
	ChangeLightTypePoint(VGet(0.0f, 0.0f, 0.0f), 2000.0f, 0.0f, 0.002f, 0.0f);
	// �W�����C�g�̕�����z���̃v���X�����ɂ���
	SetLightDirection(VGet(0.0f, -10.0f, 1.0f));
	//���C�g�̈ʒu
	SetLightPosition(VAdd(VGet(10.0f, 3.0f, -10.0f), VGet(0.0f, 200.0f, -100.0f)));

	while (ProcessMessage() != -1)
	{
		LONGLONG  time = GetNowHiPerformanceCount();

		// DX���C�u�����̐ݒ��Effekseer�̐ݒ�𓯊�����B(��{�J����)(Effekseer)
		Effekseer_Sync3DSetting();

		input.Update();
		sceneManeger.Update(input);

		UpdateEffekseer3D();

		// ��ʂ�����������
		ClearDrawScreen();

		sceneManeger.Draw();

		// Effekseer�ɂ��Đ����̃G�t�F�N�g��`�悷��B
		DrawEffekseer3D();
		//����ʂ�\��ʂ����ւ���
		ScreenFlip();

		// esc�L�[����������I������
		//if (CheckHitKey(KEY_INPUT_ESCAPE))	break;
		//isEnd��true�̂Ƃ��I������
		if (sceneManeger.GetIsEnd())	break;
		// fps��60�ɌŒ�
		while (GetNowHiPerformanceCount() - time < 16667){}
	}

	soundManager.DeleteSound();

	//�G�t�F�N�V�A���I������
	Effkseer_End();
	//Dx���C�u�������I������
	DxLib_End();

	return 0;
}
