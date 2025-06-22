#include "Boss.h"
#include "Player.h"
#include "Enemy.h"
#include "Effect.h"
#include "../Effekseer/EffekseerEffect.h"
#include "FollowCamera.h"

//定速の角度補間
float LerpAngle1(float now, float target, float speed) {
	float a = Utility::NormalizeAngle(target - now);
	if (a > speed)
		now += speed;
	else if (a < -speed)
		now -= speed;
	else
		now += a;
	return Utility::NormalizeAngle(now);
}

Boss::Boss(const CVector3D& pos)
	:Base(eBoss) {
	m_model = COPY_RESOURCE("Mutant", CModelA3M);
	//半径
	m_rad = 1.1f;
	m_b_rad = 4.5f;
	m_hp = 1800;
	//ヒットストップ初期化
	m_hit_stop = 0;
	//初期位置がプレイヤーと重ならないようにする
	m_pos.z = -10.0f;
	m_pos.x = -1.3f;
	m_pos.y = 5.0f;
	m_rush_recharge = 0;
	m_jump_recharge = 0;
	m_trick_recharge = 0;
	m_roll_recharge = 0;
	m_hit = 0;
	m_reset_time = 0;
	m_bom_pow = 0;
	m_cnt = 0;
	m_look_cnt = 0;
	m_wait_time = 0;
	m_attack_pow = 0;
	m_attackS_flag = false;
	m_sound_flag = false;
	m_act_flag = false;
	m_event_flag = false;
	m_frame = 0;
	m_b_rad = 3.0f;
	m_attck_step = 0;

	mp_camera = dynamic_cast<EventCamera*> (Base::FindObject(eEventCamera));
}

Boss::Index Boss::m_culsule_idx[eCap_Max] = {
	{11,14,0.7f},		//左腕
	{16,18,0.5f},		//右腕
};

void Boss::Update() {

	//当たり判定カプセルの設定
	m_lineS = m_pos + CVector3D(0, 3.5 - m_rad, 0);
	m_lineE = m_pos + CVector3D(0, m_rad, 0);

	//ヒットストップ
	if (m_hit_stop > 0) {
		m_hit_stop -= CFPS::GetDeltaTime();
		return;
	}

	Recharge();

	switch (m_state) {
	case eState_Idle:
		StateIdle();
		break;
	case eState_Step:
		StateStep();
		break;
	case eState_AttackP:
		StateAttackP();
		break;
	case eState_AttackK:
		StateAttackK();
		break;
	case eState_AttackKTrick:
		StateAttackKTrick();
		break;
	case eState_AttackJump:
		StateAttackJump();
		break;
	case eState_AttackSRush:
		StateAttackSRush();
		break;
	case eState_AttackRush:
		StateAttackRush();
		break;
	case eState_AttackS1:
		StateAttackS1();
		break;
	case eState_AttackS2:
		StateAttackS2();
		break;
	case eState_AttackS3:
		StateAttackS3();
		break;
	case eState_Burst:
		StateBurst();
		break;
	case eState_Damage:
		StateDamage();
		break;
	case eState_Break:
		StateBreak();
		break;
	case eState_Death:
		StateDeath();
		break;
	case eState_Roar:
		StateRoar();
		break;
	}

	m_model.UpdateAnimation();

	m_lineS = m_pos + CVector3D(0, 3.5 - m_rad, 0);
	m_lineE = m_pos + CVector3D(0, m_rad, 0);

	//カプセルの設定
	for (int i = 0; i < eCap_Max; i++) {
		//カプセルを設定
		m_cupsule[i].s = m_model.GetFrameMatrix(m_culsule_idx[i].s).GetPosition();
		m_cupsule[i].e = m_model.GetFrameMatrix(m_culsule_idx[i].e).GetPosition();
		m_cupsule[i].r = m_culsule_idx[i].r * 1.0;
	}

	//重力落下
	m_vec.y -= GRAVITY;
	m_pos.y += m_vec.y;
}

void Boss::Render()
{
	m_model.SetPos(m_pos);
	m_model.SetRot(m_rot);
	m_model.SetScale(0.02, 0.02, 0.02);
	m_model.Render();

	//キャラのカプセルの表示
	//Utility::DrawCapsule(m_lineS, m_lineE, m_rad, CVector4D(1, 0, 0, 0.5));
	//部位ごとにカプセルを設定する(矩形表示用)
	/*
	for (int i = 0; i < eCap_Max; i++) {
		Utility::DrawCapsule(m_cupsule[i].s, m_cupsule[i].e, m_cupsule[i].r, CVector4D(1, 0, 0, 0.5));
	}
	*/
}

void Boss::Collision(Base* b)
{
	switch (b->GetType()) {
	case eField:
		//モデルとの判定
	{
		//押し戻し量
		CVector3D v(0, 0, 0);
		//カプセルとモデルの衝突
		auto tri = b->GetModel()->CollisionCupsel(m_pos + CVector3D(0, 2.5 - m_rad, 0),	//始点（頭）
			m_pos + CVector3D(0, m_rad, 0),	//終点（足元）
			m_rad);
		//接触した面の数繰り返す
		for (auto& t : tri) {
			if (t.m_normal.y < -0.5f) {
				//面が下向き→天井に当たった
				//上昇速度を0に戻す
				if (m_vec.y > 0)
					m_vec.y = 0;
			}
			else if (t.m_normal.y > 0.5f) {
				//面が上向き→地面に当たった
				//重力落下速度を0に戻す
				if (m_vec.y < 0)
					m_vec.y = 0;
			}
			float max_y = max(t.m_vertex[0].y, max(t.m_vertex[1].y, t.m_vertex[2].y));
			//接触した面の方向へ、めり込んだ分押し戻す
			CVector3D nv = t.m_normal * (m_rad - t.m_dist);
			//最も大きな移動量を求める
			v.y = fabs(v.y) > fabs(nv.y) ? v.y : nv.y;
			//膝下までは乗り越える
			//膝上以上の壁のみ押し戻される
			if (max_y > m_pos.y + 0.2f) {
				v.x = fabs(v.x) > fabs(nv.x) ? v.x : nv.x;
				v.z = fabs(v.z) > fabs(nv.z) ? v.z : nv.z;
			}
		}
		//押し戻す
		m_pos += v;
	}
	break;
	case ePlayer:
		CVector3D dir;
		float dist;
		//攻撃処理
		if (m_attack_flag) {
			CVector3D c1;
			CVector3D pos;
			m_act_flag = true;

			if (Player* p = dynamic_cast<Player*>(b)){
				//プレイヤーの無敵時間
				if (p->m_state == Player::eState_Damage || p->m_state == Player::eState_DamageF || p->m_state == Player::eState_Roll) {
					break;
				}
				//攻撃
				if (m_state == eState_AttackP) {
					//多段ヒット回避と右腕とカプセルの判定
					if (m_attack_no != b->m_hit_no && CCollision::CollisionCapsule(m_cupsule[eCap_Right_Arm].s, m_cupsule[eCap_Right_Arm].e, m_cupsule[eCap_Right_Arm].r, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)) {
						//多段ヒット回避
						b->m_hit_no = m_attack_no;

						//ヒットストップ
						m_hit_stop = 0.10f;

						//回避とパリィ
						Avoid(p, this);
						Parry(p, this);

						if (m_act_flag == true) {
							//ダメージ処理
							sound.Play("bossattack");
							p->Damage(this, m_attack_pow, 0.0f);
							pos = c1;
							Base::Add(new EffekseerEffect("Blow11", pos, CVector3D(0, 0, 0), CVector3D(0.5, 0.5, 0.5)));
						}
					}
				}
				//強攻撃
				if (m_state == eState_AttackK) {
					//多段ヒット回避と左腕とカプセルの判定
					if (m_attack_no != b->m_hit_no && CCollision::CollisionCapsule(m_cupsule[eCap_Left_Arm].s, m_cupsule[eCap_Left_Arm].e, m_cupsule[eCap_Left_Arm].r, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)) {

						//多段ヒット回避
						b->m_hit_no = m_attack_no;

						//ヒットストップ
						m_hit_stop = 0.20f;

						Avoid(p, this);
						Parry(p, this);

						if (m_act_flag == true) {
							sound.Play("bossattack2");
							p->Damage(this, m_attack_pow, 0.0f);
							pos = c1;
							Base::Add(new EffekseerEffect("Blow11", pos, CVector3D(0, 0, 0), CVector3D(0.5, 0.5, 0.5)));
						}
					}
				}
				//強攻撃に見せかけた別攻撃
				if (m_state == eState_AttackKTrick) {
					//多段ヒット回避と左腕とカプセルの判定
					if (m_attack_no != b->m_hit_no && CCollision::CollisionCapsule(m_cupsule[eCap_Left_Arm].s, m_cupsule[eCap_Left_Arm].e, m_cupsule[eCap_Left_Arm].r, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)) {

						//多段ヒット回避
						b->m_hit_no = m_attack_no;

						//ヒットストップ
						m_hit_stop = 0.20f;

						Avoid(p, this);
						Parry(p, this);

						if (m_act_flag == true) {
							sound.Play("bossattack2");
							p->Damage(this, m_attack_pow, 0.0f);
							pos = c1;
							Base::Add(new EffekseerEffect("Blow11", pos, CVector3D(0, 0, 0), CVector3D(0.5, 0.5, 0.5)));
						}

					}
				}
				//とびかかり
				if (m_state == eState_AttackJump) {
					//多段ヒット回避と両腕とカプセルの判定
					if (m_attack_no != b->m_hit_no && CCollision::CollisionCapsule(m_cupsule[eCap_Left_Arm].s, m_cupsule[eCap_Left_Arm].e, m_cupsule[eCap_Left_Arm].r, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)
						|| m_attack_no != b->m_hit_no && CCollision::CollisionCapsule(m_cupsule[eCap_Left_Arm].s, m_cupsule[eCap_Left_Arm].e, m_cupsule[eCap_Left_Arm].r, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)) {

						//多段ヒット回避
						b->m_hit_no = m_attack_no;

						//ヒットストップ
						m_hit_stop = 0.15f;

						Avoid(p, this);
						Parry(p, this);

						if (m_act_flag == true) {
							sound.Play("bossattack");
							p->Damage(this, m_attack_pow, 0.0f);
							pos = c1;
							Base::Add(new EffekseerEffect("Blow11", pos, CVector3D(0, 0, 0), CVector3D(0.5, 0.5, 0.5)));
						}
					}
				}
				//突進
				if (m_state == eState_AttackRush) {
					//多段ヒット回避と体全体とカプセルの判定
					if (m_attack_no != b->m_hit_no && CCollision::CollisionCapsule(m_lineS, m_lineE, m_rad, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)) {
						//多段ヒット回避
						b->m_hit_no = m_attack_no;

						//ヒットストップ
						m_hit_stop = 0.30f;

						Avoid(p, this);
						Parry(p, this);

						if (m_act_flag == true) {
							sound.Play("rushhit");
							p->Damage(this, m_attack_pow, 0.10f);
							pos = c1;
							Base::Add(new EffekseerEffect("Blow3", pos, m_rot, CVector3D(1, 1, 1)));
						}
					}
				}
				//バースト
				if (m_state == eState_Burst) {
					CVector3D cross;
					//多段ヒット回避と球とカプセルの判定
					if (m_attack_no != b->m_hit_no && CCollision::CollisionCapsuleShpere(b->m_lineS, b->m_lineE, b->m_rad, m_pos, m_b_rad, &dist, &cross, &dir)) {
						//多段ヒット回避
						b->m_hit_no = m_attack_no;
						//ヒットストップ
						m_hit_stop = 0.40f;

						Avoid(p, this);

						if (m_act_flag == true) {
							sound.Play("rushstart");
							p->Damage(this, m_attack_pow, 0.23f);
						}
					}
				}
				//大技
				if (m_state == eState_AttackS3) {
					CVector3D cross;
					//多段ヒット回避と球とカプセルの判定
					if (m_attack_no != b->m_hit_no && CCollision::CollisionCapsule(m_lineS, m_lineE, m_rad, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)) {
						//多段ヒット回避
						b->m_hit_no = m_attack_no;

						//ヒットストップ
						m_hit_stop = 0.50f;

						Avoid(p, this);

						if (m_act_flag == true) {
							p->Damage(this, m_attack_pow, 0.17f);

							pos = c1;
							sound.Play("rushhit");
							Base::Add(new EffekseerEffect("Blow3", pos, m_rot, CVector3D(1, 1, 1)));
						}
					}
				}
			}
		}
		break;
	}
}

void Boss::StateIdle() {
	CFPS::SetTimeScale(1.0);
	m_bom_pow = 0;
	m_attack_flag = false;

	if (!mp_target)
		mp_target = Base::FindObject(ePlayer);
	//イベントシーンじゃなければ動く
	if (m_event_flag) {
		m_cnt -= CFPS::GetDeltaTime();
		if (m_wait_cnt >= 0) {
			m_model.ChangeAnimation(14);
			m_wait_cnt--;
		}
		else {
			if (m_cnt < 0) m_cnt = 2;
			if (m_cnt < 10) {
				move_speed = 0.12f;

				//リストからプレイヤーを探索
				if (mp_target) {
					//ターゲットへのベクトル
					CVector3D vec = mp_target->m_pos - m_pos;
					//ベクトルから回転値を逆算
					m_rot_target.y = atan2(vec.x, vec.z);
					//回転値から方向ベクトルを計算
					m_dir = CVector3D(CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));
					//ターゲットへの距離が1.6fより遠ければ
					if (vec.Length() > 1.6f) {
						//前進
						m_pos += m_dir * move_speed * CFPS::GetTimeScale();
						//前進アニメーション
						m_model.ChangeAnimation(2);
					}
					else if (rand() % 1000 < 150) {
						//攻撃ID変更
						m_attack_no++;
						m_state = eState_AttackP;

					}
					else if (rand() % 1000 < 80) {
						m_state = eState_Idle;
					}

					//プレイヤーとの距離が離れているかつリチャージが0の時
					//とびかかり
					if (rand() % 1000 < 20 && vec.Length() > 4.0f && m_jump_recharge <= 0) {
						m_attack_no++;
						m_state = eState_AttackJump;
					}

					//突進
					if (rand() % 1000 < 50 && m_rush_recharge <= 0 && vec.Length() > 10.0f) {
						m_attack_no++;
						m_state = eState_AttackSRush;
					}

					//少し離れていたら強攻撃する
					if (rand() % 1000 < 30 && vec.Length() < 3.0f) {
						//攻撃ID変更
						m_attack_no++;
						m_state = eState_AttackK;
					}

					//もし1.7fより近ければ後ろにステップで下がる
					if (vec.Length() < 1.7f && m_roll_recharge <= 0 && rand() % 1000 < 30) {
						m_state = eState_Step;
					}

					//大技
					if (m_hp <= 500 && m_attackS_flag == false) {
						m_state = eState_AttackS1;
					}

					float a = Utility::NormalizeAngle(m_rot_target.y - m_rot.y);

					//割合による補間
					m_rot.y += a * 0.1f;
				}
			}
			else {
				//待機アニメーション
				m_model.ChangeAnimation(0);
			}
		}
	}
}

void Boss::StateStep()
{
	m_model.SetAnimationSpeed(1.8f);
	m_model.ChangeAnimation(9,false);

	//飛び上がった時
	if (m_model.GetAnimationFrame() > 36 && m_model.GetAnimationFrame() < 56) {
		jmove_speed = 0.02;
		
		//後ろに下がる
		m_pos -= m_dir * move_speed * CFPS::GetTimeScale();
	}

	if (m_model.isAnimationEnd()) {
		m_model.SetAnimationSpeed(1.0f);
		m_roll_recharge = 15.0f;
		m_state = eState_Idle;
	}

}

//パンチ
void Boss::StateAttackP() {
	m_model.SetAnimationSpeed(0.7f);
	m_attack_pow = 10;
	m_model.ChangeAnimation(3, false);

	if (m_model.GetAnimationFrame() > 0 && m_model.GetAnimationFrame() < 5) {
		m_model.SetAnimationSpeed(0.2);
	}
	else {
		m_model.SetAnimationSpeed(1.0);
	}

	//特定のフレーム間攻撃発生
	if (m_model.GetAnimationFrame() > 7 && m_model.GetAnimationFrame() < 15) {
		//攻撃フラグON
		m_attack_flag = true;

	}
	else {
		m_attack_flag = false;
	}
	
	if (m_model.isAnimationEnd()) {
		CVector3D vec = mp_target->m_pos - m_pos;
		m_state = eState_Idle;
	}
}

//強攻撃
void Boss::StateAttackK() {
	m_model.ChangeAnimation(4, false);
	move_speed = 0.05f;
	m_attack_pow = 50;
	CVector3D vec = mp_target->m_pos - m_pos;
	m_model.SetAnimationSpeed(1.0);

	if ( m_model.GetAnimationFrame() == 1) {
		//プレイヤーの方に向く
		LookToTarget();
	}
	
	//構え中に近づかれたら確率で別攻撃へ移行
	if (m_model.GetAnimationFrame() > 20 && m_model.GetAnimationFrame() < 28 && m_trick_recharge <= 0 && vec.Length() < 1.3f && rand() % 1000 < 30) {
		m_attack_no++;
		m_state = eState_AttackKTrick;
	}

	if (m_model.GetAnimationFrame() > 31 && m_model.GetAnimationFrame() < 39) {
		m_pos += m_dir * move_speed;
	}
	else if (m_model.GetAnimationFrame() > 54 && m_model.GetAnimationFrame() < 62) {
		m_pos -= m_dir * move_speed;
	}

	//特定のフレーム間攻撃発生
	if (m_model.GetAnimationFrame() > 38 && m_model.GetAnimationFrame() < 40) {
		//攻撃フラグON
		m_attack_flag = true;
	}
	else {
		m_attack_flag = false;
	}

	if (m_model.isAnimationEnd()) {
		m_state = eState_Idle;
	}

}

//強攻撃構え中に近づかれたときのカウンター
void Boss::StateAttackKTrick()
{
	m_model.ChangeAnimation(13, false);
	m_attack_pow = 4.0f;
	m_trick_recharge = 10.0f;

	if (m_model.GetAnimationFrame() > 12 && m_model.GetAnimationFrame() < 25) {
		m_model.SetAnimationSpeed(0.7);
	}
	else if (m_model.GetAnimationFrame() > 22 && m_model.GetAnimationFrame() < 31) {
		m_model.SetAnimationSpeed(1.5);
		//攻撃フラグON
		m_attack_flag = true;
	}
	else {
		m_attack_flag = false;
		m_model.SetAnimationSpeed(1.0);
	}

	if (m_model.isAnimationEnd()) {
		m_state = eState_Idle;
	}

}

//ジャンプ攻撃
void Boss::StateAttackJump() {
	m_model.ChangeAnimation(5, false);
	m_attack_pow = 40;
	jmove_speed = 0.05f;
	m_jump_recharge = 15.0f;

	if (m_model.GetAnimationFrame() > 1 && m_model.GetAnimationFrame() < 10) {
		m_model.SetAnimationSpeed(0.5f);
		//プレイヤーの方に向く
		LookToTarget();
	}
	else {
		m_model.SetAnimationSpeed(1.0f);
	}

	//飛び上がった時
	if (m_model.GetAnimationFrame() > 16 && m_model.GetAnimationFrame() < 33) {	
		
		if (m_model.GetAnimationFrame() > 16 && m_model.GetAnimationFrame() < 18) {
			//プレイヤーの方に向く
			LookToTarget();
		}

		//自身からプレイヤーへのベクトル
		v = mp_target->m_pos - m_pos;
		//プレイヤーにとびかかる
		m_pos += v * jmove_speed * CFPS::GetTimeScale();
	}

	//特定のフレーム間攻撃発生
	if (m_model.GetAnimationFrame() > 46 && m_model.GetAnimationFrame() < 50) {
		//攻撃フラグON
		m_attack_flag = true;
	}
	else {
		m_attack_flag = false;
	}

	if (m_model.isAnimationEnd()) {
		m_state = eState_Idle;
	}

}

//走り出し
void Boss::StateAttackSRush()
{
	m_rush_cnt = 2;
	m_model.ChangeAnimation(11, false);

	if (m_model.GetAnimationFrame() == 4) {
		sound.Play("rushstart");
	}

	if (m_model.GetAnimationFrame() > 0 && m_model.GetAnimationFrame() < 3) {
		m_model.SetAnimationSpeed(0.05);
		LookToTarget();
	}
	else {
		m_model.SetAnimationSpeed(1.0);
	}

	if (m_model.isAnimationEnd()) {
		m_state = eState_AttackRush;
	}
}

//突進中
void Boss::StateAttackRush()
{
	m_attack_pow = 60;
	m_rush_recharge = 17.0f;
	m_model.SetAnimationSpeed(1.7);
	m_model.ChangeAnimation(12, true);
	//攻撃フラグON
	m_attack_flag = true;
	move_speed = 0.25;

	m_frame = m_model.GetAnimationFrame();

	LerpTarget();
	m_pos += CMatrix::MRotation(m_rot).GetFront() * move_speed * CFPS::GetTimeScale();

	//走り終わるまでの時間
	if (m_rush_cnt > 0) {
		m_rush_cnt -= CFPS::GetDeltaTime() * CFPS::GetTimeScale();
	}
	
	if (m_rush_cnt <= 0) {
		m_model.ChangeAnimation(0);
		m_model.SetAnimationSpeed(1.0);
		m_state = eState_Idle;
		m_attack_flag = false;
		m_wait_cnt = 100;
	}

}

//大技前の咆哮
void Boss::StateAttackS1()
{
	m_model.ChangeAnimation(1, false);
	m_model.SetAnimationSpeed(1.2f);

	if (m_model.GetAnimationFrame() == 51) {
		sound.Play("roar");
		Base::Add(new EffekseerEffect("Roar", m_pos, CVector3D(0, 0, 0), CVector3D(1, 1, 1)));
	}

	if (m_model.isAnimationEnd()) {
		m_state = eState_AttackS2;
	}
}

void Boss::StateAttackS2()
{
	m_model.ChangeAnimation(9, false);
	m_model.SetAnimationSpeed(1.5f);

	if (m_model.GetAnimationFrame() >= 37) {
		m_pos -= m_dir * move_speed;
	}
	
	if (m_model.GetAnimationFrame() == 55) {
		m_attack_no++;
		m_state = eState_AttackS3;
		m_attck_step = 0;
	}
}
//大技発生
void Boss::StateAttackS3()
{
	m_attack_pow = 50;
	move_speed = 0.35;

	switch (m_attck_step)
	{
	case 0:
	case 2:
	case 4:
	case 6:
	case 8:
		m_attack_flag = true;
		//ランダムに４箇所の座標が選ばれる
		m_set_point = Utility::Rand(1, 4);
		if (m_set_point == 1) {
			m_pos = CVector3D(-22, 0, -12);
		}
		else if (m_set_point == 2) {
			m_pos = CVector3D(-6, 0, -25);
		}
		else if (m_set_point == 3) {
			m_pos = CVector3D(-9, 0, -24);
		}
		else {
			m_pos = CVector3D(20, 0, -11);
		}

		m_attck_step++;
		m_rush_cnt = 2.0f;
		m_look_cnt = 0.5f;

		break;
	case 1:
	case 3:
	case 5:
	case 7:
	case 9:

		//ロックオンする
		if (m_look_cnt > 0) {
			LookToTarget();
			m_look_cnt -= CFPS::GetDeltaTime();
			m_model.SetAnimationSpeed(0.1);
			m_model.ChangeAnimation(11, false);
			m_sound_flag = false;
		}
		else {
			if (m_model.GetAnimationFrame() == 1 && m_sound_flag == false) {
				sound.Play("rushstart");
				m_sound_flag = true;
			}
			m_model.SetAnimationSpeed(2.0);
			m_model.ChangeAnimation(12, true);

			LerpTarget();
			m_pos += CMatrix::MRotation(m_rot).GetFront() * move_speed * CFPS::GetTimeScale();

			//走り終わるまでの時間
			if (m_rush_cnt > 0) {
				m_rush_cnt -= CFPS::GetDeltaTime() * CFPS::GetTimeScale();
			}

			if (m_rush_cnt <= 0) {
				m_attack_no++;
				m_attack_flag = false;
				m_attck_step++;
			}
		}
		break;
	case 10:
		m_state = eState_Idle;
		m_attackS_flag = true;
		break;
	}

}

//吹き飛ばし
void Boss::StateBurst()
{
	m_model.ChangeAnimation(10, false);
	m_attack_pow = 0;
	m_hit = 0;

	if (m_model.GetAnimationFrame() > 0 && m_model.GetAnimationFrame() < 9) {
		m_model.SetAnimationSpeed(0.4f);
	}
	else {
		m_model.SetAnimationSpeed(1.0f);
	}

	if (m_model.GetAnimationFrame() >= 10 && m_model.GetAnimationFrame() < 25) {
		m_attack_flag = true;
		if (m_model.GetAnimationFrame() == 10) {
			sound.Play("roar");
			Base::Add(new EffekseerEffect("Roar", m_pos, CVector3D(0, 0, 0), CVector3D(1, 1, 1)));
		}
	}
	else {
		m_attack_flag = false;
	}

	if (m_model.isAnimationEnd()) {
		m_state = eState_Idle;
	}
}

void Boss::StateDamage()
{
	m_model.SetAnimationSpeed(1.3f);
	m_model.ChangeAnimation(6, false);

	if (m_model.isAnimationEnd()) {
		m_model.SetAnimationSpeed(1.0f);
		m_state = eState_Idle;
	}
}

//パリィされたとき仰け反る
void Boss::StateBreak()
{
	m_model.ChangeAnimation(8, false);
	m_model.SetAnimationSpeed(0.5f);

	if (m_model.isAnimationEnd()) {
		m_model.SetAnimationSpeed(1.0f);
		m_state = eState_Idle;
	}
}

void Boss::StateDeath()
{
	m_model.SetAnimationSpeed(1.2f);
	m_model.ChangeAnimation(7, false);

	if (m_model.isAnimationEnd()) {
		sound.Play("death");
		Base::Add(new EffekseerEffect("Death", CVector3D(m_pos.x, m_pos.y, m_pos.z), CVector3D(0, 0, 0), CVector3D(1, 1, 1)));

		SetKill();
	}
}

//イベント用の咆哮
void Boss::StateRoar()
{
	m_model.ChangeAnimation(1, false);
	m_model.SetAnimationSpeed(1.0f);
	if (m_model.GetAnimationFrame() == 51) {
		mp_camera->StartShake(0.8f, 1.5f);
		sound.Play("roar");
		Base::Add(new EffekseerEffect("Roar", m_pos, CVector3D(0, 0, 0), CVector3D(1, 1, 1)));
	}

	if (m_model.isAnimationEnd()) {
		m_state = eState_Idle;
	}
}

void Boss::Damage(Base* attaker, int damage, bool breakflag, bool leanback)
{
	m_hit++;
	m_hp -= damage;

	if (m_hp < 0) m_hp = 0;
	//HPが0以下になれば
	if (m_hp <= 0) {
		m_state = eState_Death;
	}
	else {
		if (breakflag == true) {
			m_state = eState_Break;
		}
		else if (leanback == true) {
			m_state = eState_Damage;
		}
		else if (m_state == !eState_Break && m_state == !eState_Damage) {
			if (rand() % 1000 < 550 && m_hit >= 8) {
				m_state = eState_Burst;
			}
		}
	}
}

void Boss::LookToTarget()
{
	//ターゲットが居ない、または削除された
	if (mp_target && mp_target->m_kill)
		mp_target = nullptr;

	if (!mp_target) return;

	//敵からプレイヤーへのベクトル
	CVector3D v = mp_target->m_pos - m_pos;
	//ターゲットへ向く回転値を逆算
	float a = atan2(v.x, v.z);
	m_rot.y = a;
	//方向ベクトル更新
	m_dir = CVector3D(sin(m_rot.y), 0, cos(m_rot.y));
}

void Boss::LerpTarget()
{
	CVector3D v = mp_target->m_pos - m_pos;
	v.Normalize();
	CVector3D target = CVector3D(-asin(v.y), atan2(v.x, v.z), 0);

	//一定量回転による角度補間
	m_rot.y = LerpAngle1(m_rot.y, target.y, DtoR(1));

}


//クールタイムの処理
void Boss::Recharge()
{
	if (m_trick_recharge > 0) {
		m_trick_recharge -= CFPS::GetDeltaTime();
	}

	if (m_rush_recharge > 0) {
		m_rush_recharge -= CFPS::GetDeltaTime();
	}

	if (m_jump_recharge > 0) {
		m_jump_recharge -= CFPS::GetDeltaTime();
	}

	if (m_roll_recharge > 0) {
		m_roll_recharge -= CFPS::GetDeltaTime();
	}

}

//回避
void Boss::Avoid(Player* p, Boss* b)
{
	//回避処理
	if (p->m_state == Player::eState_Sway || p->m_state == Player::eState_SwayC) {
		//もしジャスト回避できたら
		if (p->m_model.GetAnimationFrame() < 20) {
			CRendaring::GetInstance()->SetGray(true);
			CFPS::SetTimeScale(0.1);
			p->m_just_flag = true;
			b->m_act_flag = false;
			sound.Play("slowstrat");
		}
	}
}

//パリィ
void Boss::Parry(Player* p, Boss* b) {
	//パリィ処理
	if (p->m_parry_flag == true && b->m_attack_flag == true) {
		sound.Play("parry");
		p->m_just_flag = true;
		b->m_act_flag = false;
		//敵を仰け反らせる
		b->m_state = eState_Break;
	}
}

#if _DEBUG
void Boss::Draw()
{
	FONT_T()->Draw(1600, 64, 1,1,1, "ボスHP：%.0f", m_hp);
	FONT_T()->Draw(1600, 84, 1, 1, 1, "ヒット：%d", m_hit);
	FONT_T()->Draw(1600, 104, 1, 1, 1, "突進継続時間：%.0f", m_rush_cnt);
	FONT_T()->Draw(1600, 124, 1, 1, 1, "突進クール：%.1f", m_rush_recharge);
	FONT_T()->Draw(1600, 144, 1, 1, 1, "カウンタークール：%.1f", m_trick_recharge);
	FONT_T()->Draw(1600, 164, 1, 1, 1, "回避クール：%.1f", m_roll_recharge);
	FONT_T()->Draw(1600, 184, 1, 1, 1, "ジャンプクール：%.1f", m_jump_recharge);
	FONT_T()->Draw(1600, 204, 1, 1, 1, "ロック時間：%.0f", m_look_cnt);
	FONT_T()->Draw(1600, 224, 1, 1, 1, "攻撃：%s", m_attack_flag ? "true" : "false");
}
#endif
