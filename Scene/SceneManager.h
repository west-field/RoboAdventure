#pragma once

#include <deque>

//プロトタイプ宣言
class Scene;//シーン基底クラス
class InputState;//入力ステート

/// <summary>
/// シーン管理クラス
/// </summary>
class SceneManager
{
private:
	//今実行中のシーンを切り替えていきたいので参照ではなくポインタとして宣言
	std::deque<Scene*> m_scenes;
	//ゲームを終了させる
	bool m_isEnd = false;
	bool m_isWindouwMode;
public:
	/// <summary>
	/// シーンの切り替えを行う
	/// </summary>
	/// <param name="scene">切り替えたい次シーンのアドレス</param>
	void ChangeScene(Scene* scene);

	/// <summary>
	/// シーンを上に積む
	/// Updateで実行されるのは上に積まれたシーンのみ
	/// </summary>
	void PushScene(Scene* scene);
	
	/// <summary>
	/// 一番上のシーンを削除する
	/// ただし、その結果スタックが空になることのないように
	/// </summary>
	void PopScene();

	/// <summary>
	/// 各シーンの更新を行う
	/// </summary>
	/// <param name="input">入力ステート</param>
	void Update(const InputState& input);
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void SetIsEnd() { m_isEnd = true; }
	bool GetIsEnd() const { return m_isEnd; }
	
	bool GetIsWindouMode()const { return m_isWindouwMode; }
	void SetIsWindouMode(bool isWindou) { m_isWindouwMode = isWindou; }
};

