#pragma once
#include "DxLib.h"

class Model
{
public:
	/// <summary>
	/// コンストラクタ
	/// ファイル名を指定してロードを行う
	/// </summary>
	/// <param name="fileName">モデルファイル名</param>
	/// <param name="scale">大きさを指定</param>
	/// 	/// <param name="useCollision">コリジョンをしようするかどうか</param>
	Model(const TCHAR* fileName, float scale, bool useCollision);
	/// <summary>
	/// コンストラクタ
	/// 指定されたハンドルのモデルをDuplicateして生成する
	/// </summary>
	/// <param name="orgModel">モデルハンドル</param>
	/// <param name="scale">大きさを指定</param>
	/// <param name="useCollision">コリジョンをしようするかどうか</param>
	Model(int orgModel, float scale, bool useCollision);
	//デストラクタ
	virtual ~Model();

	//更新
	void Update();
	//描画
	void Draw();

	/// <summary>
	/// モデルハンドルの取得
	/// </summary>
	/// <returns>モデルハンドル</returns>
	int GetModelHandle() const { return m_modelHandle; }

	//表示位置を取得する
	VECTOR GetPos() const;
	//表示位置の設定
	void SetPos(VECTOR pos);
	//回転状態の設定
	void SetRot(VECTOR rot);
	//拡大縮小の設定
	void SetScale(VECTOR scale);

	/// <summary>
	/// アニメーションを設定する(ぱっと切り替える)
	/// </summary>
	/// <param name="animNo">変更先アニメーション番号</param>
	/// <param name="isLoop">アニメーションをループさせるか</param>
	/// <param name="isForceChange">すでに指定されたアニメが再生されている場合も変更するか</param>
	void SetAnimation(int animNo, bool isLoop, bool isForceChange);

	/// <summary>
	/// アニメーションを変化させる(数フレームかけて切り替える)
	/// </summary>
	/// <param name="animNo">変更先アニメーション</param>
	/// <param name="isLoop">アニメーションをループさせるか</param>
	/// <param name="isForceChange">すでに指定されたアニメが再生されている場合も変更するか</param>
	/// <param name="changeFrame">フレーム数</param>
	void ChangeAnimstion(int animNo, bool isLoop, bool isForceChange, int changeFrame);

	/// <summary>
	/// 現在のアニメーションが終了しているかどうかを取得する
	/// Loopアニメの場合取得できない
	/// </summary>
	/// <returns>true:終了している false:終了していない</returns>
	bool isAnimEnd();

	/// <summary>
	/// 存在するかどうかを取得する
	/// </summary>
	/// <returns>true:存在する false:存在しない</returns>
	bool IsExist()const { return m_isExist; }
	/// <summary>
	/// 存在するかどうかを設定する
	/// </summary>
	/// <param name="isExist">true:存在する false:存在しない</param>
	void SetExist(bool isExist) { m_isExist = isExist; }

	/// <summary>
	/// モデルを削除する
	/// </summary>
	void DeleteModel();

	//コリジョンの更新
	void UpdateCollision();
private:
	//アニメーション情報
	struct AnimData
	{
		int animNo;			//アニメーション番号
		int attachNo;		//アタッチ番号
		float totalTime;	//アニメーションの総再生数時間
		bool isLoop;		//アニメーションがループするか
	};

private:
	/// <summary>
	/// アニメーションデータのクリア
	/// </summary>
	/// <param name="anim">アニメーションデータ</param>
	void ClearAnimData(AnimData& anim);

	/// <summary>
	/// アニメーションの更新
	/// </summary>
	/// <param name="anim">アニメーションデータ</param>
	/// <param name="dt">経過時間</param>
	void UpdateAnim(AnimData anim, float dt = 1.0f);

	/// <summary>
	/// 現在のアニメーション切り替わり情報からアニメーションのブレンド率を設定する
	/// </summary>
	void UpdateAnimBlendRate();

private:
	//モデルハンドル
	int m_modelHandle;

	//当たり判定情報を使用する
	bool m_isUseCollision;
	//当たり判定情報を毎フレーム更新する
	bool m_isUpdateVollision;

	//アニメーションのアタッチ番号
	AnimData m_animPrev;//変更前アニメーション情報
	AnimData m_animNext;//変更後アニメーション情報

	//アニメーションの切り替え情報
	int m_animChangeFrame;		//現在の切り替えフレーム数
	int m_animChangeFrameTotal;	//切り替えにかける総フレーム数

	bool m_isExist;//存在するか
};

