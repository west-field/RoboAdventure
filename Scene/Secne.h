#pragma once

//プロトタイプ宣言
class SceneManager;//シーンマネージャー
class InputState;//入力ステート

/// <summary>
/// シーン基底クラス//大基になっている
/// </summary>
class Scene
{
protected:
	SceneManager& m_manager;//シーンマネージャーへの参照
	
	static constexpr int kFadeInterval = 60;
	int m_fadeTimer = kFadeInterval;//フェードタイマー
	int m_fadeValue = 255;//黒矩形とのブレンド具合
	int m_BgmH = -1;
public:
	Scene(SceneManager& manager) :m_manager(manager){}
	virtual ~Scene() {}
	//シーンを更新する
	virtual void Update(const InputState& input) = 0;
	//シーンを描画する
	virtual void Draw() = 0;
};

