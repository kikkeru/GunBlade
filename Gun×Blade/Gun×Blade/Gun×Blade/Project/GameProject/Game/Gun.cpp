#include "Gun.h"
#include "Player.h"

Gun::Gun() :Base(eGun), mp_parent_obj(nullptr), m_shot(false)
{
	m_model = COPY_RESOURCE("Gun", CModelObj);

	m_rad = 0.02f;

}
void Gun::Update() {

	m_lineS = m_world_matrix * CVector4D(0, 0, 0.5f, 1);
	m_lineE = m_world_matrix * CVector4D(0, 0, -0.5f, 1);

	if (mp_parent_obj) {
		if (mp_parent_obj->m_kill) SetKill();
		return;
	}
	const float gravity = GRAVITY;
	m_vec.y -= gravity * CFPS::GetDeltaTime();
	m_pos += m_vec * CFPS::GetDeltaTime();


}
void Gun::Render()
{
	m_model.Render(m_world_matrix * CMatrix::MScale(2, 2, 2));
}

void Gun::SetDir(const CVector3D& dir)
{
	m_vec = dir;
}

void Gun::Shot(float sp, int pow)
{
	m_pos = m_world_matrix.GetPosition();
	m_vec = m_world_matrix.GetFront() * sp;
	m_shot = true;
	mp_parent_obj = nullptr;
	m_pow = pow;
}
