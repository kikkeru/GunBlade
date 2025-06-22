#include"Title.h"
#include"Game/Game.h"
#include"Game/Credit.h"
#include"Game/Tutorial.h"

Title::Title() :Base(eType_Scene),
m_title_text("C:\\\Windows\\Fonts\\msgothic.ttc", 32){
	m_font_img = COPY_RESOURCE("TitleFont", CImage);
	m_menu_img = COPY_RESOURCE("Menu", CImage);
	m_menu2_img = COPY_RESOURCE("Menu2", CImage);
	font_alpha = 0.0f;
	//ビデオ読み込み
	m_title = new CVideo("Movie/GameTitle.mp4");
	m_loop = new CVideo("Movie/Loop.mp4");
	m_lord = new CVideo("Movie/NowLording.mp4");

	m_title->Play(false);
	m_loop_flag = false;
	m_push_font = true;
	m_lord_flag = false;
	m_menu_select = 0;
	m_menu_now = 0;
	m_menu_cnt = 1.0f;
}

Title::~Title() {
	m_lord->Stop();
	delete m_loop;
	delete m_title;
	delete m_lord;

	//全てのオブジェクトを破棄
	Base::KillALL();

	//ゲームスタート
	if (m_menu_select == 0) {
		Base::Add(new Game());
	}
	//チュートリアル
	else if (m_menu_select == 1) {
		Base::Add(new Tutorial());
	}
	else {
		Base::Add(new Credit());
	}
}


void Title::Update() {

	//タイトルビデオが終わったらループ映像に切り替え
	if (m_title && m_title->isEnd() && m_loop_flag == false) {
		//タイトルビデオを削除
		m_title->Stop();
		delete m_title;
		m_title = nullptr;

		sound.Loop("TitleBGM");
		//ループするタイトルに変更する
		m_loop->Play(true);

		m_loop_flag = true;
	}

	//タイトル中Zキーでスキップ
	if (m_title && m_title->isPlay()) {
		if (PUSH(CInput::eButton1)) {
			m_title->Stop();
			delete m_title;
			m_title = nullptr;

			sound.Loop("TitleBGM");
			m_loop->Play(true);
			m_loop_flag = true;
		}
	}
	//ループタイトルならメニュー処理
	else {
		if (m_loop_flag == true) {

			if (PUSH(CInput::eButton1) && m_menu_now == 0) {
				m_push_font = !m_push_font;
				m_menu_now = 1;
				m_menu_cnt = 1.0f;
			}
		}
	}

	if (m_lord_flag) {
		//ロード中かどうか
		if (CLoadThread::GetInstance()->CheckEnd()) {
			//完了
			SetKill();
		}
	}
}

void Title::Draw() {
	if (m_title) {
		m_title->SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		m_title->Draw();
	}
	else if(m_lord_flag==false){
		m_loop->SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		m_loop->Draw();
		if (m_push_font) {
			Font();
		}
		else {
			Menu();
		}
	}
	else {
		m_lord->SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		m_lord->Draw();
	}
		
}

void Title::Font()
{
	CVector2D font_pos(600, 700);

	//θ加算
	font_alpha += 0.05f;
	if (font_alpha > DtoR(180)) {
		font_alpha = 0.0f;
	}

	//通常の文字の描画
	m_font_img.SetSize(678, 52);
	m_font_img.SetPos(font_pos);
	m_font_img.SetRect(0, 0, 678, 52);
	m_font_img.SetColor(1, 1, 1, 1);
	m_font_img.Draw();

	//光る文字を重ねて描画
	m_font_img.SetRect(0, 70, 678, 125);
	//sinカーブの特性を使い、0→1→0へα値が変化
	float alpha = (sin(font_alpha) + 1) / 2;
	m_font_img.SetColor(1, 1, 1, sin(font_alpha));
	m_font_img.Draw();

}

//メニュー画面
void Title::Menu()
{
	CVector2D menu_pos(700, 700);
	m_title_text.Draw(100, 1000, 1.0f, 1.0f, 1.0f, "Zキーで決定　↑↓キーまたはWSキーで移動");

	if (PUSH(CInput::eUp) || PUSH(CInput::eUpKey)) {
		if (m_menu_select > 0) {
			sound.Play("cursor");
			m_menu_select--;
		}
	}

	if (PUSH(CInput::eDown) || PUSH(CInput::eDownKey)) {
		if (m_menu_select < 2) {
			sound.Play("cursor");
			m_menu_select++;
		}
	}

	//縦に並べて画像を表示
	for (int i = 0; i < 3; i++) {
		int x = 0;
		int y = i * 64;

		m_menu_img.SetRect(0, i * 64, 360, (i + 1) * 64);
		m_menu_img.SetSize(357, 64);
		m_menu_img.SetPos(menu_pos + CVector2D(x, y));
		m_menu_img.Draw();

		//選択中の画像は光らせる
		if (m_menu_select == i) {
			m_menu2_img.SetRect(0, i * 64, 360, (i + 1) * 64);
			m_menu2_img.SetSize(357, 64);
			m_menu2_img.SetPos(menu_pos + CVector2D(x, y));
			m_menu2_img.Draw();
		}
	}

	//重複判定で一気に進まないようにする
	if (m_menu_cnt > 0) {
		m_menu_cnt--;

	}
	//選択処理
	else if (PUSH(CInput::eButton1)) {
		if (m_menu_select == 0) {
			sound.Play("select");
			m_lord_flag = true;
			sound.Stop("TitleBGM");
			//ロード開始
			CLoadThread::GetInstance()->LoadStart(Load);
			m_loop->Stop();
			//ロード中ムービー
			m_lord->Play(true);
		}
		else {
			//ゲームスタート以外ロードはなくていい
			SetKill();
		}
	}
}

void Title::Load()
{
	//キャラクター
	ADD_RESOURCE("Player", CModel::CreateModel("Charactor/swat/swat.a3m"));
	ADD_RESOURCE("Enemy", CModel::CreateModel("Charactor/assassin/assassin.a3m"));
	ADD_RESOURCE("Mutant", CModel::CreateModel("Charactor/mutant/mutant.a3m"));
	//フィールド
	ADD_RESOURCE("Field", CModel::CreateModel("Field/TestStage/TestStage.obj", 16, 1, 16));
	ADD_RESOURCE("Field2", CModel::CreateModel("Field/Dungeon/Dungeon.obj", 16, 1, 16));

	//特殊なオブジェクト
	ADD_RESOURCE("Pillar", CModel::CreateModel("Field/TestStage/Pillar.a3m", 16, 1, 16));

	//武器
	ADD_RESOURCE("Sword", CModel::CreateModel("Wepon/Sword/Sword.obj", 16, 1, 16));
	ADD_RESOURCE("Gun", CModel::CreateModel("Wepon/Gun/Gun.obj", 16, 1, 16));

	//UI
	ADD_RESOURCE("Gauge", CImage::CreateImage("UI/gauge.png"));
	ADD_RESOURCE("Capacity", CImage::CreateImage("UI/Bullets.png"));

	//エフェクト
	ADD_RESOURCE("CircleW", CModel::CreateModel("Effect/CircleW.obj"));
	ADD_RESOURCE("CircleR", CModel::CreateModel("Effect/CircleR.obj"));
	ADD_RESOURCE("CircleM", CModel::CreateModel("Effect/CircleM1.obj"));

}
