#pragma once
#include "../Base/Base.h"


class SoundManager : public Base {
private:

public:
	SoundManager();

	void Play(const char* name);
	void Loop(const char* name);
	void Stop(const char* name);
};

