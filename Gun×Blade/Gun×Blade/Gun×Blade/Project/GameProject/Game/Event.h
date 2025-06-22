#pragma once
#include "../Base/Base.h"

class Event : public Base {
private:

public:
	Event(CVector3D &pos,CVector3D &rot, std::vector<CVector3D> list);
	~Event();

	bool End();

	void Update();
	void Render();
	void Collision(Base* b);

};
