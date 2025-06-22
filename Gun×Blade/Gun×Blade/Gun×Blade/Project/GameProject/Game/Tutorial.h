#pragma once
#pragma once
#include"../Base/Base.h"
#include "SoundManager.h"

class Event;

class Tutorial :public Base {
private:
	CFont m_tutorial_text;
	SoundManager sound;
	CImage m_menu_img;

	CImage m_tutorial_img;
	CImage m_tutorial2_img;

	int m_menu_select;
public:
	//���H�Ɉڍs����t���O
	bool m_practice_flag;
	//�L�����o���t���O
	bool m_draw_flag;

	Tutorial();
	~Tutorial();
	void Update();
	void Draw();
	void Menu();
	void Practice();
};

