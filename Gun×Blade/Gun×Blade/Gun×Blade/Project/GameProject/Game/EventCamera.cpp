#include "EventCamera.h"
#include "CameraShake.h"
#include"Boss.h"
#include "../Effekseer/EffekseerEffect.h"

EventCamera::EventCamera()
	: Base(eEventCamera)
{
	m_move_speed = 0.1f;
	m_event_flag = true;
	m_event_scene = 0;
	m_shake_intensity = 0.0f;
	m_shake_duration = 0.0f;
	m_shake_time = 0.0f;
}

EventCamera::~EventCamera()
{
	if (Boss* b = dynamic_cast<Boss*>(Base::FindObject(eBoss))) {
		b->m_event_flag = true;
	}
	Base::Kill(1 << eEventCamera);
}

void EventCamera::Update()
{

	shake.Update();
	elapsedTime += CFPS::GetDeltaTime();

	// �L�[�t���[���Ԃ̕��
	for (size_t i = 0; i + 1 < keyframes.size(); ++i) {
		if (elapsedTime >= keyframes[i].time && elapsedTime < keyframes[i + 1].time) {
			float t = (elapsedTime - keyframes[i].time) / (keyframes[i + 1].time - keyframes[i].time);
			CVector3D pos = keyframes[i].pos * (1.0f - t) + keyframes[i + 1].pos * t;
			CVector3D rot = keyframes[i].rot * (1.0f - t) + keyframes[i + 1].rot * t;

			// �J�����̉�]��ݒ�
			CCamera::GetCurrent()->SetTranseRot(pos + shake.GetShakeOffset(), rot);
			break;
		}
	}
	
	// �C�x���g�V�[���̏I���
	if (keyframes.size() > 0 && elapsedTime >= keyframes.back().time) {
		keyframes.clear();
		m_event_scene++;
		elapsedTime = 0.0f;
	}

}

void EventCamera::Render()
{
	//true�ɂȂ�ƃC�x���g�V�[���������
	if (m_event_flag) {
		switch (m_event_scene)
		{
			//��l�̂ۂ����������n��
		case 0:
			AddKeyframe(0.0f, CVector3D(-1.0f, 1.5f, -1.0f), CVector3D(0.0f, DtoR(180), 0.0f));
			AddKeyframe(1.5f, CVector3D(-1.0f, 1.5f, -1.0f), CVector3D(0.0f, DtoR(190), 0.0f));
			AddKeyframe(3.0f, CVector3D(-1.0f, 1.5f, -1.0f), CVector3D(0.0f, DtoR(170), 0.0f));
			AddKeyframe(4.5f, CVector3D(-1.0f, 1.5f, -1.0f), CVector3D(0.0f, DtoR(180), 0.0f));
			elapsedTime = 0.0f;
			m_event_scene++;
			break;
		case 1:
			//m_event_scene++�҂�
			break;
			//�ォ��{�X���o��
		case 2:
			Base::Add(new Boss(CVector3D()));
			m_event_scene++;
			break;
			//�J�������{�X�ɋ߂Â��{�X�̙��K
		case 3:
			shake.StartShake(0.2f, 0.5f);
			//�{�X�̙��K
			if (Boss* b = dynamic_cast<Boss*>(Base::FindObject(eBoss))) {
				b->m_state = b->eState_Roar;
			}

			//���K��̗]�C
			AddKeyframe(0.0f, CVector3D(-1.0f, 1.5f, -6.0f), CVector3D(0.0f, DtoR(180), 0.0f));
			AddKeyframe(5.0f, CVector3D(-1.0f, 1.5f, -6.0f), CVector3D(0.0f, DtoR(180), 0.0f));
			elapsedTime = 0.0f;
			m_event_scene++;
			break;
		case 4:
			//m_event_scene++�҂�
			break;
			//�C�x���g�V�[���I���Q�[���֖߂�
		case 5:
			m_event_flag = false;
			SetKill();
			break;
		}
	}

}

void EventCamera::StartShake(float intensity, float duration)
{
	shake.StartShake(intensity, duration);
}

