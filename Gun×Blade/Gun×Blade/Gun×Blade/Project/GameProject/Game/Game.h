#pragma once
#include"../Base/Base.h"
#include "SoundManager.h"

class Event;

class Game :public Base {
private:
	SoundManager sound;
public:
	int m_stage_step;

	bool m_stage_flag1;
	bool m_stage_flag2;
	bool m_stage_flag3;
	//ƒ{ƒX•”‰®‚È‚çfalse‚»‚¤‚¶‚á‚È‚¯‚ê‚Îtrue
	bool m_boss_flag;

	Event* m_event;

	Game();
	~Game();
	void Update();
	static void Load();
};

