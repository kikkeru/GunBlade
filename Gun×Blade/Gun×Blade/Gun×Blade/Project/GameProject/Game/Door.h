#pragma once
#include "../Base/Base.h"

class Door : public Base {
private:

public:
	bool door_flag;

	Door(CVector3D& pos, CVector3D& rot);

	void Update();
	void Render();
	void Collision(Base* b);

};
