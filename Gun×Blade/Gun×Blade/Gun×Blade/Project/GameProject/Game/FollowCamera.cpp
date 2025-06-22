#include "FollowCamera.h"
#include "Field.h"
#include "Player.h"

FollowCamera::FollowCamera()
	: Base(eCamera), m_dist(4.0f) {
	//��]�l
	m_rot = CVector3D(DtoR(30), DtoR(-125), 0);
}

void FollowCamera::Update()
{
	//�J�����V�F�C�N�̏���
	shake.Update();

	m_dist = 5.0f;
	//��]���x
	float cam_speed = 0.0015;
	//�}�E�X�̈ړ���
	CVector2D mouse_vec = CInput::GetMouseVec();
	//�}�E�X�ړ��ŃJ������]
	m_rot += CVector3D(mouse_vec.y, -mouse_vec.x, 0) * cam_speed;
	m_rot.x = min(DtoR(60), max(DtoR(-35), m_rot.x));
	//-180�`180�ɐ��K��
	m_rot.y = Utility::NormalizeAngle(m_rot.y);
}

void FollowCamera::Render()
{
	//�C�x���g�J��������������v���C���[�̃J������`�悵�Ȃ�
	if (Base::FindObject(eEventCamera)) return;

	//�v���C���[�̎擾
	if (Player* b = dynamic_cast<Player*>( Base::FindObject(ePlayer)))
	{

		//�e�̏�Ԃ̎�
		if (b->m_state_flag) {
			if (b->m_lock_flag == true && b->mp_target != nullptr) {
				CVector3D dir = (b->mp_target->m_pos - b->m_pos).GetNormalize();
				m_rot = CVector3D(asin(-dir.y), atan2(dir.x, dir.z), 0);
			}

			//�L�����N�^�[�̉E�������ɃJ������ݒu����
			CMatrix cam_matrix = CMatrix::MTranselate(b->m_pos)								//�L�����N�^�[�̍s��
				* CMatrix::MTranselate(CVector3D(0, 1.07, 0)) * CMatrix::MRotation(m_rot)   //(���ւ̕��s�ړ�*��]�j
				* CMatrix::MTranselate(CVector3D(-0.5, 0.4, -2));							//(������E�����֕��s�ړ�)

			//�ڐG�_�Ɩ@��
			CVector3D cross, normal;
			CVector3D s = cam_matrix * CVector4D(0, 0, 0, 1);
			CVector3D e = cam_matrix * CVector4D(0, 0, 4, 1);
			//�t�B�[���h�Ɛ����i�����_�����_�j�Ƃ̏Փ˔���
			if (Base* f = Base::FindObject(eField)) {
				if (f->GetModel()->CollisionRay(&cross, &normal, e, s)) {
					cam_matrix.m03 = cross.x;
					cam_matrix.m13 = cross.y;
					cam_matrix.m23 = cross.z;
				}
			}
			//�J�����̃��[���h�s��ŃJ������ݒ�
			CCamera::GetCurrent()->SetTranseRot(cam_matrix * CMatrix::MTranselate(shake.GetShakeOffset()));
		}
		//���̏�Ԃ̎�
		else {
			//�ʒu�ƒ����_��ݒ�
			m_at = b->m_pos + CVector3D(0, 1.5, 0);
			m_pos = m_at + CMatrix::MRotation(m_rot).GetFront() * -m_dist;
			//�J�����ƃt�B�[���h�̐ڐG����
		    //�ڐG�_�Ɩ@��
			CVector3D cross, normal;
			//�t�B�[���h�Ɛ����i�����_�����_�j�Ƃ̏Փ˔���
			if (Base* f = Base::FindObject(eField)) {
				if (f->GetModel()->CollisionRay(&cross, &normal, m_at, m_pos)) {
					m_pos = cross;
				}
			}
			CCamera::GetCurrent()->LookAt(m_pos + shake.GetShakeOffset(), m_at + shake.GetShakeOffset(), CVector3D(0, 1, 0));
		}
	}

}

void FollowCamera::StartShake(float intensity, float duration)
{
	shake.StartShake(intensity, duration);
}
