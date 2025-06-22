#pragma once
#include "../Base/Base.h"
#include "Player.h"
#include "SoundManager.h"
#include "EventCamera.h"

class EventCamera;

class Boss : public Base {
private:
	SoundManager sound;

	CVector3D m_key_dir;

	CVector3D m_rarmS;
	CVector3D m_rarmE;

	CVector3D m_larmS;
	CVector3D m_larmE;

	enum {
		eCap_Left_Arm,
		eCap_Right_Arm,
		eCap_Max
	};

	CMatrix m_arm_matrix;
	Base* mp_target;
	CVector3D m_rot_target;		//回転目標値

	EventCamera* mp_camera;

	float m_cnt;
	//ジャスト処理終了時間
	int m_reset_time;
	//待ち時間
	int m_wait_time;
	//攻撃力
	int m_attack_pow;
	//攻撃判定かどうか
	bool m_attack_flag;
	//吹き飛ばし判定かどうか
	bool m_burst_flag;
	//大技を既に打ったかどうか
	bool m_attackS_flag;
	bool m_sound_flag;
	//回避とパリィされた時のフラグ
	bool m_act_flag;

	void StateIdle();
	void StateStep();
	void StateAttackP();
	void StateAttackK();
	void StateAttackKTrick();
	void StateAttackJump();
	void StateAttackSRush();
	void StateAttackRush();
	void StateAttackS1();
	void StateAttackS2();
	void StateAttackS3();
	void StateBurst();
	void StateDamage();
	void StateBreak();
	void StateDeath();
	void StateRoar();

public:
	//イベントシーンが終わったかどうか
	bool m_event_flag;

	CModelA3M m_model;

	//攻撃された回数
	int m_hit;
	int m_state;
	//ジャンプ攻撃や回避の移動速度
	float jmove_speed;
	//自身からプレイヤーへのベクトル
	CVector3D v;

	enum {
		eState_Idle,
		eState_Step,
		eState_AttackP,
		eState_AttackK,
		eState_AttackKTrick,
		eState_AttackJump,
		eState_AttackSRush,
		eState_AttackRush,
		eState_AttackS1,
		eState_AttackS2,
		eState_AttackS3,
		eState_AttackS4,
		eState_Burst,
		eState_Break,
		eState_Damage,
		eState_Death,
		eState_Roar,
	};

	//カプセルデータ構造体
	struct Cap {
		CVector3D s;
		CVector3D e;
		float r;
	};
	struct Index {
		int s;
		int e;
		float r;
	};

	//部位ごとのカプセル
	static Index m_culsule_idx[eCap_Max];
	Cap m_cupsule[eCap_Max];
	Cap m_arm;

	//バースト用の半径
	float m_b_rad;
	//フレーム取得用
	float m_frame;
	//突進継続時間
	float m_rush_cnt;
	//ロックオン時間
	float m_look_cnt;
	//後隙
	int m_wait_cnt;
	//突進クールタイム
	float m_rush_recharge;
	//とびかかりクールタイム
	float m_jump_recharge;
	//回避行動のクールタイム
	float m_roll_recharge;
	//カウンターのクールタイム
	float m_trick_recharge;
	//大技を出すときの位置
	int m_set_point;
	//大技のステップ処理
	int m_attck_step;

	float move_speed;
	float m_hp;
	//爆破攻撃時の仰け反り値
	float m_bom_pow;

	CVector3D m_burst;

	Boss(const CVector3D& pos);

	void Update();
	void Render();

	void Damage(Base* attaker, int damage, bool breakflag, bool leanback);
	void LookToTarget();
	//補間
	void LerpTarget();
	void Collision(Base* b);
	void Recharge();
	//回避とパリィの処理
	void Avoid(Player* p,Boss* b);
	void Parry(Player* p,Boss* b);
#if _DEBUG
	void Draw();
#endif
};
