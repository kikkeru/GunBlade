#include "Player.h"
#include "Boss.h"
#include "Enemy.h"
#include "Effect.h"
#include "../Effekseer/EffekseerEffect.h"
#include "UI/Gauge.h"
#include "UI/BulletCapacity.h"
#include "Gun.h"
#include "FollowCamera.h"
#include "Bullet.h"
#include "../GLLibrary/GLLibrary/CFPS.h"

#define SPINE 5

Player::Player(const CVector3D& pos)
	:Base(ePlayer) {
	m_model = COPY_RESOURCE("Player", CModelA3M);
	m_model.m_time_scale = false;
	m_pos.x = -2.0f;
	//半径
	m_rad = 0.35f;
	m_hp = 300.0f;
	m_hp_max = 300.0f;
	m_bullet = 24.0f;
	m_bullet_max = 24.0f;
	//ヒットストップ初期化
	m_hit_stop = 0;
	m_shot_cnt = 0.0f;
	m_reload = 0;
	m_shot = 0;
	m_out_cnt = 0.0f;
	m_just_cnt = 5.0f;
	m_auto_chage_cnt = 5.0f;
	m_just_flag = false;
	m_change_flag = false;
	m_stand_flag = false;
	m_foot_effect_index = 0;
	m_hit_sword1 = false;
	m_hit_sword2 = false;
	m_sound = 0;
	c1 = CVector3D(0, 0, 0);
	dist = 0;
	m_reload_flag = false;

	//アニメーションを分けるかどうか
	ChangeAnimationLayer(0);
	mp_camera = dynamic_cast<FollowCamera*> (Base::FindObject(eCamera));

	//ゲージ生成
	Base::Add(m_gauge = new Gauge(Gauge::GaugeType::ePlayerGauge, 0.3f));
	Base::Add(m_capacity_gauge = new BulletCapacity(BulletCapacity::GaugeType::eBulletsGauge, 0.3f));

	//武器モデル取得
	m_wepon_sword = COPY_RESOURCE("Sword", CModelObj);
	m_wepon_gun = COPY_RESOURCE("Gun", CModelObj);
}

void Player::Update() {
	//イベントカメラ中操作を受け付けない
	if (Base::FindObject(eEventCamera)) {
		//どの状態で入っても剣状態に戻るようにしておく
		m_state = eState_Idle;
		m_dash_flag = false;
		m_change_flag = false;
		return;
	}

	//ジャストタイムの管理
	if (m_just_flag == true) {
		//ジャストタイム終了までのカウント
		if (m_just_cnt > 0) {
			m_just_cnt -= CFPS::GetDeltaTime();
		}
		else {
			m_just_flag = false;
			m_just_cnt = 5.0f;
			CFPS::SetTimeScale(1.0);
			CRendaring::GetInstance()->SetGray(false);
		}
	}

	if (m_bullet <= 0) {
		m_bullet = 0;
	}

	//弾自動回復
	if (m_bullet < 24){
		if (m_auto_chage_cnt > 0) {
			m_auto_chage_cnt -= CFPS::GetDeltaTime();
		}
		else {
			m_bullet += 1.0f;
			m_auto_chage_cnt = 5.0f;
		}
	}
	else {
		m_auto_chage_cnt = 5.0f;
	}

	//銃状態、リロードならアニメーションを分ける
	if (m_state == eState_Aim || m_state == eState_Reload || m_state == eState_ShotR || m_state == eState_ShotL) {
		ChangeAnimationLayer(1);
	}
	else {
		ChangeAnimationLayer(0);
	}

	//ターゲット指定
	if (!mp_target) {
		mp_target = Base::FindObject(eEnemy);
		LockOnTargetE(5);
	}

	if (!mp_target) {
		mp_target = Base::FindObject(eBoss);
		LockOnTargetB(5);
	}

	KeyUpdate();

	//当たり判定カプセルの設定
	m_lineS = m_pos + CVector3D(0, 1.8 - m_rad, 0);
	m_lineE = m_pos + CVector3D(0, m_rad, 0);

	//ヒットストップ
	if (m_hit_stop > 0) {
		m_hit_stop -= CFPS::GetDeltaTime();
		return;
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
	case eState_Attack3:
		StateAttack3();
		break;
	case eState_AttackB1:
		StateAttackB1();
		break;
	case eState_AttackB2:
		StateAttackB2();
		break;
	case eState_AttackG:
		StateAttackG();
		break;
	case eState_AttackD:
		StateAttackD();
		break;
	case eState_Aim:
		StateAim();
		break;
	case eState_Reload:
		StateReload();
		break;
	case eState_Sway:
		StateSway();
		break;
	case eState_SwayC:
		StateSwayC();
		break;
	case eState_Roll:
		StateRoll();
		break;
	case eState_RollG:
		StateRollG();
		break;
	case eState_Damage:
		StateDamage();
		break;
	case eState_DamageF:
		StateDamageF();
		break;
	case eState_Death:
		StateDeath();
		break;
	case eState_DeathF:
		StateDeathF();
		break;
	}

	m_model.UpdateAnimation();

	//画面左上にHPゲージの位置を設定
	if (m_gauge) {
		m_gauge->m_pos = CVector3D(0.0f, 50.0f, 0.0f);
		m_gauge->SetValue((float)m_hp / m_hp_max);
	}

	//画面左上に装弾数の位置を設定
	if (m_capacity_gauge) {
		m_capacity_gauge->m_pos = CVector3D(0.0f, 130.0f, 0.0f);
		m_capacity_gauge->SetValue((float)m_bullet / m_bullet_max);
	}

	//移動入力がなければスピードを０にする
	if (m_key_dir.LengthSq() < 0.1) {
		move_speed = 0;
	}

	//重力落下
	m_vec.y -= GRAVITY;
	m_pos.y += m_vec.y;

}

void Player::Render()
{
	m_model.SetPos(m_pos);
	m_model.SetRot(m_rot);
	m_model.SetScale(0.01, 0.01, 0.01);
	m_model.Render();
	CModelObj* old = m_my_wepon;

	//銃モード
	if (m_change_flag == true) {
		//武器の表示とカプセルの設定
		//右手
		m_wepon_matrix = m_model.GetFrameMatrix(44)
			* CMatrix::MTranselate(0.0f, 5.0f, 0.0f)
			* CMatrix::MRotationX(DtoR(0))
			* CMatrix::MRotationY(DtoR(90))
			* CMatrix::MRotationZ(DtoR(0))
			* CMatrix::MScale(0.15f, 0.15f, 0.15f);
		m_wepon_gun.Render(m_wepon_matrix);

		//左手
		m_wepon_matrix = m_model.GetFrameMatrix(15)
			* CMatrix::MTranselate(0.0f, 5.0f, 0.0f)
			* CMatrix::MRotationX(DtoR(90))
			* CMatrix::MRotationY(DtoR(270))
			* CMatrix::MRotationZ(DtoR(90))
			* CMatrix::MScale(0.15f, 0.15f, 0.15f);
		m_wepon_gun.Render(m_wepon_matrix);

		m_my_wepon = &m_wepon_gun;
	}
	//剣モード
	else {
		//剣の設定
		//右手
		m_wepon_matrix = m_model.GetFrameMatrix(44)
			* CMatrix::MTranselate(0.0f, 5.0f, 0.0f)
			* CMatrix::MRotationX(DtoR(0))
			* CMatrix::MRotationY(DtoR(90))
			* CMatrix::MRotationZ(DtoR(90))
			* CMatrix::MScale(0.5f, 0.5f, 0.5f);
		m_wepon_sword.Render(m_wepon_matrix);

		//剣の当たり判定
		m_wepon_rad = 0.1f;
		m_swordS = (m_wepon_matrix * CMatrix::MTranselate(0, 0, 20)).GetPosition();
		m_swordE = (m_wepon_matrix * CMatrix::MTranselate(0, 0, 200)).GetPosition();

		//左手
		m_wepon_matrix2 = m_model.GetFrameMatrix(15)
			* CMatrix::MTranselate(0.0f, 5.0f, 0.0f)
			* CMatrix::MRotationX(DtoR(0))
			* CMatrix::MRotationY(DtoR(270))
			* CMatrix::MRotationZ(DtoR(90))
			* CMatrix::MScale(0.5f, 0.5f, 0.5f);
		m_wepon_sword.Render(m_wepon_matrix2);

		m_my_wepon = &m_wepon_sword;

		//剣の当たり判定
		m_wepon_rad2 = 0.1f;
		m_swordS2 = (m_wepon_matrix2 * CMatrix::MTranselate(0, 0, 20)).GetPosition();
		m_swordE2 = (m_wepon_matrix2 * CMatrix::MTranselate(0, 0, 200)).GetPosition();
	}
	//武器を変えたら武器のα値を0にリセット
	if (m_my_wepon != old) {
		m_wepon_alpha = 0.0f;
	}
	//武器が徐々に見えてくる
	m_wepon_alpha = min(m_wepon_alpha + CFPS::GetDeltaTime() * 1.0, 1.0);
	//武器のα値を設定
	for (int i = 0; i < m_my_wepon->GetMaterialSize(); i++) {
		CMaterial* m = m_my_wepon->GetMaterial(i);
		m->m_alpha = m_wepon_alpha;
	}
	//武器表示
	m_my_wepon->Render(m_wepon_matrix);
}

void Player::Collision(Base* b)
{
	switch (b->GetType()) {
	case eField:
	{
		//押し戻し量
		CVector3D v(0, 0, 0);
		//カプセルとモデルの衝突
		auto tri = b->GetModel()->CollisionCupsel(m_lineS,//始点（頭）
			m_lineE,//終点（足元）
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
				if (m_vec.y < 0) {
					m_vec.y = 0;
				}
			}
			//壁に当たったら吹き飛ぶ力を0にして貫通しないようにする
			else if (t.m_normal.x > 0.5f) {
				m_burst_pow = 0;
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
	case eBoss:
	{
		CVector3D dir;
		float dist;
		//球での判定（カプセルより軽い）
		if (CCollision::CollisionShpere(m_lineE, m_rad, b->m_lineE, b->m_rad, &dist, &dir)) {
			//めり込み量
			float s = (m_rad + b->m_rad) - dist;
			//押し戻しベクトルを定義
			CVector3D nv = dir * -s;

			//吹き飛ばされたらボスに運ばれないように座標を左右にずらす
			if (m_state == eState_DamageF) {
				//横方向へのずれを追加
				if (dir.x != 0 || dir.z != 0) {
					//横方向にずれる量
					float sideShift = 0.3f;
					//Z方向に横ずれ
					nv.x += sideShift * (dir.z > 0 ? 1 : -1);
					//X方向に横ずれ
					nv.z += sideShift * (dir.x > 0 ? -1 : 1);
				}

				//位置更新
				m_pos += nv;
			}
			//吹き飛ばされてなければめり込んでないかだけの処理
			else {
				m_pos += dir * -s;
			}

		}
		//攻撃処理
		if (m_attack_flag) {
			CVector3D c1;
			CVector3D pos;
			float dist;
			//剣とカプセルの判定
			if (CCollision::CollisionCapsule(m_swordS, m_swordE, m_wepon_rad, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)) {
				//多段ヒット回避
				if (m_attack_no != b->m_hit_no) {
					b->m_hit_no = m_attack_no;
					
					//ボスの処理
					if (Boss* e = dynamic_cast<Boss*>(b)) {
						if (e->m_state == e->eState_Death) break;

						//エフェクトの発生位置
						pos = c1;

						//攻撃を受けない
						if (e->m_state == Boss::eState_Step || e->m_state == Boss::eState_AttackS2) {
							break;
						}

						//空中にいる間は無敵
						if (e->m_state == e->eState_AttackJump) {
							if (e->m_model.GetAnimationFrame() > 17 && e->m_model.GetAnimationFrame() < 45) {
								break;
							}
						}

						//ヒット時プレイヤーは0.10s停止
						m_hit_stop = 0.10f;

						//爆破攻撃がヒット
						if (m_state == eState_AttackB2) {
							m_bullet -= 5.0f;
							if (e->m_state == !Boss::eState_AttackS1 || e->m_state == !Boss::eState_AttackS3) {
								e->Damage(this, m_attack_pow, true, false);
							}
							sound.Play("bom");
							Base::Add(new EffekseerEffect("Fire2", pos, CVector3D(0, 0, 0), CVector3D(1, 1, 1)));
							break;
						}

						//突き刺し時敵にヒットしたら
						if (m_state == eState_AttackB1 && b->m_hit_no == m_attack_no) {
							sound.Play("stab");
							m_hit_flag = true;
							if (e->m_state == !Boss::eState_AttackS1 || e->m_state == !Boss::eState_AttackS3) {
								e->m_model.SetAnimationSpeed(0.5f);
								e->Damage(this, m_attack_pow, false, true);
								break;
							}
						}

						//特定の攻撃が当たれば仰け反る
						if (m_state == eState_Attack3 || m_state == eState_AttackD) {
							//ボスが特定の技を出していなければ
							if (e->m_state == !Boss::eState_AttackRush || e->m_state == !Boss::eState_AttackS1 || e->m_state == !Boss::eState_AttackS3) {
								e->Damage(this, m_attack_pow, false, true);
								sound.Play("attack");
								//切りエフェクト
								Base::Add(new EffekseerEffect("Sword1", pos, CVector3D(0, 0, 0), CVector3D(1, 1, 1)));
								break;
							}
						}

						//仰け反らない
						e->Damage(this, m_attack_pow, false, false);

						sound.Play("attack");
						//切りエフェクト
						Base::Add(new EffekseerEffect("Sword1", pos, CVector3D(0, 0, 0), CVector3D(1, 1, 1)));
					}
				}
			}
		}
	}
	break;
	case eEnemy:
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
			CVector3D pos;

			//剣とカプセルの判定
			if (CCollision::CollisionCapsule(m_swordS, m_swordE, m_wepon_rad, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)) {
				//多段ヒット回避
				if (m_attack_no != b->m_hit_no) {
					//多段ヒット回避
					b->m_hit_no = m_attack_no;

					//雑魚のダメージ処理
					if (Enemy* s = dynamic_cast<Enemy*>(b)) {
						//倒されたら判定しない
						if (s->m_death_flag) break;

						//エフェクトの発生位置
						pos = c1;

						//ヒット時プレイヤーは0.10s停止
						m_hit_stop = 0.10f;

						//ゲージ消費技がヒットしたら
						if (m_state == eState_AttackG || m_state == eState_RollG) {
							s->Damage(this, m_attack_pow, 0.04f, true);
							sound.Play("attack");
							Base::Add(new EffekseerEffect("Sword1", pos, CVector3D(0, 0, 0), CVector3D(1, 1, 1)));
							break;
						}

						//爆破攻撃がヒットしたら
						if (m_state == eState_AttackB2) {
							m_bullet -= 5.0f;
							sound.Play("bom");
							Base::Add(new EffekseerEffect("Fire2", m_swordE, CVector3D(0, 0, 0), CVector3D(1, 1, 1)));
							s->Damage(this, m_attack_pow, 0.08f, true);
							break;
						}

						//突き刺し時敵にヒットしたら
						if (m_state == eState_AttackB1 && b->m_hit_no == m_attack_no) {
							sound.Play("stab");
							m_hit_flag = true;
							s->m_model.SetAnimationSpeed(0.5f);
							s->Damage(this, m_attack_pow, 0.0f, false);
							break;
						}

						s->Damage(this, m_attack_pow, 0.0f, false);
						sound.Play("attack");
						//切りエフェクト
						Base::Add(new EffekseerEffect("Sword1", pos, CVector3D(0, 0, 0), CVector3D(1, 1, 1)));
					}
				}
			}
		}
	}
	break;
	}
}

void Player::StateIdle() {
	m_model.SetAnimationSpeed(1.0f);
	m_parry_flag = false;
	m_attack_flag = false;
	m_hit_flag = false;
	float jump_pow = 1;
	key_length = 0;
	//カメラの回転値を取得する
	cam_ang = 0;
	m_state_flag = false;

	if (Base* b = Base::FindObject(eCamera))
	{
		cam_ang = b->m_rot.y;
	}

	KeyMove();

	//カウントリセット
	if (PUSH(CInput::eMouseL)) {
		m_out_cnt = 0;
	}

	//ボタンを押し続けるとカウントされていく
	if (HOLD(CInput::eMouseL)) {
		m_out_cnt++;
		//一定以上カウントされると自動的に発生
		if (m_out_cnt > 15) {
			//攻撃ID変更
			m_attack_no++;
			m_out_cnt = 0;
			//突き刺し
			m_state = eState_AttackB1;
		}
	}

	//カウント内にボタンを離すと通常攻撃
	if (PULL(CInput::eMouseL)) {
		if (m_out_cnt <= 15) {
			//攻撃
			if (move_speed < 0.08) {
				//攻撃ID変更
				m_attack_no++;
				m_out_cnt = 0;
				m_state = eState_Attack1;
			}
			else {
				//攻撃ID変更
				m_attack_no++;
				m_out_cnt = 0;
				m_state = eState_AttackD;
			}
		}
	}

	//ゲージ消費攻撃
	if (HOLD(CInput::eMouseR) && PUSH(CInput::eMouseL) && m_bullet >= 5) {
		//攻撃ID変更
		m_attack_no++;
		m_state = eState_AttackG;
	}

	//モード切替
	if (PUSH(CInput::eMouseC)) {
		m_change_flag = true;
		m_state = eState_Aim;
	}

	//ゲージ消費回避
	if (HOLD(CInput::eMouseR) && PUSH(CInput::eButton5) && m_bullet >= 2.0f) {
		//攻撃ID変更
		m_attack_no++;
		m_state = eState_RollG;
	}
	//回避
	else if (PUSH(CInput::eButton5)) {
		m_state = eState_Sway;
	}

}

void Player::KeyMove() {
	//入力があれば
	if (m_key_dir.LengthSq() > 0.1) {
		int f[] = { 0,10 };

		//ダッシュ判定
		if (HOLD(CInput::eButton6) || m_dash_flag == true) {
			m_dash_flag = true;
			move_speed = 0.18;
			m_model.ChangeAnimation(2);

			if (m_model.GetAnimationFrame() == 8) {
				sound.Play("runL");
			}
			else if (m_model.GetAnimationFrame() == 18) {
				sound.Play("runR");
			}
		}
		else {
			move_speed = 0.08;
			m_model.ChangeAnimation(1);

			if (m_model.GetAnimationFrame() == 4) {
				sound.Play("runL");
			}
			else if (m_model.GetAnimationFrame() == 12) {
				sound.Play("runR");
			}
		}

		//足元にエフェクトを出す
		if (f[m_foot_effect_index] == m_model.GetAnimationFrame()) {
			//足のつま先のボーンを取る
			int boneindex[] = { 81,75 };

			CVector3D p = m_model.GetFrameMatrix(boneindex[m_foot_effect_index]).GetPosition();

			Base::Add(new EffekseerEffect("foot_smoke1", p, CVector3D(0, 0, 0), CVector3D(1, 1, 1)));

			m_foot_effect_index = (m_foot_effect_index + 1) % 2;
		}

		key_length = min(1.0, m_key_dir.Length());
		//キーの方向ベクトル逆算
		float key_ang = atan2(m_key_dir.x, m_key_dir.z);
		//カメラの角度＋キーの角度でキャラクターの角度が決まる
		m_rot.y = Utility::NormalizeAngle(cam_ang + key_ang);
		//方向ベクトル
		m_dir = CVector3D(sin(m_rot.y), 0, cos(m_rot.y));
		//移動
		m_pos += m_dir * move_speed;

		m_model.SetAnimationSpeed(key_length);
	}
	else {
		m_foot_effect_index = 0;
		m_dash_flag = false;
		m_model.ChangeAnimation(0);
		m_model.SetAnimationSpeed(1.0);
	}
}

//銃状態時の移動処理
void Player::AimMove()
{
	move_speed = 0.05;
	CMatrix rotMtx = CMatrix::MRotationY(m_rot.y);
	float s = 1.0f;

	if (m_key_dir.LengthSq() > 0) {
		//移動処理 回転行列×移動方向
		m_dir = rotMtx * m_key_dir;
		if (m_aim_state == 0) {

		}
		else {
			s *= 0.5f;
		}
		m_pos += m_dir * move_speed * s;
		int anim = -1;
		float key_ang = atan2(m_key_dir.x, m_key_dir.z);
		//前
		if (key_ang > DtoR(-30) && key_ang < DtoR(30)) {
			anim = 0;
		}
		//右
		else if (key_ang <= DtoR(-30) && key_ang >= DtoR(-135)) {
			anim = 1;
		}
		//左
		else if (key_ang >= DtoR(+30) && key_ang <= DtoR(+135)) {
			anim = 2;
		}
		//後ろ
		else {
			anim = 3;
		}
		m_model.ChangeAnimation(19 + anim);

		if (m_model.GetAnimationFrame(0) == 9) {
			sound.Play("walkL");
		}
		else if (m_model.GetAnimationFrame(0) == 25) {
			sound.Play("walkR");
		}
	}
	else {
		m_model.ChangeAnimation(26);
	}

}

void Player::KeyUpdate()
{
	//方向キーのベクトル
	m_key_dir = CVector3D(0, 0, 0);
	if (HOLD(CInput::eLeft)) {
		m_key_dir.x = 1;
	}
	if (HOLD(CInput::eRight)) {
		m_key_dir.x = -1;
	}
	if (HOLD(CInput::eUp)) {
		m_key_dir.z = 1;
	}
	if (HOLD(CInput::eDown)) {
		m_key_dir.z = -1;
	}
}

//三連撃
void Player::StateAttack1() {
	move_speed = 0.02f;
	m_attack_pow = 10;
	m_model.ChangeAnimation(7,false);
	m_model.SetAnimationSpeed(1.6);

	if (m_model.GetAnimationFrame() > 22 && m_model.GetAnimationFrame() < 36) {
		m_pos += m_dir * move_speed;
	}

	//特定のフレーム間攻撃発生
	if (m_model.GetAnimationFrame() > 17 && m_model.GetAnimationFrame() < 34) {
		//攻撃フラグON
		m_attack_flag = true;
	}
	else {
		m_attack_flag = false;
	}

	if (m_model.isAnimationEnd()) {
		m_state = eState_Idle;
	}
	else if (m_model.GetAnimationFrame() > 30 && HOLD(CInput::eMouseL)) {
		//攻撃ID変更
		m_attack_no++;
		m_state = eState_Attack2;
	}
	else {
		if (HOLD(CInput::eButton5)) {
			m_state = eState_SwayC;
		}
	}
}

void Player::StateAttack2() {
	m_attack_pow = 10;
	m_model.ChangeAnimation(8, false);

	//特定のフレーム間攻撃発生
	if (m_model.GetAnimationFrame() > 7 && m_model.GetAnimationFrame() < 12) {
		//攻撃フラグON
		m_attack_flag = true;
	}
	else {
		m_attack_flag = false;
	}

	if (m_model.isAnimationEnd()) {
		m_state = eState_Idle;
		m_model.SetAnimationSpeed(1.0);
	}
	else if (m_model.GetAnimationFrame() > 12 && HOLD(CInput::eMouseL)) {
		//攻撃ID変更
		m_attack_no++;
		m_state = eState_Attack3;
	}
	else {
		if (HOLD(CInput::eButton5)) {
			m_state = eState_SwayC;
		}
	}
}

void Player::StateAttack3() {
	move_speed = 0.02f;
	m_attack_pow = 30;
	m_model.ChangeAnimation(9, false);

	if (m_model.GetAnimationFrame() > 1 && m_model.GetAnimationFrame() < 16) {
		m_pos += m_dir * move_speed;
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
		m_state = eState_Idle;
		m_model.SetAnimationSpeed(1.0);
	}
	else if (m_model.GetAnimationFrame() > 24) {
		//ゲージ消費攻撃派生
		if (HOLD(CInput::eMouseR) && HOLD(CInput::eMouseL) && m_bullet >= 5) {
			//攻撃ID変更
			m_attack_no++;
			m_state = eState_AttackG;
		}

		if (HOLD(CInput::eButton5)) {
			m_state = eState_SwayC;
		}
	}
}

//ダッシュアタック
void Player::StateAttackD()
{
	m_dash_flag = false;
	m_attack_pow = 30;
	m_model.ChangeAnimation(10, false);

	if (m_model.GetAnimationFrame() > 1 && m_model.GetAnimationFrame() < 8) {
		m_pos += m_dir * move_speed;
	}

	//特定のフレーム間攻撃発生
	if (m_model.GetAnimationFrame() > 3 && m_model.GetAnimationFrame() < 10) {
		//攻撃フラグON
		m_attack_flag = true;
	}
	else {
		m_attack_flag = false;
	}

	if (m_model.isAnimationEnd()) {
		m_state = eState_Idle;
	}
	else if (m_model.GetAnimationFrame() > 10 && m_key_dir.LengthSq() > 0.1) {
		if (HOLD(CInput::eButton5)) {
			m_state = eState_Roll;
		}
	}
}

//突き刺し
void Player::StateAttackB1()
{
	m_attack_pow = 15;

	if (move_speed > 0.08f) {
		m_attack_pow = 15;
		m_model.ChangeAnimation(11, false);
		m_dash_flag = false;

		//特定のフレーム間攻撃発生
		if (m_model.GetAnimationFrame() > 1 && m_model.GetAnimationFrame() < 17) {
			//攻撃フラグON
			m_attack_flag = true;
		}
		else {
			m_attack_flag = false;
		}
	}
	else {
		m_model.ChangeAnimation(12, false);

		//特定のフレーム間攻撃発生
		if (m_model.GetAnimationFrame() > 3 && m_model.GetAnimationFrame() < 12) {
			//攻撃フラグON
			m_attack_flag = true;
		}
		else {
			m_attack_flag = false;
		}
	}

	//突き刺しがヒットしていたら爆破へ派生
	if (m_model.GetAnimationFrame() > 12 && m_model.GetAnimationFrame() < 23 && HOLD(CInput::eMouseL) && m_bullet >= 5 && m_hit_flag == true) {
		m_attack_no++;
		m_state = eState_AttackB2;
	}

	if (m_model.isAnimationEnd()) {
		m_state = eState_Idle;
	}
	else if (m_model.GetAnimationFrame() > 22 && m_key_dir.LengthSq() > 0.1) {
		if (HOLD(CInput::eButton5)) {
			m_state = eState_Roll;
		}
		else {
			m_state = eState_Idle;
		}
	}
}

//爆破
void Player::StateAttackB2()
{
	m_recoil = 0.08;
	m_attack_pow = 100;
	m_model.ChangeAnimation(13, false);

	//反動で下がる
	if (m_model.GetAnimationFrame() > 0 && m_model.GetAnimationFrame() < 26) {
		m_pos -= m_dir * m_recoil;
	}

	//特定のフレーム間攻撃発生
	if (m_model.GetAnimationFrame() > 2 && m_model.GetAnimationFrame() < 5) {
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

//ゲージ消費攻撃
void Player::StateAttackG() {
	m_dash_flag = false;
	m_attack_pow = 80;
	m_model.SetAnimationSpeed(2.0f);
	m_model.ChangeAnimation(14, false);

	if (m_model.GetAnimationFrame() == 1) {
		m_bullet -= 5.0f;
		sound.Play("gaugeattack");
		Base::Add(new EffekseerEffect("Rotarycut", CVector3D(m_pos.x, 0.5, m_pos.z), CVector3D(0, 0, 0), CVector3D(1, 1, 1)));
	}

	//パリィ発生
	if (m_model.GetAnimationFrame() > 0 && m_model.GetAnimationFrame() < 10) {
		m_parry_flag = true;
	}
	else {
		m_parry_flag = false;
	}


	//特定のフレーム間攻撃発生
	if (m_model.GetAnimationFrame() > 3 && m_model.GetAnimationFrame() < 15) {
		//攻撃フラグON
		m_attack_flag = true;
	}
	else {
		m_attack_flag = false;
	}

	if (m_model.isAnimationEnd()) {
		m_state = eState_Idle;
	}
	else if (m_model.GetAnimationFrame() > 18) {
		if (HOLD(CInput::eButton5)) {
			m_state = eState_Roll;
		}
	}
}

//ゲージ消費回避
void Player::StateRollG() {
	m_dash_flag = false;
	m_attack_pow = 25;
	roll_speed = 0.3f;
	m_model.SetAnimationSpeed(2.0f);
	m_model.ChangeAnimation(14, false);

	if (m_model.GetAnimationFrame() == 1) {
		m_bullet -= 2.0f;
		sound.Play("gaugeattack");
		Base::Add(new EffekseerEffect("Rotarycut", CVector3D(m_pos.x, 0.5, m_pos.z), CVector3D(0, 0, 0), CVector3D(1, 1, 1)));
	}

	//パリィ判定
	if (m_model.GetAnimationFrame() > 0 && m_model.GetAnimationFrame() < 10) {
		m_parry_flag = true;
	}
	else {
		m_parry_flag = false;
	}

	//攻撃発生
	if (m_model.GetAnimationFrame() > 3 && m_model.GetAnimationFrame() < 15) {
		//攻撃フラグON
		m_attack_flag = true;
	}
	else {
		m_attack_flag = false;
	}

	if (m_model.GetAnimationFrame() < 15 && m_key_dir.LengthSq() > 0.1) {
		//プレイヤーが向いている方向へ移動しながら攻撃
		m_pos += m_dir * roll_speed;
	}

	if (m_model.isAnimationEnd()) {
		roll_speed = 0.0f;
		m_state = eState_Idle;
	}else if (m_model.GetAnimationFrame() > 12 && m_key_dir.LengthSq() > 0.1) {
		if ((HOLD(CInput::eMouseR) && m_bullet >= 4)) {
			m_state = eState_RollG;
		}
	}
}

//武器切り替え
void Player::StateAim()
{
	ChangeAnimationLayer(1);
	move_speed = 0.05f;
	m_state_flag = true;
	m_dash_flag = false;
	CVector3D target;

	AimMove();

	if (Base* b = Base::FindObject(eCamera))
	{
		m_rot.y = b->m_rot.y;
	}

	//マウスの中ボタンを押せば状態が切り替わる
	if (PUSH(CInput::eMouseC)) {
		ChangeAnimationLayer(0);
		m_aim_state = 0;
		m_change_flag = false;
		m_state = eState_Idle;

		if (mp_gun) {
			mp_gun->SetKill();
			mp_gun = nullptr;
		}
	}

	//リロード
	if (m_bullet <= 23.0f) {
		if (PUSH(CInput::eButton13) || PUSH(CInput::eMouseL) && m_bullet <= 0.0f) {
			m_state = eState_Reload;
		}
		else {
			m_model.ChangeAnimation(1, 25, false);
		}
	}

	//回避
	if (PUSH(CInput::eButton5)) {
		//回避する前にキー入力の方向をベクトル逆算する
		m_rot.y = atan2(m_dir.x, m_dir.z);
		m_state = eState_Roll;
	}

	//ロックオンする敵を変更
	if (PUSH(CInput::eButton12)) {
		LockOnTargetE(10, eDirRight);
		m_lock_flag = true;
	}

	//フラグがtrueならロックオンする
	if (m_lock_flag == true) {
		LookToTarget();
		//Eキーで解除
		if (PUSH(CInput::eButton11)) {
			m_lock_flag = false;
		}
	}

	//構える
	if (HOLD(CInput::eMouseR)) {
		m_model.ChangeAnimation(1, 18, false);
		m_stand_flag = true;
	}
	else {
		//ボタンを離したら戻る
		m_model.ChangeAnimation(1, 25, false);
		m_stand_flag = false;
	}

	if (m_shot_cnt > 0.0f) {
		m_shot_cnt -= 1.0f;
	}

	//銃を撃つ
	if (HOLD(CInput::eMouseL) && HOLD(CInput::eMouseR) && m_shot_cnt == 0.0f && m_bullet > 0.0f) {

		switch (m_shot)
		{
			//右
		case 0:
			sound.Play("shot");
			Base::Add(new Bullet(m_model.GetFrameMatrix(44) * CVector4D(0, 0, 0, 1), CMatrix::MRotation(m_rot).GetFront(), 1.0));
			m_shot++;
			break;
			//左
		case 1:
			sound.Play("shot");
			Base::Add(new Bullet(m_model.GetFrameMatrix(15) * CVector4D(0, 0, 0, 1), CMatrix::MRotation(m_rot).GetFront(), 1.0));
			m_shot--;
			break;
		}

		//弾を撃つ間隔
		m_shot_cnt = 15.0f;
		m_bullet -= 1.0f;
	}
}

//リロード
void Player::StateReload()
{
	m_model.ChangeAnimation(1, 29, false);
	AimMove();

	if (m_reload_flag == false) {
		sound.Play("reloads");
		m_reload_flag = true;
	}


	if (Base* b = Base::FindObject(eCamera))
	{
		m_rot.y = b->m_rot.y;
	}

	if (m_model.isAnimationEnd(1)) {
		m_state = eState_Aim;
		m_bullet = 24.0f;
		m_reload_flag = false;
	}
	else if (PUSH(CInput::eButton5)) {
		m_reload_flag = false;
		//回避する前にキー入力の方向をベクトル逆算する
		m_rot.y = atan2(m_dir.x, m_dir.z);
		m_state = eState_Roll;
	}
}

//スウェイ回避
void Player::StateSway()
{
	m_model.SetAnimationSpeed(1.8f);
	//後ろ回避
	if (key_length <= 0) {
		//スウェイの速さ
		float roll_speed = 0.07f;
		m_model.ChangeAnimation(17, false);

		if (m_model.GetAnimationFrame() < 26) {
			//プレイヤーが向いている方向へ回避
			m_pos -= m_dir * roll_speed;
		}
		//スウェイし終わったら
		if (m_model.isAnimationEnd()) {
			roll_speed = 0.0f;
			m_state = eState_Idle;
		}
	}
	//回避
	else {
		//スウェイ回避の速さ
		roll_speed = 0.12f;
		m_model.ChangeAnimation(15, false);

		if (m_model.GetAnimationFrame() < 30) {
			//プレイヤーが向いている方向へ回避
			m_pos += m_dir * roll_speed;
		}

		//スウェイ回避し終わったら
		if (m_model.isAnimationEnd()) {
			roll_speed = 0.0f;
			m_state = eState_Idle;
		}
		//回避中にもう一度ボタンを押せば転がり回避につながる
		else if (m_model.GetAnimationFrame() > 22 && m_key_dir.LengthSq() > 0.1) {
			if (HOLD(CInput::eButton5)) {
				m_dash_flag = false;
				m_state = eState_Roll;
			}
			else if (m_model.GetAnimationFrame() > 30) {
				m_dash_flag = false;
			}
			//移動キーを押していればダッシュにつながる
			else {
				m_dash_flag = true;
				roll_speed = 0.0f;
				m_state = eState_Idle;
			}
		}
	}
}

//攻撃キャンセルスウェイ
void Player::StateSwayC()
{
	m_attack_flag = false;
	m_model.SetAnimationSpeed(1.8f);
	CMatrix rotMtx = CMatrix::MRotationY(m_rot.y);
	//スウェイの速さ
	float roll_speed = 0.06f;

	if (m_key_dir.LengthSq() > 0) {
		//移動処理 回転行列×移動方向
		m_dir = rotMtx * m_key_dir;

		m_pos += m_dir * roll_speed;
		int anim = -1;
		float key_ang = atan2(m_key_dir.x, m_key_dir.z);
		//前
		if (key_ang > DtoR(-30) && key_ang < DtoR(30)) {
			anim = 0;
		}
		//右
		else if (key_ang <= DtoR(-30) && key_ang >= DtoR(-135)) {
			m_model.ChangeAnimation(28, false);
		}
		//左
		else if (key_ang >= DtoR(+30) && key_ang <= DtoR(+135)) {
			m_model.ChangeAnimation(27, false);
		}
		//後ろ
		else{
			m_model.ChangeAnimation(17, false);
		}
	}

	//スウェイし終わったら
	if (m_model.isAnimationEnd()) {
		m_model.SetAnimationSpeed(1.0f);
		roll_speed = 0.0f;
		m_state = eState_Idle;
	}
}

//回避
void Player::StateRoll()
{
	m_model.SetAnimationSpeed(1.6f);
	//ローリングの速さ
	roll_speed = 0.10f;
	m_model.ChangeAnimation(16, false);

	if (m_model.GetAnimationFrame() < 30) {
		m_pos += m_dir * roll_speed;
	}

	if (m_model.isAnimationEnd()) {
		m_model.SetAnimationSpeed(1.0f);
		roll_speed = 0.0f;

		//銃状態ならAimに戻る
		if (m_state_flag == true) {
			m_state = eState_Aim;
		}
		//剣状態ならIdleに戻る
		else {
			m_state = eState_Idle;
		}
	}
}

void Player::StateDamage()
{
	m_model.ChangeAnimation(4, false);
	m_dash_flag = false;

	if (m_model.isAnimationEnd()){
		if (m_state_flag == true) {
			m_state = eState_Aim;
		}else{
			m_state = eState_Idle;
		}
	}
}

//吹き飛ばされる
void Player::StateDamageF()
{
	m_model.ChangeAnimation(5, false);
	m_dash_flag = false;

	if (m_model.GetAnimationFrame() < 1) {
		LookToTarget();
		mp_camera->StartShake(0.5f, 1.0f);
	}

	if (m_model.GetAnimationFrame() < 30) {
		//吹き飛ばされる方向
		m_pos -= m_dir * m_burst_pow;
	}

	//起き上がりに少し時間をかける
	if (m_model.GetAnimationFrame() > 36 && m_model.GetAnimationFrame() < 40) {
		m_model.SetAnimationSpeed(0.3f);
	}
	else {
		m_model.SetAnimationSpeed(1.0f);
	}

	if (m_model.isAnimationEnd()) {
		if (m_state_flag == true) {
			m_state = eState_Aim;
		}
		else {
			m_state = eState_Idle;
		}
	}
}

void Player::StateDeath()
{
	m_model.SetAnimationSpeed(1.3f);
	m_model.ChangeAnimation(6, false);
	if (m_model.isAnimationEnd()) {
		SetKill();
	}
}

void Player::StateDeathF()
{
	m_model.ChangeAnimation(30, false);

	if (m_model.GetAnimationFrame() < 1) {
		LookToTarget();
		mp_camera->StartShake(0.7f, 1.0f);
	}

	if (m_model.GetAnimationFrame() < 30) {
		//吹き飛ばされる方向
		m_pos -= m_dir * m_burst_pow;
	}

	if (m_model.isAnimationEnd()) {
		SetKill();
	}

}

void Player::Damage(Base* attaker, int damage, float burstpow)
{
	m_hp -= damage;
	m_burst_pow = burstpow;

	if (m_hp < 0) m_hp = 0;
	if (m_hp <= 0) {
		if (m_burst_pow > 0.0f) {
			m_state = eState_DeathF;
		}
		else {
			m_state = eState_Death;
		}
	}
	else {
		if (m_burst_pow > 0.0f) {
			m_state = eState_DamageF;
		}
		else {
			m_state = eState_Damage;
		}
	}
}

void Player::ChangeAnimationLayer(int layer)
{
	for (int i = SPINE; i <= 69; i++) {
		m_model.GetNode(i)->SetAnimationLayer(layer);
	}
}

#if _DEBUG
//デバッグ用
void Player::Draw()
{
	FONT_T()->Draw(0, 204, 1, 1, 1, "ジャストタイム継続時間：%.1f", m_just_cnt);
	FONT_T()->Draw(0, 224, 1, 1, 1, "X座標：%f", m_pos.x);
	FONT_T()->Draw(0, 244, 1, 1, 1, "Y座標：%f", m_pos.y);
	FONT_T()->Draw(0, 264, 1, 1, 1, "Z座標：%f", m_pos.z);
	FONT_T()->Draw(0, 284, 1, 1, 1, "移動スピード：%f", move_speed);
	FONT_T()->Draw(0, 304, 1, 1, 1, "残り弾数：%.0f", m_bullet);
	FONT_T()->Draw(0, 324, 1, 1, 1, "ゲージ処理：%f", m_bullet / m_bullet_max);
	FONT_T()->Draw(0, 344, 1, 1, 1, "HP処理：%f", m_hp / m_hp_max);
	FONT_T()->Draw(0, 364, 1, 1, 1, "弾自動回復まで：%f", m_auto_chage_cnt);
}
#endif

void Player::LockOnTargetE(float range)
{

	//敵をリストで取得
	auto list = Base::FindObjects(eEnemy);
	//ターゲット
	Base* t = nullptr;
	//距離角度差の最小値
	float min = 10;
	//プレイヤーの方向
	CVector3D dir(sin(m_rot.y), 0, cos(m_rot.y));
	for (auto b : list) {
		//プレイヤーから敵へのベクトル
		CVector3D v = b->m_pos - (m_pos - dir * 0.5f);
		//プレイヤーから敵への距離
		float l = v.Length();
		//プレイヤーから敵への方向
		CVector3D d = v / l;
		//プレイヤーの正面方向と敵への方向のなす角度
		float a = acos(min(1.0f, max(-1.0f, CVector3D::Dot(d, dir))));
		//60度以上は無視
		if (a > DtoR(60.0f)) continue;
		//距離が近く、正面に近い敵が残る
		l = l * (a / 5 + 1.0f);
		if (l > min) continue;
		min = l;
		t = b;
	}
	mp_target = t;
}

void Player::LockOnTargetB(float range)
{
	//ボスをリストで取得
	auto list = Base::FindObjects(eBoss);
	//ターゲット
	Base* t = nullptr;
	//距離角度差の最小値
	float min = 10;
	//プレイヤーの方向
	CVector3D dir(sin(m_rot.y), 0, cos(m_rot.y));
	for (auto b : list) {
		//プレイヤーから敵へのベクトル
		CVector3D v = b->m_pos - (m_pos - dir * 0.5f);
		//プレイヤーから敵への距離
		float l = v.Length();
		//プレイヤーから敵への方向
		CVector3D d = v / l;
		//プレイヤーの正面方向と敵への方向のなす角度
		float a = acos(min(1.0f, max(-1.0f, CVector3D::Dot(d, dir))));
		//60度以上は無視
		if (a > DtoR(60.0f)) continue;
		//距離が近く、正面に近い敵が残る
		l = l * (a / 5 + 1.0f);
		if (l > min) continue;
		min = l;
		t = b;
	}
	mp_target = t;

}

void Player::LookToTarget()
{
	//ターゲットが居ない、または削除された
	if (mp_target && mp_target->m_kill)
		mp_target = nullptr;
	if (!mp_target) return;
	//プレイヤーからターゲットへのベクトル
	CVector3D v = mp_target->m_pos - m_pos;
	//ターゲットへ向く回転値を逆算
	float a = atan2(v.x, v.z);
	m_rot.y = a;
	//方向ベクトル更新
	m_dir = CVector3D(sin(m_rot.y), 0, cos(m_rot.y));

	//ロックオンエフェクト
	//Base::Add(new CEffect("CircleR", mp_target, CVector3D(0, 0, 0), CVector3D(0, 0, 0), 0.2f, 0.4f, -0.1f, CEffect::eBlendAlpha, 1, CVector3D(0, 0, 0), CVector3D(0, 0, 0), 10));
}

void Player::LockOnTargetE(float range, int lockdir)
{
	//敵をリストで取得
	auto list = Base::FindObjects(eEnemy);
	//ターゲット
	Base* t = nullptr;
	//距離角度の最小値
	float min = FLT_MAX;
	//距離角度が最も離れている
	Base* rev_t = nullptr;
	//距離角度の最大値
	float rev = FLT_MAX;
	//敵ループ
	for (auto b : list) {
		//カメラから敵へのベクトル
		CVector3D v = b->m_pos - m_pos;
		//カメラから敵への距離
		float l = v.Length();
		//カメラから敵への方向
		CVector3D d = v / l;
		//カメラから敵への方向から角度を計算
		float a = atan2(d.x, d.z);
		//カメラの角度と敵の角度の差分
		a = Utility::NormalizeAngle(a - m_rot.y);
		//ロックオン
		if (lockdir == eDirCenter) {
			//左右関係なく中心に近い敵を選ぶ
			a = abs(a);
			//正面で距離の近い敵を選ぶように
			l = l * (a / 5 + 0.01f);
		}
		else {
			//ターゲット左右への切り替え
			//現在のてターゲットとは参照しない
			if (b == mp_target) continue;
			//敵が切り替えたい方向に居ると、aは＋に
			//判定方向だとaは-に
			a *= lockdir;
			l = a;
		}
		//正面に近い敵を残す
		if (l < min && a>0) {
			min = l;
			t = b;
		}
		else
			//最も遠い敵
			if (l < rev) {
				rev = l;
				rev_t = b;
			}
	}

	//ターゲット確定
	if (t) {
		mp_target = t;
	}
	else {
		mp_target = rev_t;
	}

}

