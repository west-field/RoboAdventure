#pragma once
#include "Secne.h"
#include <memory>

class DebugScene : public Scene
{
public:
	DebugScene(SceneManager& manager);
	virtual ~DebugScene();

	void Update(const InputState& input);

	void Draw();

private:
	//画面のフェードイン
	void FadeInUpdat(const InputState& input);
	//画面のフェードアウト
	void FadeOutUpdat(const InputState& input);

	//通常更新
	void NormalUpdat(const InputState& input);
	//ステージ変更
	void StageSelect(const InputState& input);

	unsigned int m_fadeColor = 0x000000;//フェードの色（黒

	void (DebugScene::* m_updateFunc)(const InputState& input);



	enum class SelectScene
	{
		Title,
		Gameplaying,
		SelectStage,

		SelectMax
	};
	int m_select;//次へ行くシーンを選択する

	int m_stage;//ステージを選択する0から2(3ステージ)

	int m_drawY;//選択している位置に矢印を表示する

	int m_color[static_cast<int>(SelectScene::SelectMax)];//色
};

