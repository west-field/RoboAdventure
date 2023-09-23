#include "EnemyStraightFly.h"
#include "../../Model.h"
#include "../../Util/DrawFunctions.h"
#include "../../WorldSprite.h"
#include "../Map.h"

#include "EffekseerForDXLib.h"

namespace
{
	// 速度（1=1m、60fps固定として、時速10km）10000m ÷ 時間 ÷ 分 ÷ 秒 ÷ フレーム
	constexpr float kSpeed = static_cast<float>(10000.0 / 60.0 / 60.0 / 60.0) - 0.01f;

	constexpr float kScale = 0.005f;//モデルの大きさ

	constexpr int kEffectSize = 516;//エフェクトの大きさ
	constexpr int kAnimNum = 22;//エフェクト画像の数
	constexpr int kAnimSpeed = 3;//エフェクトの再生スピード

	constexpr int kScore = 150;

	constexpr int kMaxHitColl = 1000;//取得できる当たったポリゴンの数

	constexpr float kSizeX = 0.6f;//モデルの大きさX
	constexpr float kSizeY = 0.8f;//モデルの大きさY

	constexpr float kCapsuleRadius = 0.6f;//球の半径

	constexpr int kSlideTryNum = 16;//スライドを試みる回数

	constexpr float kSlideMoveSpeed = 0.01f;//スライド

	constexpr float kHitLineTop = kSizeY;//線の上の位置
	constexpr float kHitLineBottom = -0.1f;//線の下の位置
}

EnemyStraightFly::EnemyStraightFly(VECTOR pos, int modelHandle, bool isLeft, std::shared_ptr<Map> map) :
	EnemyBase(pos, modelHandle, kScale, isLeft,map)
{
	size = VGet(0.6f, 0.8f, 0.4f);
	SetLTRB(VGet(-0.4f, size.y, 0.0f), VGet(-0.4f, 0.0f, 0.0f),
		VGet(0.4f, size.y, 0.0f), VGet(0.4f, 0.0f, 0.0f));

	m_model->SetAnimation(static_cast<int>(Animation::Flying), true, true);
}

EnemyStraightFly::~EnemyStraightFly()
{
}

void EnemyStraightFly::Update()
{
	//アニメーションを進める
	m_model->Update();

	//今向いている方向に移動する
	if (m_isLeft)
	{
		m_dir = VAdd(m_dir, VGet(-1, 0, 0));
		m_rot = VGet(0.0f, -80.0f, 0.0f);
	}
	else
	{
		m_dir = VAdd(m_dir, VGet(1, 0, 0));
		m_rot = VGet(0.0f, 80.0f, 0.0f);
	}

	// ゼロ除算避け
	if (VSquareSize(m_dir) > 0)
	{
		// 正規化
		m_dir = VNorm(m_dir);
	}
	m_vel = VScale(m_dir, kSpeed);

	Move();

	// 力をかけ終わったベロシティの方向にディレクションを調整.
	if (VSize(m_vel) != 0)
	{
		m_dir = VNorm(m_vel);
	}
	m_model->SetPos(m_pos);
	m_model->SetRot(m_rot);
	m_model->Update();
}

void EnemyStraightFly::Draw()
{
	//モデルを表示
	m_model->Draw();
}

int EnemyStraightFly::Score()
{
	return kScore;
}

void EnemyStraightFly::Move()
{
	bool isHitFlag;
	MV1_COLL_RESULT_POLY_DIM HitDim;//周囲にあるポリゴンを検出スタ結果が代入される当たり判定結果構造体
	int wallCollisionNum;//壁ポリゴンと判断されたポリゴンの数
	MV1_COLL_RESULT_POLY* wallCol[kMaxHitColl];	// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	HITRESULT_LINE LineRes;				// 線分とポリゴンとの当たり判定の結果を代入する構造体
	VECTOR oldPos;						// 移動前の座標	
	VECTOR nowPos;						// 移動後の座標


	// 移動前の座標を保存
	oldPos = m_pos;

	// 移動後の座標を算出
	nowPos = VAdd(m_pos, m_vel);
	//m_mapから当たり判定用のVECTORを取得する
	for (auto& model : m_map->GetPiece())
	{
		if (!model.model->IsExist())	continue;
		// プレイヤーの周囲にあるステージポリゴンを取得する
		// ( 検出する範囲は移動距離も考慮する )
		HitDim = MV1CollCheck_Sphere(model.model->GetModelHandle(), -1, VAdd(m_pos, VGet(0.0f, kSizeY / 3, 0.0f)), kCapsuleRadius + VSize(m_dir));

		if (HitDim.HitNum == 0)	continue;

		// 検出されたポリゴンが壁ポリゴン( ＸＺ平面に垂直なポリゴン )か床ポリゴン( ＸＺ平面に垂直ではないポリゴン )かを判断する
		{
			// 壁ポリゴンと床ポリゴンの数を初期化する
			wallCollisionNum = 0;

			// 検出されたポリゴンの数だけ繰り返し
			for (int i = 0; i < HitDim.HitNum; i++)
			{
				// ＸＺ平面に垂直かどうかはポリゴンの法線のＹ成分が０に限りなく近いかどうかで判断する
				if (HitDim.Dim[i].Normal.y < 0.000001f && HitDim.Dim[i].Normal.y > -0.000001f)
				{
					// 壁ポリゴンと判断された場合でも、プレイヤーのＹ座標＋１．０ｆより高いポリゴンのみ当たり判定を行う
					if (HitDim.Dim[i].Position[0].y > m_pos.y + 1.0f ||
						HitDim.Dim[i].Position[1].y > m_pos.y + 1.0f ||
						HitDim.Dim[i].Position[2].y > m_pos.y + 1.0f)
					{
						// ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
						if (wallCollisionNum < kMaxHitColl)
						{
							// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
							wallCol[wallCollisionNum] = &HitDim.Dim[i];

							// 壁ポリゴンの数を加算する
							wallCollisionNum++;
						}
					}
				}
			}
		}

		// 壁ポリゴンとの当たり判定処理
		if (wallCollisionNum != 0)
		{
			// 壁に当たったかどうかのフラグは初期状態では「当たっていない」にしておく
			isHitFlag = false;

			// 壁ポリゴンの数だけ繰り返し
			for (int i = 0; i < wallCollisionNum; i++)
			{
				// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
				if (HitCheck_Sphere_Triangle(VAdd(m_pos, VGet(0.0f, kSizeY / 3, 0.0f)), kCapsuleRadius,
					wallCol[i]->Position[0], wallCol[i]->Position[1], wallCol[i]->Position[2]) == FALSE)	continue;

				// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
				isHitFlag = true;

				m_isLeft = !m_isLeft;//壁と当たったら逆方向に移動させる

				// 壁に当たったら壁に遮られない移動成分分だけ移動する
				{
					VECTOR SlideVec;	// プレイヤーをスライドさせるベクトル

					// 進行方向ベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出
					SlideVec = VCross(m_vel, wallCol[i]->Normal);

					// 算出したベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出、これが
					// 元の移動成分から壁方向の移動成分を抜いたベクトル
					SlideVec = VCross(wallCol[i]->Normal, SlideVec);

					// それを移動前の座標に足したものを新たな座標とする
					nowPos = VAdd(oldPos, SlideVec);
					nowPos.z = 0.0f;
				}
				int j = 0;
				// 新たな移動座標で壁ポリゴンと当たっていないかどうかを判定する
				for (j = 0; j < wallCollisionNum; j++)
				{
					// 当たっていたらループから抜ける
					if (HitCheck_Sphere_Triangle(VAdd(m_pos, VGet(0.0f, kSizeY / 3, 0.0f)), kCapsuleRadius,
						wallCol[i]->Position[0], wallCol[i]->Position[1], wallCol[i]->Position[2]) == TRUE) break;
				}

				// j が KabeNum だった場合はどのポリゴンとも当たらなかったということなので
				// 壁に当たったフラグを倒した上でループから抜ける
				if (j == wallCollisionNum)
				{
					isHitFlag = false;
					break;
				}
			}

			// 壁に当たっていたら壁から押し出す処理を行う
			if (isHitFlag)
			{
				// 壁からの押し出し処理を試みる最大数だけ繰り返し
				for (int k = 0; k < kSlideTryNum; k++)
				{
					int i = 0;
					// 壁ポリゴンの数だけ繰り返し
					for (i = 0; i < wallCollisionNum; i++)
					{
						// プレイヤーと当たっているかを判定
						if (HitCheck_Sphere_Triangle(VAdd(m_pos, VGet(0.0f, kSizeY / 3, 0.0f)), kCapsuleRadius,
							wallCol[i]->Position[0], wallCol[i]->Position[1], wallCol[i]->Position[2]) == FALSE) continue;

						// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
						nowPos = VAdd(nowPos, VScale(wallCol[i]->Normal, kSlideMoveSpeed));
						nowPos.z = 0.0f;

						int j = 0;
						// 移動した上で壁ポリゴンと接触しているかどうかを判定
						for (j = 0; j < wallCollisionNum; j++)
						{
							if (HitCheck_Sphere_Triangle(VAdd(m_pos, VGet(0.0f, kSizeY / 3, 0.0f)), kCapsuleRadius,
								wallCol[i]->Position[0], wallCol[i]->Position[1], wallCol[i]->Position[2]) == TRUE) break;
						}

						// 全てのポリゴンと当たっていなかったらここでループ終了
						if (j == wallCollisionNum) break;
					}

					// i が KabeNum ではない場合は全部のポリゴンで押し出しを試みる前に全ての壁ポリゴンと接触しなくなったということなのでループから抜ける
					if (i != wallCollisionNum) break;
				}
			}
		}
	}

	// 新しい座標を保存する
	m_pos = nowPos;
}

const VECTOR EnemyStraightFly::GetEffectPos() const
{
	return VAdd(m_pos, VGet(0.0f, kSizeY / 2, 0.0f));
}
