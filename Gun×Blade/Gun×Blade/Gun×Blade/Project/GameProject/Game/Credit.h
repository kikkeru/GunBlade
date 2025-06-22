#pragma once
#include"../Base/Base.h"
#include "SoundManager.h"

class Credit :public Base {
	//画像オブジェクト
	CImage m_credit_img;
	//文字表示オブジェクト
	CFont m_credit_text;
	CFont m_credit_text2;
	CFont m_text;

	SoundManager sound;
public:
	Credit();
	~Credit();
	void Update();
	void Draw();
};
