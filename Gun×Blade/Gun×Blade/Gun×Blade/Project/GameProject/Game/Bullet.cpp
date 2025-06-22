#include "Bullet.h"
#include "Boss.h"
#include "Player.h"
#include "Enemy.h"
#include "Field.h"
#include "Effect.h"
#include "../Effekseer/EffekseerEffect.h"

Bullet::Bullet(const CVector3D& pos, const CVector3D dir, const float speed)
	:Base(eBullet)
{

	m_dir = dir;
	m_vec = dir * speed;
	m_pos = pos;

	m_vanish_cnt = 10;
}

Bullet::~Bullet()
{

}

void Bullet::Update()
{
	m_lineS = m_pos;
	m_pos += m_vec;
	m_lineE = m_pos;
}

void Bullet::Render()
{
	Utility::DrawLine(m_lineS, m_lineE, CVector4D(0, 1, 0, 1), 2.0f);
}

void Bullet::Collision(Base* b)
{
	CVector3D c1;
	CVector3D pos;
	float dist;

	//一定時間何にも当たらなければ消える
	if (m_vanish_cnt > 0) {
		m_vanish_cnt -= CFPS::GetDeltaTime();
	}
	else {
		SetKill();
		m_vanish_cnt = 10;
	}

	switch (b->GetType()) {
	case eField:
	{
		if (CCollision::CollisionCapsule(m_lineS, m_lineE, 0, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)) {
			SetKill();
		}
	}
	break;
	case eBoss:
	{
		m_attack_pow = 5;

		if (CCollision::CollisionCapsule(m_lineS, m_lineE, 0, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)) {
			if (Boss* bo = dynamic_cast<Boss*>(b)) {

				if (bo->m_state == bo->eState_Death) break;

				if (bo->m_state == bo->eState_AttackJump) {
					if (bo->m_model.GetAnimationFrame() > 17 && bo->m_model.GetAnimationFrame() < 45) {
						break;
					}
				}

				//仰け反らない
				bo->m_hp -= m_attack_pow;
				if (bo->m_hp < 0) bo->m_hp = 0;
				//HPが0以下になれば
				if (bo->m_hp <= 0) {
					bo->m_state = Boss::eState_Death;
				}
				//エフェクトの発生位置
				pos = c1;
				Base::Add(new EffekseerEffect("BulletImpact", pos, CVector3D(0, 0, 0), CVector3D(1, 1, 1)));
				SetKill();
				break;
			}
		}
	}
	break;
	case eEnemy:
	{
		m_attack_pow = 15;

		if (CCollision::CollisionCapsule(m_lineS, m_lineE, 0, b->m_lineS, b->m_lineE, b->m_rad, &dist, &c1)) {
			if (Enemy* e = dynamic_cast<Enemy*>(b)) {

				if (e->m_death_flag) break;

				//確率で仰け反る
				if (rand() % 1000 < 160) {
					e->Damage(this, m_attack_pow, 0.0f, false);
					break;
				}

				//仰け反らない
				e->m_hp -= m_attack_pow;
				if (e->m_hp < 0) e->m_hp = 0;
				//HPが0以下になれば
				if (e->m_hp <= 0) {
					e->m_state = Enemy::eState_Death;
				}
				//エフェクトの発生位置
				pos = c1;
				Base::Add(new EffekseerEffect("BulletImpact", pos, CVector3D(0, 0, 0), CVector3D(1, 1, 1)));
				SetKill();
				break;
			}
		}
	}
	break;
	}

}