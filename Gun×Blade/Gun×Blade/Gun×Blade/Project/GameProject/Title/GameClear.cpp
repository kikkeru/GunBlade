#include "GameClear.h"
#include "Title.h"

GameClear::GameClear() :Base(eType_Scene),
m_text("C:\\\Windows\\Fonts\\msgothic.ttc", 50)
{
	m_clear_img = COPY_RESOURCE("GameClear", CImage);
	sound.Play("gameclear");
}

GameClear::~GameClear() {
	//全てのオブジェクトを破棄
	Base::KillALL();
	//タイトルへ
	Base::Add(new Title());
}

void GameClear::Update() {
	//Zでタイトル破棄
	if (PUSH(CInput::eButton1)) {
		SetKill();
	}
}

void GameClear::Draw() {
	m_clear_img.Draw();
	m_text.Draw(650, 800, 1.0f, 1.0f, 1.0f, "Zキーでタイトルに戻る");
}
