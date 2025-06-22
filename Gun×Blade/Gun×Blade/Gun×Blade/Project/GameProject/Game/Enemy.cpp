#include "Enemy.h"
#include "Player.h"
#include "Bullet.h"
#include "../Effekseer/EffekseerEffect.h"

Enemy::Enemy(const CVector3D& pos)
	:Base(eEnemy)
{
	m_model = COPY_RESOURCE("Enemy", CModelA3M);
	//半径
	m_rad = 0.9f;
	m_hp = 250;
	//ヒットストップ初期化
	m_hit_stop = 0;
	//初期位置がプレイヤーと重ならないようにする
	m_pos = pos;
	m_jump_recharge = 0;
	m_cnt = 0;
	m_wait_time = 0;
	m_attack_pow = 0;
	m_bom_pow = 0;
	m_lineS = m_pos + CVector3D(0, 2.5 - m_rad, 0);
	m_lineE = m_pos + CVector3D(0, m_rad, 0);
	m_act_flag = false;
	m_death_flag = false;
	yaw = 0.0f;
	maxYaw = 0.0f;
	minYaw = 0.0f;
}

Enemy::Index Enemy::m_culsule_idx[eCap_Max] = {
	{16,19,0.4f},		//左腕
	{45,48,0.4f},		//右腕
	{86,89,0.4f},		//右足
};

void Enemy::Update()
{
	
	//常にプレイヤーの位置を首だけ見る
	if (m_death_flag == false) {
		LookNeckTarget();
	}

	//当たり判定カプセルの設定
	m_lineS = m_pos + CVector3D(0, 2.5 - m_rad, 0);
	m_lineE = m_pos + CVector3D(0, m_rad, 0);

	//ヒットストップ
	if (m_hit_stop > 0) {
		m_hit_stop -= CFPS::GetDeltaTime();
		return;
	}

	if (m_jump_recharge > 0) {
		m_jump_recharge -= CFPS::GetDeltaTime();
	}

	switch (m_state) {
	case eState_Idle:
		StateIdle();
		break;
	case eState_Attack1:
		StateAttack1();
		break;
	case eState_Attack2:
		StateAttack2();
		break;
	case eState_Attack_Kick:
		StateAttack_Kick();
		break;
	case eState_Attack_Jump:
		StateAttack_Jump();
		break;
	case eState_Damage:
		StateDamage();
		break;
	case eState_DamageF:
		StateDamageF();
		break;
	case eState_StundUp:
		StateStundUp();
		break;
	case eState_Break:
		StateBreak();
		break;
	case eState_Death:
		StateDeath();
		break;
	case eState_DeathF:
		StateDeathF();
		break;
	case eState_DeathS:
		StateDeathS();
		break;
	}

	m_model.UpdateAnimation();

	m_lineS = m_pos + CVector3D(0, 2.5 - m_rad, 0);
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

void Enemy::Render()
{
	m_model.SetPos(m_pos);
	m_model.SetRot(m_rot);
	m_model.SetScale(0.010, 0.010, 0.010);
	m_model.Render();

	//キャラのカプセルの表示
	//Utility::DrawCapsule(m_lineS, m_lineE, m_rad, CVector4D(1, 0, 0, 0.5));

	//部位ごとにカプセルを設定する(矩形表示用)
	for (int i = 0; i < eCap_Max; i++) {
		//Utility::DrawCapsule(m_cupsule[i].s, m_cupsule[i].e, m_cupsule[i].r, CVector4D(1, 0, 0, 0.5));
	}
}

void Enemy::Collision(Base* b)
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
			else if (t.m_normal.x > 0.5f) {
				m_bom_pow = 0;
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
	//雑魚敵同士で重ならないようにする
	case eEnemy:
	{
		//自分自身とは判定しない
		if (this != b) {
			CVector3D dir;
			float dist;
			//球での判定（カプセルより軽い）
			if (CCollision::CollisionShpere(m_lineE, m_rad, b->m_lineE, b->m_rad, &dist, &dir)) {
				//めり込み量
				float s = (m_rad + b->m_rad) - dist;
					//お互いめり込んだ分離れる
					m_pos += dir * -s / 2;
					b->m_pos += dir * s / 2;
			}
		}
	}
	break;
	case ePlayer:
	{
		CVector3D dir;
		float dist;
		//球での判定（カプセルより軽い）
		if (CCollision::CollisionShpere(m_lineE, m_rad, b->m_lineE, b->m_rad, &dist, &dir)) {
			//めり込み量
			float s = (m_rad + b->m_rad) - dist;
			//お互いめり込んだ分離れる
			m_pos += dir * -s / 2;
			b->m_pos += dir * s / 2;
		}
		//攻撃処理
		if (m_attack_flag) {
			m_act_flag = true;
			CVector3D c1;
			CVector3D pos;

			if (Player* p = dynamic_cast<Player*>(b)) {

				//プレイヤーの無敵時間
				if (p->m_state == Player::eState_Damage || p->m_state == Player::eState_DamageF || p->m_state == Player::eState_Roll || p->m_parry_flag == true) {
					break;
				}
				//右腕の攻撃
				if (m_state == eState_Attack1 || m_state == eState_Attack2) {
					//多段ヒット回避と右腕とカプセルの判定
					if (m_attack_no != b->m_hit_no && CCollision::CollisionCapsule(m_cupsule[eCap_Right_Arm].s, m_cupsule[eCap_Right_Arm].e, m_cupsule[eCap_Right_Arm].r, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)) {
						pos = c1;
						//多段ヒット回避
						b->m_hit_no = m_attack_no;

						//ヒットストップ
						m_hit_stop = 0.10f;

						Avoid(p, this);

						if (m_act_flag) {
							//ダメージ処理
							p->Damage(this, m_attack_pow,0.0f);
							sound.Play("enemyattck");
							Base::Add(new EffekseerEffect("Blow11", pos, CVector3D(0, 0, 0), CVector3D(0.5, 0.5, 0.5)));
						}
					}
				}
				//右足の攻撃
				if (m_state == eState_Attack_Kick) {
					//多段ヒット回避と左腕とカプセルの判定
					if (m_attack_no != b->m_hit_no && CCollision::CollisionCapsule(m_cupsule[eCap_Right_Leg].s, m_cupsule[eCap_Left_Arm].e, m_cupsule[eCap_Left_Arm].r, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)) {
						pos = c1;
						//多段ヒット回避
						b->m_hit_no = m_attack_no;

						//ヒットストップ
						m_hit_stop = 0.20f;

						Avoid(p, this);

						if (m_act_flag) {
							sound.Play("enemyattck");
							Base::Add(new EffekseerEffect("Blow11", pos, CVector3D(0, 0, 0), CVector3D(0.5, 0.5, 0.5)));
							p->Damage(this, m_attack_pow,0.0f);
						}
					}
				}
				//とびかかり
				if (m_state == eState_Attack_Jump) {
					//多段ヒット回避と左腕とカプセルの判定
					if (m_attack_no != b->m_hit_no && CCollision::CollisionCapsule(m_cupsule[eCap_Right_Leg].s, m_cupsule[eCap_Left_Arm].e, m_cupsule[eCap_Left_Arm].r, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)) {
						pos = c1;
						//多段ヒット回避
						b->m_hit_no = m_attack_no;

						//ヒットストップ
						m_hit_stop = 0.10f;

						Avoid(p, this);

						if (m_act_flag) {
							sound.Play("enemyattck");
							Base::Add(new EffekseerEffect("Blow11", pos, CVector3D(0, 0, 0), CVector3D(0.5, 0.5, 0.5)));
							p->Damage(this, m_attack_pow,0.0f);
						}
					}
				}
			}
		}
	}
	break;
	}
}

void Enemy::Avoid(Player* p, Enemy* b)
{
	if (p->m_state == Player::eState_Sway || p->m_state == Player::eState_SwayC) {
		//もしジャスト回避できたら
		if (p->m_model.GetAnimationFrame() < 20) {
			CRendaring::GetInstance()->SetGray(true);
			CFPS::SetTimeScale(0.1);
			p->m_just_flag = true;
			sound.Play("slowstrat");
			b->m_act_flag = false;
		}
	}
}

void Enemy::StateIdle()
{
	m_model.SetAnimationSpeed(1.0f);
	m_action_flag = false;

	if (!mp_target) {
		mp_target = Base::FindObject(ePlayer);
	}

	m_cnt -= CFPS::GetDeltaTime();

	move_speed = 0.08f;

	//他の敵が攻撃をしていたら取得する
	m_npc_act_flag = false;
	auto List = Base::FindObjects(eEnemy);
	for (auto& b : List) {
		if (Enemy* e = dynamic_cast<Enemy*>(b)) {
			if (e->m_action_flag) {
				m_npc_act_flag = true;
			}
		}
	}

	//リストからプレイヤーを探索
	if (mp_target) {
		//ターゲットへのベクトル
		vec = mp_target->m_pos - m_pos;

		if (m_target_position_offset.Length() == 0 || m_npc_act_flag == false) {
			// 敵キャラクターがプレイヤーの周りを囲むようにオフセットを設定 
			float angle = atan2(-vec.x, -vec.z) + DtoR(Utility::Rand(-60.0f, 60.0f));
			float radius = 2.5f + (rand() % 100) / 100.0f;
			// 距離を設定 
			m_target_position_offset = CVector3D(cos(angle) * radius, 0, sin(angle) * radius);
		}

		// ターゲット位置を計算
		CVector3D target_pos = mp_target->m_pos + m_target_position_offset;

		// 攻撃のためにプレイヤーに近づく
		if (!m_npc_act_flag) {
			// プレイヤーに近づくための計算 
			target_pos = mp_target->m_pos;
		}

		// ターゲット位置へのベクトル
		vec = target_pos - m_pos;

		//ベクトルから回転値を逆算
		m_rot_target.y = atan2(vec.x, vec.z);
		//回転値から方向ベクトルを計算
		m_dir = CVector3D(CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));

		//ターゲットへの距離が1.2fより遠ければ
		if (vec.Length() > 1.2f) {
			//前進
			m_pos += m_dir * move_speed * CFPS::GetTimeScale();
			//前進アニメーション
			m_model.ChangeAnimation(1);
		}
		else {
			m_model.ChangeAnimation(0);
			LookToTarget();
		}

		//他の敵が攻撃していなければ
		if (vec.Length() < 1.2f && m_npc_act_flag == false) {
			m_action_flag = true;

			if (rand() % 1000 < 150) {
				//攻撃ID変更
				m_attack_no++;
				m_state = eState_Attack1;
			}
			else if (rand() % 1000 < 30) {
				//攻撃ID変更
				m_attack_no++;
				m_state = eState_Attack_Kick;
			}
		}

		//プレイヤーとの距離が離れているかつリチャージが0の時
		//とびかかり
		if (rand() % 1000 < 50 && m_jump_recharge <= 0 && vec.Length() > 1.8f && m_npc_act_flag == false) {
			m_action_flag = true;
			m_attack_no++;
			m_state = eState_Attack_Jump;
		}

		float a = Utility::NormalizeAngle(m_rot_target.y - m_rot.y);
		//割合による補間
		m_rot.y += a * 0.1f;
	}
}

//3連撃
void Enemy::StateAttack1()
{
	m_attack_pow = 5;
	m_model.ChangeAnimation(3, false);

	if (m_model.GetAnimationFrame() > 1 && m_model.GetAnimationFrame() < 10) {
		m_model.SetAnimationSpeed(0.7f);
	}
	else {
		m_model.SetAnimationSpeed(1.0f);
	}

	//特定のフレーム間攻撃発生
	if (m_model.GetAnimationFrame() > 26 && m_model.GetAnimationFrame() < 32) {
		//攻撃フラグON
		m_attack_flag = true;
	}
	else {
		m_attack_flag = false;
	}
	
	if (m_model.isAnimationEnd()) {
		m_model.SetAnimationSpeed(1.0f);
		if (rand() % 1000 < 270) {
			m_attack_no++;
			m_state = eState_Attack2;
		}
		else {
			m_state = eState_Idle;
		}
	}
}

void Enemy::StateAttack2()
{
	m_attack_pow = 10;
	m_model.ChangeAnimation(5, false);

	if (m_model.GetAnimationFrame() > 1 && m_model.GetAnimationFrame() < 8) {
		m_model.SetAnimationSpeed(0.4f);
	}
	else {
		m_model.SetAnimationSpeed(1.0f);
	}

	//特定のフレーム間攻撃発生
	if (m_model.GetAnimationFrame() > 14 && m_model.GetAnimationFrame() < 21) {
		//攻撃フラグON
		m_attack_flag = true;
	}
	else {
		m_attack_flag = false;
	}

	if (m_model.isAnimationEnd()) {
		m_model.SetAnimationSpeed(1.0f);
		m_state = eState_Idle;
	}

}

void Enemy::StateAttack_Kick()
{
	m_attack_pow = 5;
	m_model.ChangeAnimation(2, false);

	if (m_model.GetAnimationFrame() > 1 && m_model.GetAnimationFrame() < 10) {
		m_model.SetAnimationSpeed(0.5f);
	}
	else {
		m_model.SetAnimationSpeed(1.2f);
	}

	//特定のフレーム間攻撃発生
	if (m_model.GetAnimationFrame() > 17 && m_model.GetAnimationFrame() < 22) {
		//攻撃フラグON
		m_attack_flag = true;
	}
	else {
		m_attack_flag = false;
	}

	if (m_model.isAnimationEnd()) {
		m_model.SetAnimationSpeed(1.0f);
		m_state = eState_Idle;
	}
}

//とびかかり
void Enemy::StateAttack_Jump()
{
	m_model.ChangeAnimation(6, false);
	m_attack_pow = 15;

	//とびかかる前の助走
	if (m_model.GetAnimationFrame() > 1 && m_model.GetAnimationFrame() < 10) {
		m_pos += m_dir * 0.05 * CFPS::GetTimeScale();
		m_model.SetAnimationSpeed(0.5f);
	}
	else {
		m_model.SetAnimationSpeed(1.0f);
	}

	if (m_model.GetAnimationFrame() < 11) {
		//プレイヤーの方に向く
		LookToTarget();
	}

	//飛び上がった時
	if (m_model.GetAnimationFrame() > 11 && m_model.GetAnimationFrame() < 28) {
		jmove_speed = 0.05f;
		//自身からプレイヤーへのベクトル
		v = mp_target->m_pos - m_pos;
		//プレイヤーにとびかかる
		m_pos += v* jmove_speed * CFPS::GetTimeScale();
	}

	//特定のフレーム間攻撃発生
	if (m_model.GetAnimationFrame() > 31 && m_model.GetAnimationFrame() < 36) {
		//攻撃フラグON
		m_attack_flag = true;
	}
	else {
		m_attack_flag = false;
	}

	if (m_model.isAnimationEnd()) {
		m_state = eState_Idle;
		m_jump_recharge = 15.0f;
	}
}

void Enemy::StateBreak()
{
	m_model.SetAnimationSpeed(0.5f);
	m_model.ChangeAnimation(7, false);

	if (m_model.isAnimationEnd()) {
		m_model.SetAnimationSpeed(1.0f);
		m_state = eState_Idle;
	}
}

void Enemy::StateDamage()
{
	m_model.ChangeAnimation(7, false);
	m_model.SetAnimationSpeed(1.3f);
	m_action_flag = false;

	if (m_model.isAnimationEnd()) {
		m_model.SetAnimationSpeed(1.0f);
		m_state = eState_Idle;
	}
}
//吹き飛ばされる
void Enemy::StateDamageF()
{
	m_model.SetAnimationSpeed(1.2f);
	m_model.ChangeAnimation(9, false);

	//プレイヤーが攻撃してきた方向へ向く
	if (m_model.GetAnimationFrame() == 0) {
		LookToTarget();
	}

	if (m_model.GetAnimationFrame() < 29) {
		m_pos -= m_dir * m_bom_pow * CFPS::GetTimeScale();
	}

	if (m_model.isAnimationEnd()) {
		m_state = eState_StundUp;
	}
}

//立ち上がる
void Enemy::StateStundUp()
{
	m_model.ChangeAnimation(10, false);
	m_model.SetAnimationSpeed(1.5f);

	if (m_model.isAnimationEnd()) {
		m_model.SetAnimationSpeed(1.0f);
		m_state = eState_Idle;
	}
}

//吹き飛ばされて倒される
void Enemy::StateDeathF()
{
	m_model.SetAnimationSpeed(1.2f);
	m_model.ChangeAnimation(9, false);
	m_death_flag = true;

	//プレイヤーが攻撃してきた方向へ向く
	if (m_model.GetAnimationFrame() == 0) {
		LookToTarget();
	}

	if (m_model.GetAnimationFrame() < 29) {
		m_pos -= m_dir * m_bom_pow * CFPS::GetTimeScale();
	}

	if (m_model.isAnimationEnd()) {
		sound.Play("death");
		Base::Add(new EffekseerEffect("Death", CVector3D(m_pos.x, m_pos.y, m_pos.z), CVector3D(0, 0, 0), CVector3D(1, 1, 1)));

		SetKill();
	}
}

//立ち上がる前に倒される
void Enemy::StateDeathS()
{
	m_model.ChangeAnimation(11, false);
	m_death_flag = true;

	if (m_model.GetAnimationFrame() < 6) {
		m_model.SetAnimationSpeed(0.3f);
	}
	else {
		m_model.SetAnimationSpeed(1.3f);
	}

	if (m_model.isAnimationEnd()) {
		sound.Play("death");
		Base::Add(new EffekseerEffect("Death", CVector3D(m_pos.x, m_pos.y, m_pos.z), CVector3D(0, 0, 0), CVector3D(1, 1, 1)));

		SetKill();
	}
}

void Enemy::StateDeath()
{
	m_model.SetAnimationSpeed(1.2f);
	m_model.ChangeAnimation(8, false);
	m_death_flag = true;

	if (m_model.isAnimationEnd()) {
		sound.Play("death");
		Base::Add(new EffekseerEffect("Death", CVector3D(m_pos.x, m_pos.y, m_pos.z), CVector3D(0, 0, 0), CVector3D(1, 1, 1)));

		SetKill();
	}
}

//(攻撃者、ダメージ量、吹き飛ぶ力、吹き飛ばされるかどうか)
void Enemy::Damage(Base* attaker, int damage, float pow, bool breakflag)
{
	m_action_flag = false;
	m_hp -= damage;
	m_bom_pow = pow;
	m_model.BindFrameMatrix(8, CMatrix::MRotation(CVector3D(0, 0, 0)));

	if (m_hp < 0) m_hp = 0;
	//HPが0以下になれば
	if (m_hp <= 0) {
		if (m_death_flag == false) {
			if (m_bom_pow <= 0) {
				if (m_state == eState_StundUp || m_state == eState_DamageF) {
					m_state = eState_DeathS;
				}
				else {
					m_state = eState_Death;
				}
			}
			else {
				m_state = eState_DeathF;
			}
		}
	}
	else {
		if (breakflag == true) {
			m_state = eState_DamageF;
		}
		else if (m_state == eState_StundUp || m_state == eState_DamageF) {
			return;
		}
		else {
			m_state = eState_Damage;
		}
	}
}

void Enemy::LookToTarget()
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

//首の方向のみプレイヤーへ向ける
void Enemy::LookNeckTarget()
{
	if (mp_target && mp_target->m_kill) {
		mp_target = nullptr;
	}

	if (!mp_target) return;

	// 敵からプレイヤーへのベクトル
	CVector3D v = mp_target->m_pos - m_pos;

	// ターゲットへ向く回転値を逆算 
	yaw = atan2(v.x, v.z);

	//首の向きを取得
	CVector3D parent_dir = m_model.GetFrameMatrix(7).GetFront();
	float parent_rot = atan2(parent_dir.x, parent_dir.z);
	float a = Utility::NormalizeAngle(yaw - parent_rot);

	// 回転値の上限を±60度に制限
	maxYaw = DtoR(60.0f);
	minYaw = DtoR(-60.0f);

	if (a < minYaw) {
		a = minYaw;
	}
	else if (a > maxYaw) {
		a = maxYaw;
	}

	yaw = Utility::NormalizeAngle(parent_rot + a);

	//プレイヤーの方向へ首を動かす
	m_model.BindFrameMatrix(8, CMatrix::MRotation(CVector3D(0, yaw, 0)));
}

#if _DEBUG
void Enemy::Draw()
{
	FONT_T()->Draw(1700, 64, 1, 1, 1, "敵HP：%.0f", m_hp);
	FONT_T()->Draw(1700, 84, 1, 1, 1, "%s", m_action_flag ? "true" : "false");
	FONT_T()->Draw(1700, 104, 1, 1, 1, "ジャンプ：%.0f", m_jump_recharge);
	FONT_T()->Draw(1700, 124, 1, 1, 1, "X座標：%.0f", m_pos.x);
	FONT_T()->Draw(1700, 144, 1, 1, 1, "Y座標：%.0f", m_pos.y);
	FONT_T()->Draw(1700, 164, 1, 1, 1, "Z座標：%.0f", m_pos.z);
	FONT_T()->Draw(1700, 184, 1, 1, 1, "首の回転値：%.0f", yaw);

}
#endif
