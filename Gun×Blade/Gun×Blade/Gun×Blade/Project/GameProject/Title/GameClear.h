#pragma once
#include"../Base/Base.h"
#include "Game/SoundManager.h"

class GameClear :public Base {
private:
	CImage m_clear_img;
	CFont m_text;
	SoundManager sound;
public:
	GameClear();
	~GameClear();
	void Update();
	void Draw();
};

