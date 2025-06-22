#pragma once
#include"../Base/Base.h"

class GameOver :public Base {
	CVideo* m_gameover;
	CFont m_text;
public:
	GameOver();
	~GameOver();
	void Update();
	void Draw();
};
