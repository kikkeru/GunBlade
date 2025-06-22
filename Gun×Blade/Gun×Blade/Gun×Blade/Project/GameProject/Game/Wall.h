#pragma once
#include "../Base/Base.h"
#include "SoundManager.h"

class Wall : public Base {
private:
	SoundManager sound;

public:
	CModelA3M m_model;

	bool m_break_flag;

	Wall(CVector3D& pos, CVector3D& rot);
	~Wall();

	void Update();
	void Render();
	void Collision(Base* b);

};
