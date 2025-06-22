#include "GameClear.h"
#include "Title.h"

GameClear::GameClear() :Base(eType_Scene),
m_text("C:\\\Windows\\Fonts\\msgothic.ttc", 50)
{
	m_clear_img = COPY_RESOURCE("GameClear", CImage);
	sound.Play("gameclear");
}

GameClear::~GameClear() {
	//�S�ẴI�u�W�F�N�g��j��
	Base::KillALL();
	//�^�C�g����
	Base::Add(new Title());
}

void GameClear::Update() {
	//Z�Ń^�C�g���j��
	if (PUSH(CInput::eButton1)) {
		SetKill();
	}
}

void GameClear::Draw() {
	m_clear_img.Draw();
	m_text.Draw(650, 800, 1.0f, 1.0f, 1.0f, "Z�L�[�Ń^�C�g���ɖ߂�");
}
