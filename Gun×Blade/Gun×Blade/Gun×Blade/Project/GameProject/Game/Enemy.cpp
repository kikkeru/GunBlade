#include "Enemy.h"
#include "Player.h"
#include "Bullet.h"
#include "../Effekseer/EffekseerEffect.h"

Enemy::Enemy(const CVector3D& pos)
	:Base(eEnemy)
{
	m_model = COPY_RESOURCE("Enemy", CModelA3M);
	//���a
	m_rad = 0.9f;
	m_hp = 250;
	//�q�b�g�X�g�b�v������
	m_hit_stop = 0;
	//�����ʒu���v���C���[�Əd�Ȃ�Ȃ��悤�ɂ���
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
	{16,19,0.4f},		//���r
	{45,48,0.4f},		//�E�r
	{86,89,0.4f},		//�E��
};

void Enemy::Update()
{
	
	//��Ƀv���C���[�̈ʒu���񂾂�����
	if (m_death_flag == false) {
		LookNeckTarget();
	}

	//�����蔻��J�v�Z���̐ݒ�
	m_lineS = m_pos + CVector3D(0, 2.5 - m_rad, 0);
	m_lineE = m_pos + CVector3D(0, m_rad, 0);

	//�q�b�g�X�g�b�v
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

	//�J�v�Z���̐ݒ�
	for (int i = 0; i < eCap_Max; i++) {
		//�J�v�Z����ݒ�
		m_cupsule[i].s = m_model.GetFrameMatrix(m_culsule_idx[i].s).GetPosition();
		m_cupsule[i].e = m_model.GetFrameMatrix(m_culsule_idx[i].e).GetPosition();
		m_cupsule[i].r = m_culsule_idx[i].r * 1.0;
	}

	//�d�͗���
	m_vec.y -= GRAVITY;
	m_pos.y += m_vec.y;
}

void Enemy::Render()
{
	m_model.SetPos(m_pos);
	m_model.SetRot(m_rot);
	m_model.SetScale(0.010, 0.010, 0.010);
	m_model.Render();

	//�L�����̃J�v�Z���̕\��
	//Utility::DrawCapsule(m_lineS, m_lineE, m_rad, CVector4D(1, 0, 0, 0.5));

	//���ʂ��ƂɃJ�v�Z����ݒ肷��(��`�\���p)
	for (int i = 0; i < eCap_Max; i++) {
		//Utility::DrawCapsule(m_cupsule[i].s, m_cupsule[i].e, m_cupsule[i].r, CVector4D(1, 0, 0, 0.5));
	}
}

void Enemy::Collision(Base* b)
{
	switch (b->GetType()) {
	case eField:
		//���f���Ƃ̔���
	{
		//�����߂���
		CVector3D v(0, 0, 0);
		//�J�v�Z���ƃ��f���̏Փ�
		auto tri = b->GetModel()->CollisionCupsel(m_pos + CVector3D(0, 2.5 - m_rad, 0),	//�n�_�i���j
			m_pos + CVector3D(0, m_rad, 0),	//�I�_�i�����j
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
				if (m_vec.y < 0)
					m_vec.y = 0;
			}
			else if (t.m_normal.x > 0.5f) {
				m_bom_pow = 0;
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
	//�G���G���m�ŏd�Ȃ�Ȃ��悤�ɂ���
	case eEnemy:
	{
		//�������g�Ƃ͔��肵�Ȃ�
		if (this != b) {
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
		}
	}
	break;
	case ePlayer:
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
			m_act_flag = true;
			CVector3D c1;
			CVector3D pos;

			if (Player* p = dynamic_cast<Player*>(b)) {

				//�v���C���[�̖��G����
				if (p->m_state == Player::eState_Damage || p->m_state == Player::eState_DamageF || p->m_state == Player::eState_Roll || p->m_parry_flag == true) {
					break;
				}
				//�E�r�̍U��
				if (m_state == eState_Attack1 || m_state == eState_Attack2) {
					//���i�q�b�g����ƉE�r�ƃJ�v�Z���̔���
					if (m_attack_no != b->m_hit_no && CCollision::CollisionCapsule(m_cupsule[eCap_Right_Arm].s, m_cupsule[eCap_Right_Arm].e, m_cupsule[eCap_Right_Arm].r, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)) {
						pos = c1;
						//���i�q�b�g���
						b->m_hit_no = m_attack_no;

						//�q�b�g�X�g�b�v
						m_hit_stop = 0.10f;

						Avoid(p, this);

						if (m_act_flag) {
							//�_���[�W����
							p->Damage(this, m_attack_pow,0.0f);
							sound.Play("enemyattck");
							Base::Add(new EffekseerEffect("Blow11", pos, CVector3D(0, 0, 0), CVector3D(0.5, 0.5, 0.5)));
						}
					}
				}
				//�E���̍U��
				if (m_state == eState_Attack_Kick) {
					//���i�q�b�g����ƍ��r�ƃJ�v�Z���̔���
					if (m_attack_no != b->m_hit_no && CCollision::CollisionCapsule(m_cupsule[eCap_Right_Leg].s, m_cupsule[eCap_Left_Arm].e, m_cupsule[eCap_Left_Arm].r, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)) {
						pos = c1;
						//���i�q�b�g���
						b->m_hit_no = m_attack_no;

						//�q�b�g�X�g�b�v
						m_hit_stop = 0.20f;

						Avoid(p, this);

						if (m_act_flag) {
							sound.Play("enemyattck");
							Base::Add(new EffekseerEffect("Blow11", pos, CVector3D(0, 0, 0), CVector3D(0.5, 0.5, 0.5)));
							p->Damage(this, m_attack_pow,0.0f);
						}
					}
				}
				//�Ƃт�����
				if (m_state == eState_Attack_Jump) {
					//���i�q�b�g����ƍ��r�ƃJ�v�Z���̔���
					if (m_attack_no != b->m_hit_no && CCollision::CollisionCapsule(m_cupsule[eCap_Right_Leg].s, m_cupsule[eCap_Left_Arm].e, m_cupsule[eCap_Left_Arm].r, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)) {
						pos = c1;
						//���i�q�b�g���
						b->m_hit_no = m_attack_no;

						//�q�b�g�X�g�b�v
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
		//�����W���X�g����ł�����
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

	//���̓G���U�������Ă�����擾����
	m_npc_act_flag = false;
	auto List = Base::FindObjects(eEnemy);
	for (auto& b : List) {
		if (Enemy* e = dynamic_cast<Enemy*>(b)) {
			if (e->m_action_flag) {
				m_npc_act_flag = true;
			}
		}
	}

	//���X�g����v���C���[��T��
	if (mp_target) {
		//�^�[�Q�b�g�ւ̃x�N�g��
		vec = mp_target->m_pos - m_pos;

		if (m_target_position_offset.Length() == 0 || m_npc_act_flag == false) {
			// �G�L�����N�^�[���v���C���[�̎�����͂ނ悤�ɃI�t�Z�b�g��ݒ� 
			float angle = atan2(-vec.x, -vec.z) + DtoR(Utility::Rand(-60.0f, 60.0f));
			float radius = 2.5f + (rand() % 100) / 100.0f;
			// ������ݒ� 
			m_target_position_offset = CVector3D(cos(angle) * radius, 0, sin(angle) * radius);
		}

		// �^�[�Q�b�g�ʒu���v�Z
		CVector3D target_pos = mp_target->m_pos + m_target_position_offset;

		// �U���̂��߂Ƀv���C���[�ɋ߂Â�
		if (!m_npc_act_flag) {
			// �v���C���[�ɋ߂Â����߂̌v�Z 
			target_pos = mp_target->m_pos;
		}

		// �^�[�Q�b�g�ʒu�ւ̃x�N�g��
		vec = target_pos - m_pos;

		//�x�N�g�������]�l���t�Z
		m_rot_target.y = atan2(vec.x, vec.z);
		//��]�l��������x�N�g�����v�Z
		m_dir = CVector3D(CVector3D(sin(m_rot.y), 0, cos(m_rot.y)));

		//�^�[�Q�b�g�ւ̋�����1.2f��艓�����
		if (vec.Length() > 1.2f) {
			//�O�i
			m_pos += m_dir * move_speed * CFPS::GetTimeScale();
			//�O�i�A�j���[�V����
			m_model.ChangeAnimation(1);
		}
		else {
			m_model.ChangeAnimation(0);
			LookToTarget();
		}

		//���̓G���U�����Ă��Ȃ����
		if (vec.Length() < 1.2f && m_npc_act_flag == false) {
			m_action_flag = true;

			if (rand() % 1000 < 150) {
				//�U��ID�ύX
				m_attack_no++;
				m_state = eState_Attack1;
			}
			else if (rand() % 1000 < 30) {
				//�U��ID�ύX
				m_attack_no++;
				m_state = eState_Attack_Kick;
			}
		}

		//�v���C���[�Ƃ̋���������Ă��邩���`���[�W��0�̎�
		//�Ƃт�����
		if (rand() % 1000 < 50 && m_jump_recharge <= 0 && vec.Length() > 1.8f && m_npc_act_flag == false) {
			m_action_flag = true;
			m_attack_no++;
			m_state = eState_Attack_Jump;
		}

		float a = Utility::NormalizeAngle(m_rot_target.y - m_rot.y);
		//�����ɂ����
		m_rot.y += a * 0.1f;
	}
}

//3�A��
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

	//����̃t���[���ԍU������
	if (m_model.GetAnimationFrame() > 26 && m_model.GetAnimationFrame() < 32) {
		//�U���t���OON
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

	//����̃t���[���ԍU������
	if (m_model.GetAnimationFrame() > 14 && m_model.GetAnimationFrame() < 21) {
		//�U���t���OON
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

	//����̃t���[���ԍU������
	if (m_model.GetAnimationFrame() > 17 && m_model.GetAnimationFrame() < 22) {
		//�U���t���OON
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

//�Ƃт�����
void Enemy::StateAttack_Jump()
{
	m_model.ChangeAnimation(6, false);
	m_attack_pow = 15;

	//�Ƃт�����O�̏���
	if (m_model.GetAnimationFrame() > 1 && m_model.GetAnimationFrame() < 10) {
		m_pos += m_dir * 0.05 * CFPS::GetTimeScale();
		m_model.SetAnimationSpeed(0.5f);
	}
	else {
		m_model.SetAnimationSpeed(1.0f);
	}

	if (m_model.GetAnimationFrame() < 11) {
		//�v���C���[�̕��Ɍ���
		LookToTarget();
	}

	//��яオ������
	if (m_model.GetAnimationFrame() > 11 && m_model.GetAnimationFrame() < 28) {
		jmove_speed = 0.05f;
		//���g����v���C���[�ւ̃x�N�g��
		v = mp_target->m_pos - m_pos;
		//�v���C���[�ɂƂт�����
		m_pos += v* jmove_speed * CFPS::GetTimeScale();
	}

	//����̃t���[���ԍU������
	if (m_model.GetAnimationFrame() > 31 && m_model.GetAnimationFrame() < 36) {
		//�U���t���OON
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
//������΂����
void Enemy::StateDamageF()
{
	m_model.SetAnimationSpeed(1.2f);
	m_model.ChangeAnimation(9, false);

	//�v���C���[���U�����Ă��������֌���
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

//�����オ��
void Enemy::StateStundUp()
{
	m_model.ChangeAnimation(10, false);
	m_model.SetAnimationSpeed(1.5f);

	if (m_model.isAnimationEnd()) {
		m_model.SetAnimationSpeed(1.0f);
		m_state = eState_Idle;
	}
}

//������΂���ē|�����
void Enemy::StateDeathF()
{
	m_model.SetAnimationSpeed(1.2f);
	m_model.ChangeAnimation(9, false);
	m_death_flag = true;

	//�v���C���[���U�����Ă��������֌���
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

//�����オ��O�ɓ|�����
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

//(�U���ҁA�_���[�W�ʁA������ԗ́A������΂���邩�ǂ���)
void Enemy::Damage(Base* attaker, int damage, float pow, bool breakflag)
{
	m_action_flag = false;
	m_hp -= damage;
	m_bom_pow = pow;
	m_model.BindFrameMatrix(8, CMatrix::MRotation(CVector3D(0, 0, 0)));

	if (m_hp < 0) m_hp = 0;
	//HP��0�ȉ��ɂȂ��
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
	//�^�[�Q�b�g�����Ȃ��A�܂��͍폜���ꂽ
	if (mp_target && mp_target->m_kill)
		mp_target = nullptr;

	if (!mp_target) return;

	//�G����v���C���[�ւ̃x�N�g��
	CVector3D v = mp_target->m_pos - m_pos;
	//�^�[�Q�b�g�֌�����]�l���t�Z
	float a = atan2(v.x, v.z);
	m_rot.y = a;
	//�����x�N�g���X�V
	m_dir = CVector3D(sin(m_rot.y), 0, cos(m_rot.y));
}

//��̕����̂݃v���C���[�֌�����
void Enemy::LookNeckTarget()
{
	if (mp_target && mp_target->m_kill) {
		mp_target = nullptr;
	}

	if (!mp_target) return;

	// �G����v���C���[�ւ̃x�N�g��
	CVector3D v = mp_target->m_pos - m_pos;

	// �^�[�Q�b�g�֌�����]�l���t�Z 
	yaw = atan2(v.x, v.z);

	//��̌������擾
	CVector3D parent_dir = m_model.GetFrameMatrix(7).GetFront();
	float parent_rot = atan2(parent_dir.x, parent_dir.z);
	float a = Utility::NormalizeAngle(yaw - parent_rot);

	// ��]�l�̏�����}60�x�ɐ���
	maxYaw = DtoR(60.0f);
	minYaw = DtoR(-60.0f);

	if (a < minYaw) {
		a = minYaw;
	}
	else if (a > maxYaw) {
		a = maxYaw;
	}

	yaw = Utility::NormalizeAngle(parent_rot + a);

	//�v���C���[�̕����֎�𓮂���
	m_model.BindFrameMatrix(8, CMatrix::MRotation(CVector3D(0, yaw, 0)));
}

#if _DEBUG
void Enemy::Draw()
{
	FONT_T()->Draw(1700, 64, 1, 1, 1, "�GHP�F%.0f", m_hp);
	FONT_T()->Draw(1700, 84, 1, 1, 1, "%s", m_action_flag ? "true" : "false");
	FONT_T()->Draw(1700, 104, 1, 1, 1, "�W�����v�F%.0f", m_jump_recharge);
	FONT_T()->Draw(1700, 124, 1, 1, 1, "X���W�F%.0f", m_pos.x);
	FONT_T()->Draw(1700, 144, 1, 1, 1, "Y���W�F%.0f", m_pos.y);
	FONT_T()->Draw(1700, 164, 1, 1, 1, "Z���W�F%.0f", m_pos.z);
	FONT_T()->Draw(1700, 184, 1, 1, 1, "��̉�]�l�F%.0f", yaw);

}
#endif
