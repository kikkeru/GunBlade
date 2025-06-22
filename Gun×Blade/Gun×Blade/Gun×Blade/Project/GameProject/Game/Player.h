#pragma once
#include "../Base/Base.h"
#include "SoundManager.h"

class Gauge;
class BulletCapacity;
class Gun;
class FollowCamera;

class Player : public Base {
private:
	
	CModelObj m_wepon_sword;

	CModelObj m_wepon_gun;
	//サウンド処理
	SoundManager sound;

	float m_wepon_rad;
	float m_wepon_rad2;
	float m_wepon_alpha;
	CMatrix m_wepon_matrix;
	CMatrix m_wepon_matrix2;
	
	CModelObj* m_my_wepon;
	
	int m_attack_pow;
	bool m_attack_flag;
	bool m_ground;

	//吹き飛ばされる威力
	float m_burst_pow;

	CVector3D m_key_dir;

	//ゲージ
	Gauge* m_gauge;
	BulletCapacity* m_capacity_gauge;
	FollowCamera* mp_camera;

	void StateIdle();
	void KeyUpdate();
	void StateAttack1();
	void StateAttack2();
	void StateAttack3();
	void StateAttackD();
	void StateAttackB1();
	void StateAttackB2();
	void StateAttackG();
	void StateAim();
	void StateReload();
	void StateSway();
	void StateSwayC();
	void StateRoll();
	void StateRollG();
	void StateDamage();
	void StateDamageF();
	void StateDeath();
	void StateDeathF();
	//雑魚敵のロックオン
	void LockOnTargetE(float range);
	//ボスのロックオン
	void LockOnTargetB(float range);
	void LookToTarget();
	void KeyMove();
	void AimMove();

public:
	CModelA3M m_model;

	enum {
		eState_Idle,
		eState_Attack1,
		eState_Attack2,
		eState_Attack3,
		eState_AttackD,
		eState_AttackB1,
		eState_AttackB2,
		eState_AttackG,
		eState_Aim,
		eState_ShotR,
		eState_ShotL,
		eState_Reload,
		eState_Jump,
		eState_Parry,
		eState_Sway,
		eState_SwayC,
		eState_Roll,
		eState_RollG,
		eState_JustRoll,
		eState_Damage,
		eState_DamageF,
		eState_Death,
		eState_DeathF,
	};

	enum {
		eDirRight = -1,
		eDirCenter = 0,
		eDirLeft = 1,

	};

	Base* mp_target;
	Gun* mp_gun;

	CMatrix m_shield_matrix;

	//構えているかどうか
	bool m_stand_flag;
	//ダッシュ判定かどうか
	bool m_dash_flag;
	//パリィ判定かどうかのフラグ
	bool m_parry_flag;
	//突き刺し攻撃が当たったかどうか
	bool m_hit_flag;
	//体のボーンを分けて動かすかどうか
	bool m_state_flag;
	//ジャストタイムの処理用
	bool m_just_flag;
	//手に持っているのが剣か銃かの判定
	bool m_change_flag;
	//ロックオンするかどうか
	bool m_lock_flag;
	//どっちの剣が当たっているか
	bool m_hit_sword1;
	bool m_hit_sword2;
	bool m_reload_flag;

	int m_state;
	//銃を構えているかどうか
	int m_aim_state;

	//ジャストタイムが終わるまでのカウント
	float m_just_cnt;
	//弾の自動回復カウント
	float m_auto_chage_cnt;
	//エフェクトの生成タイミング
	int m_foot_effect_index;

	//移動時の足音切り替え用
	int m_sound;

	CVector3D c1;
	float dist;

	float m_hp;
	float m_hp_max;
	//銃の発射間隔
	float m_shot_cnt;
	//装弾数
	float m_bullet;
	//最大装弾数
	float m_bullet_max;
	//どっちの銃で撃つかの切り替え用
	int m_shot;
	float m_reload;
	//ボタンを離すまでの秒数
	float m_out_cnt;

	float key_length;

	float move_speed;
	float roll_speed;
	//爆破攻撃時のプレイヤーの仰け反り値
	float m_recoil;

	float cam_ang;
	
	Player(const CVector3D& pos);
	void Update();
	void Render();

	void Damage(Base* attaker, int damage, float burstpow);
	void Collision(Base* b);
	/// <summary>
	/// アニメーションを分けるかどうか
	/// 0の時は全身同じ動き
	/// 1の時は上半身と下半身で別のアニメーションをさせられる
	/// </summary>
	void LockOnTargetE(float range, int lockdir);
	void ChangeAnimationLayer(int layer);
#if _DEBUG
	void Draw();
#endif

};
