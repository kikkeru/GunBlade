#pragma once
#include"../Base/Base.h"
#include "SoundManager.h"

class Credit :public Base {
	//�摜�I�u�W�F�N�g
	CImage m_credit_img;
	//�����\���I�u�W�F�N�g
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
