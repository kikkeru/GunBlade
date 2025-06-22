#include "Credit.h"
#include "Title/Title.h"

Credit::Credit() :Base(eType_Scene),
m_credit_text("C:\\\Windows\\Fonts\\msgothic.ttc", 64),
m_credit_text2("C:\\\Windows\\Fonts\\msgothic.ttc", 48),
m_text("C:\\\Windows\\Fonts\\msgothic.ttc", 32) {
	m_credit_img = COPY_RESOURCE("Dark", CImage);
}

Credit::~Credit() {
	sound.Stop("TitleBGM");
	//全てのオブジェクトを破棄
	Base::KillALL();
	//タイトルへ
	Base::Add(new Title());
}

void Credit::Update() {
	//Xキーでタイトル破棄
	if (PUSH(CInput::eButton2)) {
		SetKill();
	}
}

void Credit::Draw() {
	m_credit_img.Draw();

	m_credit_text2.Draw(100, 100, 1.0f, 1.0f, 1.0f, "使用した楽曲");
	m_credit_text.Draw(100, 200, 1.0f, 1.0f, 1.0f, "フリーBGM・音楽素材MusMus 様");
	m_credit_text.Draw(100, 300, 1.0f, 1.0f, 1.0f, "フリーBGM・DOVA-SYNDROME 様");

	m_credit_text2.Draw(100, 600, 1.0f, 1.0f, 1.0f, "使用した効果音");
	m_credit_text.Draw(100, 700, 1.0f, 1.0f, 1.0f, "効果音ラボ 様");

	m_text.Draw(100, 1000, 1.0f, 1.0f, 1.0f, "Xキーでタイトルに戻る");
}
