#include "Wall.h"
#include "Boss.h"

Wall::Wall(CVector3D& pos, CVector3D& rot)
	:Base(eWall)
{
	m_model = COPY_RESOURCE("Pillar", CModelA3M);
	m_model.ChangeAnimation(0, false,false);
	m_pos = pos;
	m_rot = rot;
	m_obb = COBB(m_pos, m_rot, CVector3D(1.25, 3, 1.25));
	m_break_flag = false;
}

Wall::~Wall()
{
}

void Wall::Update()
{
	if (m_break_flag == true) {
		m_model.UpdateAnimation();

		if (m_model.GetAnimationFrame() == 1) {
			sound.Play("collapse");
		}

		if (m_model.isAnimationEnd()) {
			SetKill();
		}
	}
}

void Wall::Render()
{
	m_model.SetPos(m_pos);
	m_model.SetRot(m_rot);
	m_model.SetScale(0.01, 0.01, 0.01);
	m_model.Render();
	//��`����m�F�p
	//Utility::DrawOBB(m_obb, CVector4D(1.0, 0.0, 0.0, 0.2));
}

void Wall::Collision(Base* b)
{
	switch (b->GetType())
	{
	case ePlayer:
	{
		CVector3D axis;
		float dist;
		if (CCollision::CollisionOBBCapsule(m_obb, b->m_lineS, b->m_lineE, b->m_rad, &axis, &dist)) {
			//�߂荞�ݗ�
			float s = b->m_rad - dist;
			//�v���C���[�݂̂߂荞�񂾕������
			b->m_pos += axis * s / 2;
		}
	}
	break;
	case eBoss:
	{
		CVector3D axis;
		float dist;
		if (Boss* e = dynamic_cast<Boss*>(b)) {
			//���ɓ���������
			if (CCollision::CollisionOBBCapsule(m_obb, b->m_lineS, b->m_lineE, b->m_rad, &axis, &dist)) {
				//�߂荞�ݗ�
				float s = e->m_rad - dist;
				//�G�݂̂߂荞�񂾕������
				b->m_pos += axis * s / 2;

				//��Z���ɓ�����΋��܂Ȃ�
				if (e->m_state == e->eState_AttackS3) {
					if (m_break_flag == false) {
						e->m_hp -= 50;
						m_break_flag = true;
					}
					//HP��0�ȉ��ɂȂ��
					if (e->m_hp <= 0) {
						e->m_state = e->eState_Death;
					}
					break;
				}

				//�ːi���̂ݔ���
				if (e->m_state == e->eState_AttackRush) {
					m_break_flag = true;
					e->m_state = e->eState_Break;
					e->m_hp -= 200;
				}

				if (e->m_hp < 0) e->m_hp = 0;
				//HP��0�ȉ��ɂȂ��
				if (e->m_hp <= 0) {
					e->m_state = e->eState_Death;
				}
			}
		}
	}
	break;
	}
}
