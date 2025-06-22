#include"Game.h"
#include"Field.h"
#include"Player.h"
#include"Boss.h"
#include"Enemy.h"
#include"Door.h"
#include"Event.h"
#include"EventCamera.h"
#include "FollowCamera.h"
#include"../Title/Title.h"
#include"../Title/GameClear.h"
#include"../Title/GameOver.h"

Game::Game() :Base(eType_Scene) {
	m_stage_step = 5;
	m_event = 0;
	m_stage_flag1 = false;
	m_stage_flag2 = false;
	m_stage_flag3 = false;
	m_boss_flag = true;

	Base::Add(new FollowCamera());
	Base::Add(new Player(CVector3D()));
	Base::Add(new Field(0));
	Base::Add(new Door(CVector3D(-14, 0, -51), CVector3D(0, 0, 0)));

}

Game::~Game() {
	sound.Stop("BossBGM");
	sound.Stop("FieldBGM1");
}

void Game::Update() {

	Base* p = Base::FindObject(ePlayer);
	Base* d = Base::FindObject(eDoor);
	Base* b = Base::FindObject(eBoss);

	//プレイヤー死亡でゲームシーン終了
	if (!p && !m_kill) {
		m_stage_flag1 = true;
		m_stage_flag2 = true;
		m_stage_flag3 = true;
		SetKill();
		//ゲームオーバーへ
		Base::Add(new GameOver());
	}
	//ボスが倒されたら
	else if (!Base::FindObject(eBoss) && m_boss_flag == false && !m_kill) {
		SetKill();
		Base::Add(new GameClear());
	}

	if (m_stage_flag1 == false) {
		//左大部屋
		//大部屋の対角線の最小値と最大値を取って部屋全体に判定を取る
		if (CCollision::CollisionAABBPoint(CVector3D(-27, -1, 15), CVector3D(-11, 1, 31), p->m_pos)) {
			//部屋に閉じ込める壁の生成と部屋の中に出てくる敵の生成
			Base::Add(m_event = new Event(CVector3D(-9, 0, 23.5), CVector3D(0, DtoR(90), 0),
				{ CVector3D(-13,0,17),CVector3D(-24,0,24),CVector3D(-21,0,29) }));
			Base::Add(m_event = new Event(CVector3D(-20, 0, 14), CVector3D(0, 0, 0), {}));

			m_stage_flag1 = true;
		}
	}

	if (m_stage_flag2 == false) {
		//上大部屋
		//大部屋の対角線の最小値と最大値を取って部屋全体に判定を取る
		if (CCollision::CollisionAABBPoint(CVector3D(-39, -1, -21), CVector3D(-29, 1, 1.7), p->m_pos)) {
			//部屋に閉じ込める壁の生成と部屋の中に出てくる敵の生成
			Base::Add(m_event = new Event(CVector3D(-27, 0, -0.7), CVector3D(0, DtoR(90), 0),
				{ CVector3D(-35,0,-7),CVector3D(-35,0,-14),CVector3D(-34,0,-0.6),CVector3D(-29,0,-9) }));
			Base::Add(m_event = new Event(CVector3D(-27, 0, -18.5), CVector3D(0, DtoR(90), 0), {}));

			m_stage_flag2 = true;
		}
	}
	
	
	if (m_stage_flag3 == false) {
		//右大部屋
			//大部屋の対角線の最小値と最大値を取って部屋全体に判定を取る
		if (CCollision::CollisionAABBPoint(CVector3D(-21, -1, -41), CVector3D(-5, 1, -28), p->m_pos)) {
			//部屋に閉じ込める壁の生成と部屋の中に出てくる敵の生成
			Base::Add(m_event = new Event(CVector3D(-20, 0, -27), CVector3D(0, 0, 0),
				{ CVector3D(-14,0,-36),CVector3D(-7,0,-36),CVector3D(-12,0,-29),CVector3D(-14,0,-42),CVector3D(-7,0,-35) }));
			Base::Add(m_event = new Event(CVector3D(-14, 0, -46), CVector3D(0, 0, 0), {}));

			m_stage_flag3 = true;
		}
	}

	if (m_event->End()) {
		Base::Kill(1 << eEvent);
	}

	//ドアの処理
	if (Door* dd = dynamic_cast<Door*>(d)) {
		if (dd->door_flag) {
			Kill(1 << eField | 1 << eDoor | 1 << eEvent);
			sound.Stop("FieldBGM1");
			Base::Add(new Field(1));
			Base::Add(new EventCamera);
			//プレイヤーを出現させたい位置に置く
			p->m_pos = CVector3D(0.0f, 0.0f, 2.5f);
		}
	}

	if (Base::FindObject(eBoss)) {
		m_boss_flag = false;
	}

}
