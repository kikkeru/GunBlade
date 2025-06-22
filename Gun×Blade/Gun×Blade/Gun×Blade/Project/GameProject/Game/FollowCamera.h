#pragma once
#include "../Base/Base.h"
#include "CameraShake.h"

class FollowCamera : public Base {
private:
	CameraShake shake;

	float m_dist;
public:
	FollowCamera();
	void Update();
	void Render();
	void StartShake(float intensity, float duration);
};