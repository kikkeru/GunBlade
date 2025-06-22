#include "GameOver.h"
#include "Title.h"
#include "Game/Boss.h"

GameOver::GameOver() :Base(eType_Scene),
m_text("C:\\\Windows\\Fonts\\msgothic.ttc", 50) {
	m_gameover = new CVideo("Movie/GameOver.mp4");
	m_gameover->Play(false);
}

GameOver::~GameOver() {
	//�S�ẴI�u�W�F�N�g��j��
	Base::KillALL();
	//�^�C�g����
	Base::Add(new Title());
}

void GameOver::Update() {
	//�c���Ă���{�X������
	if (Base::FindObject(eBoss) != nullptr) {
		Kill(1 << eBoss);
	}

	//Z�Ń^�C�g���j��
	if (PUSH(CInput::eButton1)) {
		SetKill();
	}
}

void GameOver::Draw() {
	m_gameover->Draw();
	m_gameover->SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	if (m_gameover->isEnd()) {
		m_text.Draw(650, 800, 1.0f, 1.0f, 1.0f, "Z�L�[�Ń^�C�g���ɖ߂�");
	}
}
