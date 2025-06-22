#pragma once
#include "../Base/Base.h"
#include "SoundManager.h"
#include "CameraShake.h"

class EventCamera : public Base {
private:
	//カメラスピード
	float m_move_speed;
	//イベントの切り替わりタイミング
	int m_event_scene;

	CCamera m_event_camera;
	SoundManager sound;
	CameraShake shake;

public:

	bool m_event_flag;

	struct Keyframe {
		float time;       // 時間
		CVector3D pos;    // カメラの位置
		CVector3D rot;    // カメラの回転
	};

	std::vector<Keyframe> keyframes;
	//経過時間
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
