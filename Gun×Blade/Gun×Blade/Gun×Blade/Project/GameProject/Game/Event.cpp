#include "Event.h"
#include "Player.h"
#include "Enemy.h"
#include "Effect.h"

Event::Event(CVector3D& pos ,CVector3D &rot,std::vector<CVector3D> list)
	:Base(eEvent)
{
	m_pos = pos;
	m_rot = rot;
	m_obb = COBB(m_pos, m_rot, CVector3D(5, 5, 0.1));

	//“G‚ÌƒŠƒXƒg
	for (auto& v : list) {
		Base::Add(new Enemy(v));
	}
}

Event::~Event()
{
}

//“G‚ª‚¢‚È‚­‚È‚é‚Æ‹N‚±‚é
bool Event::End()
{
	Base* b = Base::FindObject(eEnemy);

	return (b == nullptr);
}

void Event::Update()
{
	
}

void Event::Render()
{
	Base::Add(new CEffect("CircleM", CVector3D(m_pos.x, 2.5, m_pos.z), CVector3D(0, 0, 0), CVector3D(0, 0, 0), 1.5f, 1.5f, -0.3f, CEffect::eBlendAlpha, 0, m_rot, CVector3D(0, 0, 0), 10));
}

void Event::Collision(Base* b)
{
	switch (b->GetType())
	{
	case ePlayer:
		CVector3D axis;
		float dist;
		if (CCollision::CollisionOBBCapsule(m_obb, b->m_lineS, b->m_lineE, b->m_rad, &axis, &dist)) {
			if (Player* p = dynamic_cast<Player*>(b)) {

				//‰Ÿ‚µ•Ô‚·
				float s = p->m_rad - dist;
				p->m_pos += axis * s;
				
			}
		}
		break;
	}
}
