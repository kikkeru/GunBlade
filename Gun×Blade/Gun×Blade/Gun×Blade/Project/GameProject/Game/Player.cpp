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
	//���a
	m_rad = 0.35f;
	m_hp = 300.0f;
	m_hp_max = 300.0f;
	m_bullet = 24.0f;
	m_bullet_max = 24.0f;
	//�q�b�g�X�g�b�v������
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

	//�A�j���[�V�����𕪂��邩�ǂ���
	ChangeAnimationLayer(0);
	mp_camera = dynamic_cast<FollowCamera*> (Base::FindObject(eCamera));

	//�Q�[�W����
	Base::Add(m_gauge = new Gauge(Gauge::GaugeType::ePlayerGauge, 0.3f));
	Base::Add(m_capacity_gauge = new BulletCapacity(BulletCapacity::GaugeType::eBulletsGauge, 0.3f));

	//���탂�f���擾
	m_wepon_sword = COPY_RESOURCE("Sword", CModelObj);
	m_wepon_gun = COPY_RESOURCE("Gun", CModelObj);
}

void Player::Update() {
	//�C�x���g�J������������󂯕t���Ȃ�
	if (Base::FindObject(eEventCamera)) {
		//�ǂ̏�Ԃœ����Ă�����Ԃɖ߂�悤�ɂ��Ă���
		m_state = eState_Idle;
		m_dash_flag = false;
		m_change_flag = false;
		return;
	}

	//�W���X�g�^�C���̊Ǘ�
	if (m_just_flag == true) {
		//�W���X�g�^�C���I���܂ł̃J�E���g
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

	//�e������
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

	//�e��ԁA�����[�h�Ȃ�A�j���[�V�����𕪂���
	if (m_state == eState_Aim || m_state == eState_Reload || m_state == eState_ShotR || m_state == eState_ShotL) {
		ChangeAnimationLayer(1);
	}
	else {
		ChangeAnimationLayer(0);
	}

	//�^�[�Q�b�g�w��
	if (!mp_target) {
		mp_target = Base::FindObject(eEnemy);
		LockOnTargetE(5);
	}

	if (!mp_target) {
		mp_target = Base::FindObject(eBoss);
		LockOnTargetB(5);
	}

	KeyUpdate();

	//�����蔻��J�v�Z���̐ݒ�
	m_lineS = m_pos + CVector3D(0, 1.8 - m_rad, 0);
	m_lineE = m_pos + CVector3D(0, m_rad, 0);

	//�q�b�g�X�g�b�v
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

	//��ʍ����HP�Q�[�W�̈ʒu��ݒ�
	if (m_gauge) {
		m_gauge->m_pos = CVector3D(0.0f, 50.0f, 0.0f);
		m_gauge->SetValue((float)m_hp / m_hp_max);
	}

	//��ʍ���ɑ��e���̈ʒu��ݒ�
	if (m_capacity_gauge) {
		m_capacity_gauge->m_pos = CVector3D(0.0f, 130.0f, 0.0f);
		m_capacity_gauge->SetValue((float)m_bullet / m_bullet_max);
	}

	//�ړ����͂��Ȃ���΃X�s�[�h���O�ɂ���
	if (m_key_dir.LengthSq() < 0.1) {
		move_speed = 0;
	}

	//�d�͗���
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

	//�e���[�h
	if (m_change_flag == true) {
		//����̕\���ƃJ�v�Z���̐ݒ�
		//�E��
		m_wepon_matrix = m_model.GetFrameMatrix(44)
			* CMatrix::MTranselate(0.0f, 5.0f, 0.0f)
			* CMatrix::MRotationX(DtoR(0))
			* CMatrix::MRotationY(DtoR(90))
			* CMatrix::MRotationZ(DtoR(0))
			* CMatrix::MScale(0.15f, 0.15f, 0.15f);
		m_wepon_gun.Render(m_wepon_matrix);

		//����
		m_wepon_matrix = m_model.GetFrameMatrix(15)
			* CMatrix::MTranselate(0.0f, 5.0f, 0.0f)
			* CMatrix::MRotationX(DtoR(90))
			* CMatrix::MRotationY(DtoR(270))
			* CMatrix::MRotationZ(DtoR(90))
			* CMatrix::MScale(0.15f, 0.15f, 0.15f);
		m_wepon_gun.Render(m_wepon_matrix);

		m_my_wepon = &m_wepon_gun;
	}
	//�����[�h
	else {
		//���̐ݒ�
		//�E��
		m_wepon_matrix = m_model.GetFrameMatrix(44)
			* CMatrix::MTranselate(0.0f, 5.0f, 0.0f)
			* CMatrix::MRotationX(DtoR(0))
			* CMatrix::MRotationY(DtoR(90))
			* CMatrix::MRotationZ(DtoR(90))
			* CMatrix::MScale(0.5f, 0.5f, 0.5f);
		m_wepon_sword.Render(m_wepon_matrix);

		//���̓����蔻��
		m_wepon_rad = 0.1f;
		m_swordS = (m_wepon_matrix * CMatrix::MTranselate(0, 0, 20)).GetPosition();
		m_swordE = (m_wepon_matrix * CMatrix::MTranselate(0, 0, 200)).GetPosition();

		//����
		m_wepon_matrix2 = m_model.GetFrameMatrix(15)
			* CMatrix::MTranselate(0.0f, 5.0f, 0.0f)
			* CMatrix::MRotationX(DtoR(0))
			* CMatrix::MRotationY(DtoR(270))
			* CMatrix::MRotationZ(DtoR(90))
			* CMatrix::MScale(0.5f, 0.5f, 0.5f);
		m_wepon_sword.Render(m_wepon_matrix2);

		m_my_wepon = &m_wepon_sword;

		//���̓����蔻��
		m_wepon_rad2 = 0.1f;
		m_swordS2 = (m_wepon_matrix2 * CMatrix::MTranselate(0, 0, 20)).GetPosition();
		m_swordE2 = (m_wepon_matrix2 * CMatrix::MTranselate(0, 0, 200)).GetPosition();
	}
	//�����ς����畐��̃��l��0�Ƀ��Z�b�g
	if (m_my_wepon != old) {
		m_wepon_alpha = 0.0f;
	}
	//���킪���X�Ɍ����Ă���
	m_wepon_alpha = min(m_wepon_alpha + CFPS::GetDeltaTime() * 1.0, 1.0);
	//����̃��l��ݒ�
	for (int i = 0; i < m_my_wepon->GetMaterialSize(); i++) {
		CMaterial* m = m_my_wepon->GetMaterial(i);
		m->m_alpha = m_wepon_alpha;
	}
	//����\��
	m_my_wepon->Render(m_wepon_matrix);
}

void Player::Collision(Base* b)
{
	switch (b->GetType()) {
	case eField:
	{
		//�����߂���
		CVector3D v(0, 0, 0);
		//�J�v�Z���ƃ��f���̏Փ�
		auto tri = b->GetModel()->CollisionCupsel(m_lineS,//�n�_�i���j
			m_lineE,//�I�_�i�����j
			m_rad);
		//�ڐG�����ʂ̐��J��Ԃ�
		for (auto& t : tri) {
			if (t.m_normal.y < -0.5f) {
				//�ʂ����������V��ɓ�������
				//�㏸���x��0�ɖ߂�
				if (m_vec.y > 0)
					m_vec.y = 0;
			}
			else if (t.m_normal.y > 0.5f) {
				//�ʂ���������n�ʂɓ�������
				//�d�͗������x��0�ɖ߂�
				if (m_vec.y < 0) {
					m_vec.y = 0;
				}
			}
			//�ǂɓ��������琁����ԗ͂�0�ɂ��Ċђʂ��Ȃ��悤�ɂ���
			else if (t.m_normal.x > 0.5f) {
				m_burst_pow = 0;
			}
			float max_y = max(t.m_vertex[0].y, max(t.m_vertex[1].y, t.m_vertex[2].y));
			//�ڐG�����ʂ̕����ցA�߂荞�񂾕������߂�
			CVector3D nv = t.m_normal * (m_rad - t.m_dist);
			//�ł��傫�Ȉړ��ʂ����߂�
			v.y = fabs(v.y) > fabs(nv.y) ? v.y : nv.y;
			//�G���܂ł͏��z����
			//�G��ȏ�̕ǂ̂݉����߂����
			if (max_y > m_pos.y + 0.2f) {
				v.x = fabs(v.x) > fabs(nv.x) ? v.x : nv.x;
				v.z = fabs(v.z) > fabs(nv.z) ? v.z : nv.z;
			}
		}
		//�����߂�
		m_pos += v;
	}
	break;
	case eBoss:
	{
		CVector3D dir;
		float dist;
		//���ł̔���i�J�v�Z�����y���j
		if (CCollision::CollisionShpere(m_lineE, m_rad, b->m_lineE, b->m_rad, &dist, &dir)) {
			//�߂荞�ݗ�
			float s = (m_rad + b->m_rad) - dist;
			//�����߂��x�N�g�����`
			CVector3D nv = dir * -s;

			//������΂��ꂽ��{�X�ɉ^�΂�Ȃ��悤�ɍ��W�����E�ɂ��炷
			if (m_state == eState_DamageF) {
				//�������ւ̂����ǉ�
				if (dir.x != 0 || dir.z != 0) {
					//�������ɂ�����
					float sideShift = 0.3f;
					//Z�����ɉ�����
					nv.x += sideShift * (dir.z > 0 ? 1 : -1);
					//X�����ɉ�����
					nv.z += sideShift * (dir.x > 0 ? -1 : 1);
				}

				//�ʒu�X�V
				m_pos += nv;
			}
			//������΂���ĂȂ���΂߂荞��łȂ��������̏���
			else {
				m_pos += dir * -s;
			}

		}
		//�U������
		if (m_attack_flag) {
			CVector3D c1;
			CVector3D pos;
			float dist;
			//���ƃJ�v�Z���̔���
			if (CCollision::CollisionCapsule(m_swordS, m_swordE, m_wepon_rad, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)) {
				//���i�q�b�g���
				if (m_attack_no != b->m_hit_no) {
					b->m_hit_no = m_attack_no;
					
					//�{�X�̏���
					if (Boss* e = dynamic_cast<Boss*>(b)) {
						if (e->m_state == e->eState_Death) break;

						//�G�t�F�N�g�̔����ʒu
						pos = c1;

						//�U�����󂯂Ȃ�
						if (e->m_state == Boss::eState_Step || e->m_state == Boss::eState_AttackS2) {
							break;
						}

						//�󒆂ɂ���Ԃ͖��G
						if (e->m_state == e->eState_AttackJump) {
							if (e->m_model.GetAnimationFrame() > 17 && e->m_model.GetAnimationFrame() < 45) {
								break;
							}
						}

						//�q�b�g���v���C���[��0.10s��~
						m_hit_stop = 0.10f;

						//���j�U�����q�b�g
						if (m_state == eState_AttackB2) {
							m_bullet -= 5.0f;
							if (e->m_state == !Boss::eState_AttackS1 || e->m_state == !Boss::eState_AttackS3) {
								e->Damage(this, m_attack_pow, true, false);
							}
							sound.Play("bom");
							Base::Add(new EffekseerEffect("Fire2", pos, CVector3D(0, 0, 0), CVector3D(1, 1, 1)));
							break;
						}

						//�˂��h�����G�Ƀq�b�g������
						if (m_state == eState_AttackB1 && b->m_hit_no == m_attack_no) {
							sound.Play("stab");
							m_hit_flag = true;
							if (e->m_state == !Boss::eState_AttackS1 || e->m_state == !Boss::eState_AttackS3) {
								e->m_model.SetAnimationSpeed(0.5f);
								e->Damage(this, m_attack_pow, false, true);
								break;
							}
						}

						//����̍U����������΋�����
						if (m_state == eState_Attack3 || m_state == eState_AttackD) {
							//�{�X������̋Z���o���Ă��Ȃ����
							if (e->m_state == !Boss::eState_AttackRush || e->m_state == !Boss::eState_AttackS1 || e->m_state == !Boss::eState_AttackS3) {
								e->Damage(this, m_attack_pow, false, true);
								sound.Play("attack");
								//�؂�G�t�F�N�g
								Base::Add(new EffekseerEffect("Sword1", pos, CVector3D(0, 0, 0), CVector3D(1, 1, 1)));
								break;
							}
						}

						//������Ȃ�
						e->Damage(this, m_attack_pow, false, false);

						sound.Play("attack");
						//�؂�G�t�F�N�g
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
		//���ł̔���i�J�v�Z�����y���j
		if (CCollision::CollisionShpere(m_lineE, m_rad, b->m_lineE, b->m_rad, &dist, &dir)) {
			//�߂荞�ݗ�
			float s = (m_rad + b->m_rad) - dist;
			//���݂��߂荞�񂾕������
			m_pos += dir * -s / 2;
			b->m_pos += dir * s / 2;
		}
		//�U������
		if (m_attack_flag) {
			CVector3D pos;

			//���ƃJ�v�Z���̔���
			if (CCollision::CollisionCapsule(m_swordS, m_swordE, m_wepon_rad, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)) {
				//���i�q�b�g���
				if (m_attack_no != b->m_hit_no) {
					//���i�q�b�g���
					b->m_hit_no = m_attack_no;

					//�G���̃_���[�W����
					if (Enemy* s = dynamic_cast<Enemy*>(b)) {
						//�|���ꂽ�画�肵�Ȃ�
						if (s->m_death_flag) break;

						//�G�t�F�N�g�̔����ʒu
						pos = c1;

						//�q�b�g���v���C���[��0.10s��~
						m_hit_stop = 0.10f;

						//�Q�[�W����Z���q�b�g������
						if (m_state == eState_AttackG || m_state == eState_RollG) {
							s->Damage(this, m_attack_pow, 0.04f, true);
							sound.Play("attack");
							Base::Add(new EffekseerEffect("Sword1", pos, CVector3D(0, 0, 0), CVector3D(1, 1, 1)));
							break;
						}

						//���j�U�����q�b�g������
						if (m_state == eState_AttackB2) {
							m_bullet -= 5.0f;
							sound.Play("bom");
							Base::Add(new EffekseerEffect("Fire2", m_swordE, CVector3D(0, 0, 0), CVector3D(1, 1, 1)));
							s->Damage(this, m_attack_pow, 0.08f, true);
							break;
						}

						//�˂��h�����G�Ƀq�b�g������
						if (m_state == eState_AttackB1 && b->m_hit_no == m_attack_no) {
							sound.Play("stab");
							m_hit_flag = true;
							s->m_model.SetAnimationSpeed(0.5f);
							s->Damage(this, m_attack_pow, 0.0f, false);
							break;
						}

						s->Damage(this, m_attack_pow, 0.0f, false);
						sound.Play("attack");
						//�؂�G�t�F�N�g
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
	//�J�����̉�]�l���擾����
	cam_ang = 0;
	m_state_flag = false;

	if (Base* b = Base::FindObject(eCamera))
	{
		cam_ang = b->m_rot.y;
	}

	KeyMove();

	//�J�E���g���Z�b�g
	if (PUSH(CInput::eMouseL)) {
		m_out_cnt = 0;
	}

	//�{�^��������������ƃJ�E���g����Ă���
	if (HOLD(CInput::eMouseL)) {
		m_out_cnt++;
		//���ȏ�J�E���g�����Ǝ����I�ɔ���
		if (m_out_cnt > 15) {
			//�U��ID�ύX
			m_attack_no++;
			m_out_cnt = 0;
			//�˂��h��
			m_state = eState_AttackB1;
		}
	}

	//�J�E���g���Ƀ{�^���𗣂��ƒʏ�U��
	if (PULL(CInput::eMouseL)) {
		if (m_out_cnt <= 15) {
			//�U��
			if (move_speed < 0.08) {
				//�U��ID�ύX
				m_attack_no++;
				m_out_cnt = 0;
				m_state = eState_Attack1;
			}
			else {
				//�U��ID�ύX
				m_attack_no++;
				m_out_cnt = 0;
				m_state = eState_AttackD;
			}
		}
	}

	//�Q�[�W����U��
	if (HOLD(CInput::eMouseR) && PUSH(CInput::eMouseL) && m_bullet >= 5) {
		//�U��ID�ύX
		m_attack_no++;
		m_state = eState_AttackG;
	}

	//���[�h�ؑ�
	if (PUSH(CInput::eMouseC)) {
		m_change_flag = true;
		m_state = eState_Aim;
	}

	//�Q�[�W������
	if (HOLD(CInput::eMouseR) && PUSH(CInput::eButton5) && m_bullet >= 2.0f) {
		//�U��ID�ύX
		m_attack_no++;
		m_state = eState_RollG;
	}
	//���
	else if (PUSH(CInput::eButton5)) {
		m_state = eState_Sway;
	}

}

void Player::KeyMove() {
	//���͂������
	if (m_key_dir.LengthSq() > 0.1) {
		int f[] = { 0,10 };

		//�_�b�V������
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

		//�����ɃG�t�F�N�g���o��
		if (f[m_foot_effect_index] == m_model.GetAnimationFrame()) {
			//���̂ܐ�̃{�[�������
			int boneindex[] = { 81,75 };

			CVector3D p = m_model.GetFrameMatrix(boneindex[m_foot_effect_index]).GetPosition();

			Base::Add(new EffekseerEffect("foot_smoke1", p, CVector3D(0, 0, 0), CVector3D(1, 1, 1)));

			m_foot_effect_index = (m_foot_effect_index + 1) % 2;
		}

		key_length = min(1.0, m_key_dir.Length());
		//�L�[�̕����x�N�g���t�Z
		float key_ang = atan2(m_key_dir.x, m_key_dir.z);
		//�J�����̊p�x�{�L�[�̊p�x�ŃL�����N�^�[�̊p�x�����܂�
		m_rot.y = Utility::NormalizeAngle(cam_ang + key_ang);
		//�����x�N�g��
		m_dir = CVector3D(sin(m_rot.y), 0, cos(m_rot.y));
		//�ړ�
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

//�e��Ԏ��̈ړ�����
void Player::AimMove()
{
	move_speed = 0.05;
	CMatrix rotMtx = CMatrix::MRotationY(m_rot.y);
	float s = 1.0f;

	if (m_key_dir.LengthSq() > 0) {
		//�ړ����� ��]�s��~�ړ�����
		m_dir = rotMtx * m_key_dir;
		if (m_aim_state == 0) {

		}
		else {
			s *= 0.5f;
		}
		m_pos += m_dir * move_speed * s;
		int anim = -1;
		float key_ang = atan2(m_key_dir.x, m_key_dir.z);
		//�O
		if (key_ang > DtoR(-30) && key_ang < DtoR(30)) {
			anim = 0;
		}
		//�E
		else if (key_ang <= DtoR(-30) && key_ang >= DtoR(-135)) {
			anim = 1;
		}
		//��
		else if (key_ang >= DtoR(+30) && key_ang <= DtoR(+135)) {
			anim = 2;
		}
		//���
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
	//�����L�[�̃x�N�g��
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

//�O�A��
void Player::StateAttack1() {
	move_speed = 0.02f;
	m_attack_pow = 10;
	m_model.ChangeAnimation(7,false);
	m_model.SetAnimationSpeed(1.6);

	if (m_model.GetAnimationFrame() > 22 && m_model.GetAnimationFrame() < 36) {
		m_pos += m_dir * move_speed;
	}

	//����̃t���[���ԍU������
	if (m_model.GetAnimationFrame() > 17 && m_model.GetAnimationFrame() < 34) {
		//�U���t���OON
		m_attack_flag = true;
	}
	else {
		m_attack_flag = false;
	}

	if (m_model.isAnimationEnd()) {
		m_state = eState_Idle;
	}
	else if (m_model.GetAnimationFrame() > 30 && HOLD(CInput::eMouseL)) {
		//�U��ID�ύX
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

	//����̃t���[���ԍU������
	if (m_model.GetAnimationFrame() > 7 && m_model.GetAnimationFrame() < 12) {
		//�U���t���OON
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
		//�U��ID�ύX
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

	//����̃t���[���ԍU������
	if (m_model.GetAnimationFrame() > 14 && m_model.GetAnimationFrame() < 21) {
		//�U���t���OON
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
		//�Q�[�W����U���h��
		if (HOLD(CInput::eMouseR) && HOLD(CInput::eMouseL) && m_bullet >= 5) {
			//�U��ID�ύX
			m_attack_no++;
			m_state = eState_AttackG;
		}

		if (HOLD(CInput::eButton5)) {
			m_state = eState_SwayC;
		}
	}
}

//�_�b�V���A�^�b�N
void Player::StateAttackD()
{
	m_dash_flag = false;
	m_attack_pow = 30;
	m_model.ChangeAnimation(10, false);

	if (m_model.GetAnimationFrame() > 1 && m_model.GetAnimationFrame() < 8) {
		m_pos += m_dir * move_speed;
	}

	//����̃t���[���ԍU������
	if (m_model.GetAnimationFrame() > 3 && m_model.GetAnimationFrame() < 10) {
		//�U���t���OON
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

//�˂��h��
void Player::StateAttackB1()
{
	m_attack_pow = 15;

	if (move_speed > 0.08f) {
		m_attack_pow = 15;
		m_model.ChangeAnimation(11, false);
		m_dash_flag = false;

		//����̃t���[���ԍU������
		if (m_model.GetAnimationFrame() > 1 && m_model.GetAnimationFrame() < 17) {
			//�U���t���OON
			m_attack_flag = true;
		}
		else {
			m_attack_flag = false;
		}
	}
	else {
		m_model.ChangeAnimation(12, false);

		//����̃t���[���ԍU������
		if (m_model.GetAnimationFrame() > 3 && m_model.GetAnimationFrame() < 12) {
			//�U���t���OON
			m_attack_flag = true;
		}
		else {
			m_attack_flag = false;
		}
	}

	//�˂��h�����q�b�g���Ă����甚�j�֔h��
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

//���j
void Player::StateAttackB2()
{
	m_recoil = 0.08;
	m_attack_pow = 100;
	m_model.ChangeAnimation(13, false);

	//�����ŉ�����
	if (m_model.GetAnimationFrame() > 0 && m_model.GetAnimationFrame() < 26) {
		m_pos -= m_dir * m_recoil;
	}

	//����̃t���[���ԍU������
	if (m_model.GetAnimationFrame() > 2 && m_model.GetAnimationFrame() < 5) {
		//�U���t���OON
		m_attack_flag = true;
	}
	else {
		m_attack_flag = false;
	}

	if (m_model.isAnimationEnd()) {
		m_state = eState_Idle;
	}
}

//�Q�[�W����U��
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

	//�p���B����
	if (m_model.GetAnimationFrame() > 0 && m_model.GetAnimationFrame() < 10) {
		m_parry_flag = true;
	}
	else {
		m_parry_flag = false;
	}


	//����̃t���[���ԍU������
	if (m_model.GetAnimationFrame() > 3 && m_model.GetAnimationFrame() < 15) {
		//�U���t���OON
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

//�Q�[�W������
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

	//�p���B����
	if (m_model.GetAnimationFrame() > 0 && m_model.GetAnimationFrame() < 10) {
		m_parry_flag = true;
	}
	else {
		m_parry_flag = false;
	}

	//�U������
	if (m_model.GetAnimationFrame() > 3 && m_model.GetAnimationFrame() < 15) {
		//�U���t���OON
		m_attack_flag = true;
	}
	else {
		m_attack_flag = false;
	}

	if (m_model.GetAnimationFrame() < 15 && m_key_dir.LengthSq() > 0.1) {
		//�v���C���[�������Ă�������ֈړ����Ȃ���U��
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

//����؂�ւ�
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

	//�}�E�X�̒��{�^���������Ώ�Ԃ��؂�ւ��
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

	//�����[�h
	if (m_bullet <= 23.0f) {
		if (PUSH(CInput::eButton13) || PUSH(CInput::eMouseL) && m_bullet <= 0.0f) {
			m_state = eState_Reload;
		}
		else {
			m_model.ChangeAnimation(1, 25, false);
		}
	}

	//���
	if (PUSH(CInput::eButton5)) {
		//�������O�ɃL�[���͂̕������x�N�g���t�Z����
		m_rot.y = atan2(m_dir.x, m_dir.z);
		m_state = eState_Roll;
	}

	//���b�N�I������G��ύX
	if (PUSH(CInput::eButton12)) {
		LockOnTargetE(10, eDirRight);
		m_lock_flag = true;
	}

	//�t���O��true�Ȃ烍�b�N�I������
	if (m_lock_flag == true) {
		LookToTarget();
		//E�L�[�ŉ���
		if (PUSH(CInput::eButton11)) {
			m_lock_flag = false;
		}
	}

	//�\����
	if (HOLD(CInput::eMouseR)) {
		m_model.ChangeAnimation(1, 18, false);
		m_stand_flag = true;
	}
	else {
		//�{�^���𗣂�����߂�
		m_model.ChangeAnimation(1, 25, false);
		m_stand_flag = false;
	}

	if (m_shot_cnt > 0.0f) {
		m_shot_cnt -= 1.0f;
	}

	//�e������
	if (HOLD(CInput::eMouseL) && HOLD(CInput::eMouseR) && m_shot_cnt == 0.0f && m_bullet > 0.0f) {

		switch (m_shot)
		{
			//�E
		case 0:
			sound.Play("shot");
			Base::Add(new Bullet(m_model.GetFrameMatrix(44) * CVector4D(0, 0, 0, 1), CMatrix::MRotation(m_rot).GetFront(), 1.0));
			m_shot++;
			break;
			//��
		case 1:
			sound.Play("shot");
			Base::Add(new Bullet(m_model.GetFrameMatrix(15) * CVector4D(0, 0, 0, 1), CMatrix::MRotation(m_rot).GetFront(), 1.0));
			m_shot--;
			break;
		}

		//�e�����Ԋu
		m_shot_cnt = 15.0f;
		m_bullet -= 1.0f;
	}
}

//�����[�h
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
		//�������O�ɃL�[���͂̕������x�N�g���t�Z����
		m_rot.y = atan2(m_dir.x, m_dir.z);
		m_state = eState_Roll;
	}
}

//�X�E�F�C���
void Player::StateSway()
{
	m_model.SetAnimationSpeed(1.8f);
	//�����
	if (key_length <= 0) {
		//�X�E�F�C�̑���
		float roll_speed = 0.07f;
		m_model.ChangeAnimation(17, false);

		if (m_model.GetAnimationFrame() < 26) {
			//�v���C���[�������Ă�������։��
			m_pos -= m_dir * roll_speed;
		}
		//�X�E�F�C���I�������
		if (m_model.isAnimationEnd()) {
			roll_speed = 0.0f;
			m_state = eState_Idle;
		}
	}
	//���
	else {
		//�X�E�F�C����̑���
		roll_speed = 0.12f;
		m_model.ChangeAnimation(15, false);

		if (m_model.GetAnimationFrame() < 30) {
			//�v���C���[�������Ă�������։��
			m_pos += m_dir * roll_speed;
		}

		//�X�E�F�C������I�������
		if (m_model.isAnimationEnd()) {
			roll_speed = 0.0f;
			m_state = eState_Idle;
		}
		//��𒆂ɂ�����x�{�^���������Γ]�������ɂȂ���
		else if (m_model.GetAnimationFrame() > 22 && m_key_dir.LengthSq() > 0.1) {
			if (HOLD(CInput::eButton5)) {
				m_dash_flag = false;
				m_state = eState_Roll;
			}
			else if (m_model.GetAnimationFrame() > 30) {
				m_dash_flag = false;
			}
			//�ړ��L�[�������Ă���΃_�b�V���ɂȂ���
			else {
				m_dash_flag = true;
				roll_speed = 0.0f;
				m_state = eState_Idle;
			}
		}
	}
}

//�U���L�����Z���X�E�F�C
void Player::StateSwayC()
{
	m_attack_flag = false;
	m_model.SetAnimationSpeed(1.8f);
	CMatrix rotMtx = CMatrix::MRotationY(m_rot.y);
	//�X�E�F�C�̑���
	float roll_speed = 0.06f;

	if (m_key_dir.LengthSq() > 0) {
		//�ړ����� ��]�s��~�ړ�����
		m_dir = rotMtx * m_key_dir;

		m_pos += m_dir * roll_speed;
		int anim = -1;
		float key_ang = atan2(m_key_dir.x, m_key_dir.z);
		//�O
		if (key_ang > DtoR(-30) && key_ang < DtoR(30)) {
			anim = 0;
		}
		//�E
		else if (key_ang <= DtoR(-30) && key_ang >= DtoR(-135)) {
			m_model.ChangeAnimation(28, false);
		}
		//��
		else if (key_ang >= DtoR(+30) && key_ang <= DtoR(+135)) {
			m_model.ChangeAnimation(27, false);
		}
		//���
		else{
			m_model.ChangeAnimation(17, false);
		}
	}

	//�X�E�F�C���I�������
	if (m_model.isAnimationEnd()) {
		m_model.SetAnimationSpeed(1.0f);
		roll_speed = 0.0f;
		m_state = eState_Idle;
	}
}

//���
void Player::StateRoll()
{
	m_model.SetAnimationSpeed(1.6f);
	//���[�����O�̑���
	roll_speed = 0.10f;
	m_model.ChangeAnimation(16, false);

	if (m_model.GetAnimationFrame() < 30) {
		m_pos += m_dir * roll_speed;
	}

	if (m_model.isAnimationEnd()) {
		m_model.SetAnimationSpeed(1.0f);
		roll_speed = 0.0f;

		//�e��ԂȂ�Aim�ɖ߂�
		if (m_state_flag == true) {
			m_state = eState_Aim;
		}
		//����ԂȂ�Idle�ɖ߂�
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

//������΂����
void Player::StateDamageF()
{
	m_model.ChangeAnimation(5, false);
	m_dash_flag = false;

	if (m_model.GetAnimationFrame() < 1) {
		LookToTarget();
		mp_camera->StartShake(0.5f, 1.0f);
	}

	if (m_model.GetAnimationFrame() < 30) {
		//������΂�������
		m_pos -= m_dir * m_burst_pow;
	}

	//�N���オ��ɏ������Ԃ�������
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
		//������΂�������
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
//�f�o�b�O�p
void Player::Draw()
{
	FONT_T()->Draw(0, 204, 1, 1, 1, "�W���X�g�^�C���p�����ԁF%.1f", m_just_cnt);
	FONT_T()->Draw(0, 224, 1, 1, 1, "X���W�F%f", m_pos.x);
	FONT_T()->Draw(0, 244, 1, 1, 1, "Y���W�F%f", m_pos.y);
	FONT_T()->Draw(0, 264, 1, 1, 1, "Z���W�F%f", m_pos.z);
	FONT_T()->Draw(0, 284, 1, 1, 1, "�ړ��X�s�[�h�F%f", move_speed);
	FONT_T()->Draw(0, 304, 1, 1, 1, "�c��e���F%.0f", m_bullet);
	FONT_T()->Draw(0, 324, 1, 1, 1, "�Q�[�W�����F%f", m_bullet / m_bullet_max);
	FONT_T()->Draw(0, 344, 1, 1, 1, "HP�����F%f", m_hp / m_hp_max);
	FONT_T()->Draw(0, 364, 1, 1, 1, "�e�����񕜂܂ŁF%f", m_auto_chage_cnt);
}
#endif

void Player::LockOnTargetE(float range)
{

	//�G�����X�g�Ŏ擾
	auto list = Base::FindObjects(eEnemy);
	//�^�[�Q�b�g
	Base* t = nullptr;
	//�����p�x���̍ŏ��l
	float min = 10;
	//�v���C���[�̕���
	CVector3D dir(sin(m_rot.y), 0, cos(m_rot.y));
	for (auto b : list) {
		//�v���C���[����G�ւ̃x�N�g��
		CVector3D v = b->m_pos - (m_pos - dir * 0.5f);
		//�v���C���[����G�ւ̋���
		float l = v.Length();
		//�v���C���[����G�ւ̕���
		CVector3D d = v / l;
		//�v���C���[�̐��ʕ����ƓG�ւ̕����̂Ȃ��p�x
		float a = acos(min(1.0f, max(-1.0f, CVector3D::Dot(d, dir))));
		//60�x�ȏ�͖���
		if (a > DtoR(60.0f)) continue;
		//�������߂��A���ʂɋ߂��G���c��
		l = l * (a / 5 + 1.0f);
		if (l > min) continue;
		min = l;
		t = b;
	}
	mp_target = t;
}

void Player::LockOnTargetB(float range)
{
	//�{�X�����X�g�Ŏ擾
	auto list = Base::FindObjects(eBoss);
	//�^�[�Q�b�g
	Base* t = nullptr;
	//�����p�x���̍ŏ��l
	float min = 10;
	//�v���C���[�̕���
	CVector3D dir(sin(m_rot.y), 0, cos(m_rot.y));
	for (auto b : list) {
		//�v���C���[����G�ւ̃x�N�g��
		CVector3D v = b->m_pos - (m_pos - dir * 0.5f);
		//�v���C���[����G�ւ̋���
		float l = v.Length();
		//�v���C���[����G�ւ̕���
		CVector3D d = v / l;
		//�v���C���[�̐��ʕ����ƓG�ւ̕����̂Ȃ��p�x
		float a = acos(min(1.0f, max(-1.0f, CVector3D::Dot(d, dir))));
		//60�x�ȏ�͖���
		if (a > DtoR(60.0f)) continue;
		//�������߂��A���ʂɋ߂��G���c��
		l = l * (a / 5 + 1.0f);
		if (l > min) continue;
		min = l;
		t = b;
	}
	mp_target = t;

}

void Player::LookToTarget()
{
	//�^�[�Q�b�g�����Ȃ��A�܂��͍폜���ꂽ
	if (mp_target && mp_target->m_kill)
		mp_target = nullptr;
	if (!mp_target) return;
	//�v���C���[����^�[�Q�b�g�ւ̃x�N�g��
	CVector3D v = mp_target->m_pos - m_pos;
	//�^�[�Q�b�g�֌�����]�l���t�Z
	float a = atan2(v.x, v.z);
	m_rot.y = a;
	//�����x�N�g���X�V
	m_dir = CVector3D(sin(m_rot.y), 0, cos(m_rot.y));

	//���b�N�I���G�t�F�N�g
	//Base::Add(new CEffect("CircleR", mp_target, CVector3D(0, 0, 0), CVector3D(0, 0, 0), 0.2f, 0.4f, -0.1f, CEffect::eBlendAlpha, 1, CVector3D(0, 0, 0), CVector3D(0, 0, 0), 10));
}

void Player::LockOnTargetE(float range, int lockdir)
{
	//�G�����X�g�Ŏ擾
	auto list = Base::FindObjects(eEnemy);
	//�^�[�Q�b�g
	Base* t = nullptr;
	//�����p�x�̍ŏ��l
	float min = FLT_MAX;
	//�����p�x���ł�����Ă���
	Base* rev_t = nullptr;
	//�����p�x�̍ő�l
	float rev = FLT_MAX;
	//�G���[�v
	for (auto b : list) {
		//�J��������G�ւ̃x�N�g��
		CVector3D v = b->m_pos - m_pos;
		//�J��������G�ւ̋���
		float l = v.Length();
		//�J��������G�ւ̕���
		CVector3D d = v / l;
		//�J��������G�ւ̕�������p�x���v�Z
		float a = atan2(d.x, d.z);
		//�J�����̊p�x�ƓG�̊p�x�̍���
		a = Utility::NormalizeAngle(a - m_rot.y);
		//���b�N�I��
		if (lockdir == eDirCenter) {
			//���E�֌W�Ȃ����S�ɋ߂��G��I��
			a = abs(a);
			//���ʂŋ����̋߂��G��I�Ԃ悤��
			l = l * (a / 5 + 0.01f);
		}
		else {
			//�^�[�Q�b�g���E�ւ̐؂�ւ�
			//���݂̂ă^�[�Q�b�g�Ƃ͎Q�Ƃ��Ȃ�
			if (b == mp_target) continue;
			//�G���؂�ւ����������ɋ���ƁAa�́{��
			//�����������a��-��
			a *= lockdir;
			l = a;
		}
		//���ʂɋ߂��G���c��
		if (l < min && a>0) {
			min = l;
			t = b;
		}
		else
			//�ł������G
			if (l < rev) {
				rev = l;
				rev_t = b;
			}
	}

	//�^�[�Q�b�g�m��
	if (t) {
		mp_target = t;
	}
	else {
		mp_target = rev_t;
	}

}

