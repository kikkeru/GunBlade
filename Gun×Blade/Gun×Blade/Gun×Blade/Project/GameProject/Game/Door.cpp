#include "Door.h"
#include "Player.h"
#include "Effect.h"
#include "../Effekseer/EffekseerEffect.h"

Door::Door(CVector3D& pos, CVector3D& rot)
	:Base(eDoor)
{
	m_pos = pos;
	m_rot = rot;
	m_obb = COBB(m_pos, m_rot, CVector3D(1, 1, 1));
	door_flag = false;
}

void Door::Update()
{
}

void Door::Render()
{

	Base::Add(new EffekseerEffect("MagicShield", m_pos, CVector3D(DtoR(90), DtoR(90), DtoR(90)), CVector3D(1, 1, 1), 0, 33, false));
	//Utility::DrawOBB(m_obb, CVector4D(1.0, 0.0, 0.0, 1.0));
}

void Door::Collision(Base* b)
{
	switch (b->GetType())
	{
	case ePlayer:
		CVector3D axis;
		float dist;
		if (CCollision::CollisionOBBCapsule(m_obb, b->m_lineS, b->m_lineE, b->m_rad, &axis, &dist)) {
			if (Player* p = dynamic_cast<Player*>(b)) {

				door_flag = true;

			}
		}
		break;
	}
}
