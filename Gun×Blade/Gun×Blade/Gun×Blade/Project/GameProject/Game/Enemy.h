#pragma once
#include "../Base/Base.h"
#include"Player.h"
#include "SoundManager.h"

class Enemy : public Base {

	SoundManager sound;

	CVector3D m_key_dir;
	//敵の散らばり
	CVector3D m_target_position_offset;

	CVector3D m_rarmS;
	CVector3D m_rarmE;

	CVector3D m_larmS;
	CVector3D m_larmE;

	enum {
		eCap_Left_Arm,
		eCap_Right_Arm,
		eCap_Right_Leg,
		eCap_Max
	};

	CMatrix m_arm_matrix;
	Base* mp_target;
	CVector3D m_rot_target;		//回転目標値

	float m_cnt;
	//待ち時間
	int m_wait_time;
	//攻撃力
	int m_attack_pow;

	void StateIdle();
	void StateAttack1();
	void StateAttack2();
	void StateAttack_Kick();
	void StateAttack_Jump();
	void StateBreak();
	void StateDamage();
	void StateDamageF();
	void StateStundUp();
	//立ち上がり途中に倒される
	void StateDeathF();
	void StateDeathS();
	void StateDeath();

public:
	CModelA3M m_model;

	int m_state;
	//ジャンプ攻撃や回避の移動速度
	float jmove_speed;
	//自身からプレイヤーへのベクトル
	CVector3D v;
	CVector3D vec;

	enum {
		eState_Idle,
		eState_Attack1,
		eState_Attack2,
		eState_Attack_Kick,
		eState_Attack_Jump,
		eState_Break,
		eState_Damage,
		eState_DamageF,
		eState_StundUp,
		eState_DeathF,
		eState_DeathS,
		eState_Death,
	};

	//敵の各部位との判定
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

	//とびかかりクールタイム
	float m_jump_recharge;

	float move_speed;
	float m_hp;
	//ジャスト回避されたときに遅くなる秒数
	float m_slow_time;
	//爆破攻撃時の仰け反り値
	float m_bom_pow;

	//首の回転値
	float yaw;
	// 回転値の上限
	float maxYaw;
	// 回転値の下限
	float minYaw;

	//攻撃判定かどうか
	bool m_attack_flag;
	//自分が攻撃中かどうか
	bool m_action_flag;
	//自分以外が攻撃中かどうか
	bool m_npc_act_flag;
	////回避とパリィされた時のフラグ
	bool m_act_flag;
	//倒されたかどうか
	bool m_death_flag;

	CVector3D m_burst;

	Enemy(const CVector3D& pos);

	void Update();
	void Render();

	void Damage(Base* attaker, int damage,float pow,bool breakflag);
	void LookToTarget();
	void LookNeckTarget();
	void Collision(Base* b);
	void Avoid(Player* p, Enemy* b);
#if _DEBUG
	void Draw();
#endif
};
