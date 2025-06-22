#pragma once
#include "../Base/Base.h"
#include "SoundManager.h"
#include "CameraShake.h"

class EventCamera : public Base {
private:
	//�J�����X�s�[�h
	float m_move_speed;
	//�C�x���g�̐؂�ւ��^�C�~���O
	int m_event_scene;

	CCamera m_event_camera;
	SoundManager sound;
	CameraShake shake;

public:

	bool m_event_flag;

	struct Keyframe {
		float time;       // ����
		CVector3D pos;    // �J�����̈ʒu
		CVector3D rot;    // �J�����̉�]
	};

	std::vector<Keyframe> keyframes;
	//�o�ߎ���
	float elapsedTime = 0.0f;

	float m_shake_intensity; 
	float m_shake_duration; 
	float m_shake_time; 

	EventCamera();
	~EventCamera();
	void Update();
	void Render();
	void AddKeyframe(float time, const CVector3D& pos, const CVector3D& rot) {
		keyframes.push_back({ time, pos, rot });
	}
	void StartShake(float intensity, float duration);
};
