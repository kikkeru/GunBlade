#pragma once
#include "../Base/Base.h"

class Bullet : public Base {
private:
	float m_hit_distance;
	Base* m_hit_object;
	CVector3D m_hit_point;
public:
	//’e‚ªˆê’èŠÔ“–‚½‚ç‚È‚¯‚ê‚ÎÁ‚¦‚é
	float m_vanish_cnt;
	int m_attack_pow;

	Bullet(const CVector3D& pos, const CVector3D dir, const float speed);
	~Bullet();

	void Update();
	void Render();
	void Collision(Base* b);
};
