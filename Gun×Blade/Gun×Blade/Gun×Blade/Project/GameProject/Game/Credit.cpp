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
	//�S�ẴI�u�W�F�N�g��j��
	Base::KillALL();
	//�^�C�g����
	Base::Add(new Title());
}

void Credit::Update() {
	//X�L�[�Ń^�C�g���j��
	if (PUSH(CInput::eButton2)) {
		SetKill();
	}
}

void Credit::Draw() {
	m_credit_img.Draw();

	m_credit_text2.Draw(100, 100, 1.0f, 1.0f, 1.0f, "�g�p�����y��");
	m_credit_text.Draw(100, 200, 1.0f, 1.0f, 1.0f, "�t���[BGM�E���y�f��MusMus �l");
	m_credit_text.Draw(100, 300, 1.0f, 1.0f, 1.0f, "�t���[BGM�EDOVA-SYNDROME �l");

	m_credit_text2.Draw(100, 600, 1.0f, 1.0f, 1.0f, "�g�p�������ʉ�");
	m_credit_text.Draw(100, 700, 1.0f, 1.0f, 1.0f, "���ʉ����{ �l");

	m_text.Draw(100, 1000, 1.0f, 1.0f, 1.0f, "X�L�[�Ń^�C�g���ɖ߂�");
}
