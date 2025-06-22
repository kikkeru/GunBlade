#pragma once
#include"../Base/Base.h"
#include "Game/SoundManager.h"

class Title :public Base {
private:
	//•¶Žš
	CFont m_title_text;

	CImage m_font_img;
	CImage m_menu_img;
	CImage m_menu2_img;

	CVideo* m_title;
	CVideo* m_loop;
	CVideo* m_lord;

	SoundManager sound;

	float font_alpha;
	//“ü—Í‚ðŽó‚¯•t‚¯‚È‚¢ŽžŠÔ
	float m_menu_cnt;

	int m_menu_select;
	int m_menu_now;
public:

	bool m_loop_flag;
	bool m_push_font;
	bool m_lord_flag;

	Title();
	~Title();
	void Update();
	void Draw();
	void Font();
	void Menu();
	static void Load();
};
