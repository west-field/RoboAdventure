#include "Model.h"
#include <cassert>

namespace
{
	//当たり判定として使用するフレームの名前
	const TCHAR* const kCollisionFrameName = L"Collision";
}

//モデルファイル名を指定してロードを行う
Model::Model(const TCHAR* fileName,float scale,bool useCollision) :
	m_modelHandle(-1), m_isUseCollision(useCollision), m_isUpdateVollision(false), m_animChangeFrame(0),
	m_animChangeFrameTotal(0), m_isExist(true)
{
	m_modelHandle = MV1LoadModel(fileName);
	//モデルのロードに失敗した
	assert(m_modelHandle != -1);
	if (m_isUseCollision)
	{
		MV1SetupCollInfo(m_modelHandle, -1);
	}
	//アニメーションデータのクリア
	ClearAnimData(m_animPrev);
	ClearAnimData(m_animNext);
	SetScale(VGet(scale, scale, scale));
}
//指定されたハンドルのモデルをDuplicateして生成する
Model::Model(int orgModel, float scale, bool useCollision) :
	m_modelHandle(-1), m_isUseCollision(useCollision), m_isUpdateVollision(false), m_animChangeFrame(0),
	m_animChangeFrameTotal(0)
{
	//ロードに失敗したモデルがコピー元として指定されている
	assert(orgModel != -1);
	m_modelHandle = MV1DuplicateModel(orgModel);
	assert(m_modelHandle != -1);
	if (m_isUseCollision)
	{
		MV1SetupCollInfo(m_modelHandle, -1);
	}
	//アニメーションデータのクリア
	ClearAnimData(m_animPrev);
	ClearAnimData(m_animNext);
	SetScale(VGet(scale, scale, scale));
}
//デストラクタ
Model::~Model()
{
	if (m_isUseCollision)
	{
		//当たり判定情報を使用していたら破棄する
		MV1TerminateCollInfo(m_modelHandle, -1);
	}
	MV1DeleteModel(m_modelHandle);
}

//更新
void Model::Update()
{
	UpdateAnim(m_animPrev);
	UpdateAnim(m_animNext);

	//指定フレームかけてアニメーションを変更する
	m_animChangeFrame++;
	if (m_animChangeFrame > m_animChangeFrameTotal)
	{
		m_animChangeFrame = m_animChangeFrameTotal;
	}
	//アニメーションのブレンド率を設定する
	UpdateAnimBlendRate();
	//コリジョンの更新
	UpdateCollision();
}
//描画
void Model::Draw()
{
	MV1DrawModel(m_modelHandle);
}

//表示位置を取得する
VECTOR Model::GetPos() const
{
	return MV1GetPosition(m_modelHandle);
}
//表示位置の設定
void Model::SetPos(VECTOR pos)
{
	MV1SetPosition(m_modelHandle, pos);
}
//回転状態の設定
void Model::SetRot(VECTOR rot)
{
	MV1SetRotationXYZ(m_modelHandle, rot);
}
//拡大縮小の設定
void Model::SetScale(VECTOR scale)
{
	MV1SetScale(m_modelHandle, scale);
}

//アニメーションを設定する(パット切り替える)
void Model::SetAnimation(int animNo, bool isLoop, bool isForceChange)
{
	if (!isForceChange)
	{
		//すでに再生されているアニメは再生しない
		if (m_animNext.animNo == animNo)	return;
	}

	//以前のアニメーションが残っていれば終了
	if (m_animPrev.attachNo != -1)
	{
		MV1DetachAnim(m_modelHandle, m_animPrev.attachNo);
		ClearAnimData(m_animPrev);
	}
	if (m_animNext.attachNo != -1)
	{
		MV1DetachAnim(m_modelHandle, m_animNext.attachNo);
		ClearAnimData(m_animNext);
	}
	//新しいアニメーションを設定
	m_animNext.animNo = animNo;
	m_animNext.attachNo = MV1AttachAnim(m_modelHandle, animNo, -1, false);
	m_animNext.totalTime = MV1GetAttachAnimTotalTime(m_modelHandle, m_animNext.attachNo);
	m_animNext.isLoop = isLoop;

	m_animChangeFrameTotal = 1;
	m_animChangeFrame = 1;
}

//アニメーションを変化させる(数フレームかけて切り替える)
void Model::ChangeAnimstion(int animNo, bool isLoop, bool isForceChange, int changeFrame)
{
	if (!isForceChange)
	{
		//すでに再生されているアニメは再生しない
		if (m_animNext.animNo == animNo)	return;
	}

	if (m_animPrev.attachNo != -1)
	{
		//前回のアニメーションが設定されていたら完全に終了させる
		MV1DetachAnim(m_modelHandle, m_animPrev.attachNo);
		ClearAnimData(m_animPrev);
	}

	//現在再生しているアニメーションを古いアニメーションとする
	m_animPrev = m_animNext;
	//新しいアニメーションを設定
	m_animNext.animNo = animNo;
	m_animNext.attachNo = MV1AttachAnim(m_modelHandle, animNo, -1, false);
	m_animNext.totalTime = MV1GetAttachAnimTotalTime(m_modelHandle, m_animNext.attachNo);
	m_animNext.isLoop = isLoop;

	//変更にかけるフレーム数を覚えておく
	m_animChangeFrameTotal = changeFrame;
	m_animChangeFrame = 0;
	//アニメーションのブレンド率を設定する
	UpdateAnimBlendRate();
}

//現在のアニメーションが終了しているかどうかを取得する
bool Model::isAnimEnd()
{
	//Loopアニメの場合は常にfalseを返す
	if (m_animNext.isLoop)	return false;

	float time = MV1GetAttachAnimTime(m_modelHandle, m_animNext.attachNo);
	//現在のアニメーション時間がトータル時間よりも長いとき
	if (time >= m_animNext.totalTime)	return true;

	return false;
}

void Model::DeleteModel()
{
	if (m_isUseCollision)
	{
		//当たり判定情報を使用していたら破棄する
		MV1TerminateCollInfo(m_modelHandle, -1);
	}
	MV1DeleteModel(m_modelHandle);
}

void Model::UpdateCollision()
{
	//コリジョンを使用する場合、更新する
	if (m_isUseCollision)
	{
		MV1SetupCollInfo(m_modelHandle, -1);
	}
}

void Model::ClearAnimData(AnimData& anim)
{
	anim.animNo = -1;
	anim.attachNo = -1;
	anim.totalTime = 0.0f;
	anim.isLoop = false;
}

void Model::UpdateAnim(AnimData anim, float dt)
{
	//アニメーションが設定されていない場合は何もしない
	if (anim.attachNo == -1) return;

	//アニメーションの更新
	float time = MV1GetAttachAnimTime(m_modelHandle, anim.attachNo);
	time += dt;

	if (time >= anim.totalTime)
	{
		if (anim.isLoop)
		{
			//アニメーションのループ
			time -= anim.totalTime;
		}
		else
		{
			//最後の状態で止まる
			time = anim.totalTime;
		}
	}

	MV1SetAttachAnimTime(m_modelHandle, anim.attachNo, time);

}

void Model::UpdateAnimBlendRate()
{
	//アニメーション変化のフレーム数に応じたブレンド率を設定する
	float rate = static_cast<float>(m_animChangeFrame / m_animChangeFrameTotal);

	if (rate > 1.0f)
	{
		rate = 1.0f;
	}

	MV1SetAttachAnimBlendRate(m_modelHandle, m_animPrev.attachNo, 1.0f - rate);
	MV1SetAttachAnimBlendRate(m_modelHandle, m_animNext.attachNo, rate);

}
