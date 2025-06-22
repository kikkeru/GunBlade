#include"Tutorial.h"
#include"Field.h"
#include"Player.h"
#include"Enemy.h"
#include"Door.h"
#include "FollowCamera.h"
#include"../Title/Title.h"

Tutorial::Tutorial() :Base(eType_Scene) ,
m_tutorial_text("C:\\\Windows\\Fonts\\msgothic.ttc", 32) {
	m_menu_img = COPY_RESOURCE("Dark", CImage);
	m_tutorial_img = COPY_RESOURCE("Tutorial", CImage);
	m_tutorial2_img = COPY_RESOURCE("Tutorial2", CImage);

	m_menu_select = 0;
	m_practice_flag = false;
	m_draw_flag = false;
}

Tutorial::~Tutorial() {
	Base::KillALL();
	Base::Add(new Title());
}

void Tutorial::Update() {

	Base* p = Base::FindObject(ePlayer);

	if (PUSH(CInput::eButton14)) {
		sound.Stop("TitleBGM");
		SetKill();
	}
}

void Tutorial::Draw()
{
	if (m_practice_flag == false) {
		m_draw_flag = false;
		Menu();
	}
	else {
		Practice();
	}
	m_tutorial_text.Draw(100, 1000, 1.0f, 1.0f, 1.0f, "Xキーで前に戻る     Kキーでタイトルに戻る");
}

void Tutorial::Menu()
{
	m_menu_img.Draw();
	CVector2D menu_pos(700, 400);


	if (PUSH(CInput::eUp) || PUSH(CInput::eUpKey)) {
		if (m_menu_select > 0) {
			sound.Play("cursor");
			m_menu_select--;
		}
	}

	if (PUSH(CInput::eDown) || PUSH(CInput::eDownKey)) {
		if (m_menu_select < 3) {
			sound.Play("cursor");
			m_menu_select++;
		}
	}

	//縦に並べて画像を表示
	for (int i = 0; i < 4; i++) {
		int x = 0;
		int y = i * 80;

		m_tutorial_img.SetRect(0, i * 80, 438, (i + 1) * 80);
		m_tutorial_img.SetSize(438, 80);
		m_tutorial_img.SetPos(menu_pos + CVector2D(x, y));
		m_tutorial_img.Draw();

		//選択中の画像は光らせる
		if (m_menu_select == i) {
			m_tutorial2_img.SetRect(0, i * 80, 438, (i + 1) * 80);
			m_tutorial2_img.SetSize(438, 80);
			m_tutorial2_img.SetPos(menu_pos + CVector2D(x, y));
			m_tutorial2_img.Draw();
		}
	}

	//操作方法へ
	if (PUSH(CInput::eButton1)) {
		sound.Play("next");
		m_practice_flag = true;
		Practice();
	}
}

void Tutorial::Practice()
{
	m_menu_img.Draw();
	//Xキーで前に戻る
	if (PUSH(CInput::eButton2)) {
		m_practice_flag = false;
	}

	switch (m_menu_select)
	{
		//移動の仕方について
	case 0:
		m_tutorial_text.Draw(100, 100, 1.0f, 1.0f, 1.0f, "・WASDキーで移動");
		m_tutorial_text.Draw(100, 200, 1.0f, 1.0f, 1.0f, "・shiftボタンでダッシュ");
		m_tutorial_text.Draw(100, 300, 1.0f, 1.0f, 1.0f, "shiftボタンを押し続けなくても移動キーを押している限り走りは継続される");
		m_tutorial_text.Draw(100, 400, 1.0f, 1.0f, 1.0f, "走るのをやめたいときはもう一度shiftボタンを押すか一度止まれば解除される");
		m_tutorial_text.Draw(100, 500, 1.0f, 1.0f, 1.0f, "・マウスを動かすことでカメラを操作できる");
		m_tutorial_text.Draw(100, 600, 1.0f, 1.0f, 1.0f, "・マウスの中ボタンを押すと武器を持ち替える");
		break;
		//剣の状態での攻撃方法(ゲージ攻撃も込み)
	case 1:
		m_tutorial_text.Draw(100, 100, 1.0f, 1.0f, 1.0f, "・マウス左ボタンを押すと攻撃(最大３回連続で攻撃できる)");
		m_tutorial_text.Draw(100, 200, 1.0f, 1.0f, 1.0f, "・マウス左ボタンを長押しすると突き刺し攻撃");
		m_tutorial_text.Draw(100, 300, 1.0f, 1.0f, 1.0f, "そのまま長押しすると爆破攻撃に派生");
		m_tutorial_text.Draw(100, 400, 1.0f, 1.0f, 1.0f, "・マウス右ボタンを押しながら左ボタンを押すとゲージを消費した強攻撃");
		m_tutorial_text.Draw(100, 500, 1.0f, 1.0f, 1.0f, "・マウス右ボタンを押しながらスペースキーを押すと移動しつつ強攻撃がうてる（威力は低くなる）");
		m_tutorial_text.Draw(100, 600, 1.0f, 1.0f, 1.0f, "強攻撃を敵の攻撃に合わせて当てるとパリィが発生");
		break;
		//銃の状態での攻撃方法
	case 2:
		m_tutorial_text.Draw(100, 100, 1.0f, 1.0f, 1.0f, "・マウス右ボタンを押し続けて構える");
		m_tutorial_text.Draw(100, 200, 1.0f, 1.0f, 1.0f, "構え中にマウス左ボタンを押すと弾を撃つ");
		m_tutorial_text.Draw(100, 300, 1.0f, 1.0f, 1.0f, "・Rキーでリロード");
		m_tutorial_text.Draw(100, 400, 1.0f, 1.0f, 1.0f, "リロードしなくても一定時間ごとに弾が一発ずつ自動回復する");
		m_tutorial_text.Draw(100, 500, 1.0f, 1.0f, 1.0f, "・Qキーで敵をロックオン　もう一度押すと別の敵をロックオン");
		m_tutorial_text.Draw(100, 600, 1.0f, 1.0f, 1.0f, "・Eキーでロックオン解除");
		break;
		//回避と攻撃キャンセル回避について
	case 3:
		m_tutorial_text.Draw(100, 100, 1.0f, 1.0f, 1.0f, "・スペースキーで回避");
		m_tutorial_text.Draw(100, 200, 1.0f, 1.0f, 1.0f, "回避は連続で二回までできる");
		m_tutorial_text.Draw(100, 300, 1.0f, 1.0f, 1.0f, "・一度目の回避時移動キーを押していればそのまま走ることができる");
		m_tutorial_text.Draw(100, 400, 1.0f, 1.0f, 1.0f, "・剣使用時には攻撃中にもスペースキーを押すと回避ができる");
		break;
	}
}