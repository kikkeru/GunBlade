#include "FollowCamera.h"
#include "Field.h"
#include "Player.h"

FollowCamera::FollowCamera()
	: Base(eCamera), m_dist(4.0f) {
	//回転値
	m_rot = CVector3D(DtoR(30), DtoR(-125), 0);
}

void FollowCamera::Update()
{
	//カメラシェイクの処理
	shake.Update();

	m_dist = 5.0f;
	//回転速度
	float cam_speed = 0.0015;
	//マウスの移動量
	CVector2D mouse_vec = CInput::GetMouseVec();
	//マウス移動でカメラ回転
	m_rot += CVector3D(mouse_vec.y, -mouse_vec.x, 0) * cam_speed;
	m_rot.x = min(DtoR(60), max(DtoR(-35), m_rot.x));
	//-180～180に正規化
	m_rot.y = Utility::NormalizeAngle(m_rot.y);
}

void FollowCamera::Render()
{
	//イベントカメラがあったらプレイヤーのカメラを描画しない
	if (Base::FindObject(eEventCamera)) return;

	//プレイヤーの取得
	if (Player* b = dynamic_cast<Player*>( Base::FindObject(ePlayer)))
	{

		//銃の状態の時
		if (b->m_state_flag) {
			if (b->m_lock_flag == true && b->mp_target != nullptr) {
				CVector3D dir = (b->mp_target->m_pos - b->m_pos).GetNormalize();
				m_rot = CVector3D(asin(-dir.y), atan2(dir.x, dir.z), 0);
			}

			//キャラクターの右肩ごしにカメラを設置する
			CMatrix cam_matrix = CMatrix::MTranselate(b->m_pos)								//キャラクターの行列
				* CMatrix::MTranselate(CVector3D(0, 1.07, 0)) * CMatrix::MRotation(m_rot)   //(胸への平行移動*回転）
				* CMatrix::MTranselate(CVector3D(-0.5, 0.4, -2));							//(胸から右上後方へ平行移動)

			//接触点と法線
			CVector3D cross, normal;
			CVector3D s = cam_matrix * CVector4D(0, 0, 0, 1);
			CVector3D e = cam_matrix * CVector4D(0, 0, 4, 1);
			//フィールドと線分（注視点→視点）との衝突判定
			if (Base* f = Base::FindObject(eField)) {
				if (f->GetModel()->CollisionRay(&cross, &normal, e, s)) {
					cam_matrix.m03 = cross.x;
					cam_matrix.m13 = cross.y;
					cam_matrix.m23 = cross.z;
				}
			}
			//カメラのワールド行列でカメラを設定
			CCamera::GetCurrent()->SetTranseRot(cam_matrix * CMatrix::MTranselate(shake.GetShakeOffset()));
		}
		//剣の状態の時
		else {
			//位置と注視点を設定
			m_at = b->m_pos + CVector3D(0, 1.5, 0);
			m_pos = m_at + CMatrix::MRotation(m_rot).GetFront() * -m_dist;
			//カメラとフィールドの接触判定
		    //接触点と法線
			CVector3D cross, normal;
			//フィールドと線分（注視点→視点）との衝突判定
			if (Base* f = Base::FindObject(eField)) {
				if (f->GetModel()->CollisionRay(&cross, &normal, m_at, m_pos)) {
					m_pos = cross;
				}
			}
			CCamera::GetCurrent()->LookAt(m_pos + shake.GetShakeOffset(), m_at + shake.GetShakeOffset(), CVector3D(0, 1, 0));
		}
	}

}

void FollowCamera::StartShake(float intensity, float duration)
{
	shake.StartShake(intensity, duration);
}
