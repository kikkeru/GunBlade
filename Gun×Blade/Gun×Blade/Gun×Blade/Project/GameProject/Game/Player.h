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
	//�T�E���h����
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

	//������΂����З�
	float m_burst_pow;

	CVector3D m_key_dir;

	//�Q�[�W
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
	//�G���G�̃��b�N�I��
	void LockOnTargetE(float range);
	//�{�X�̃��b�N�I��
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

	//�\���Ă��邩�ǂ���
	bool m_stand_flag;
	//�_�b�V�����肩�ǂ���
	bool m_dash_flag;
	//�p���B���肩�ǂ����̃t���O
	bool m_parry_flag;
	//�˂��h���U���������������ǂ���
	bool m_hit_flag;
	//�̂̃{�[���𕪂��ē��������ǂ���
	bool m_state_flag;
	//�W���X�g�^�C���̏����p
	bool m_just_flag;
	//��Ɏ����Ă���̂������e���̔���
	bool m_change_flag;
	//���b�N�I�����邩�ǂ���
	bool m_lock_flag;
	//�ǂ����̌����������Ă��邩
	bool m_hit_sword1;
	bool m_hit_sword2;
	bool m_reload_flag;

	int m_state;
	//�e���\���Ă��邩�ǂ���
	int m_aim_state;

	//�W���X�g�^�C�����I���܂ł̃J�E���g
	float m_just_cnt;
	//�e�̎����񕜃J�E���g
	float m_auto_chage_cnt;
	//�G�t�F�N�g�̐����^�C�~���O
	int m_foot_effect_index;

	//�ړ����̑����؂�ւ��p
	int m_sound;

	CVector3D c1;
	float dist;

	float m_hp;
	float m_hp_max;
	//�e�̔��ˊԊu
	float m_shot_cnt;
	//���e��
	float m_bullet;
	//�ő呕�e��
	float m_bullet_max;
	//�ǂ����̏e�Ō����̐؂�ւ��p
	int m_shot;
	float m_reload;
	//�{�^���𗣂��܂ł̕b��
	float m_out_cnt;

	float key_length;

	float move_speed;
	float roll_speed;
	//���j�U�����̃v���C���[�̋�����l
	float m_recoil;

	float cam_ang;
	
	Player(const CVector3D& pos);
	void Update();
	void Render();

	void Damage(Base* attaker, int damage, float burstpow);
	void Collision(Base* b);
	/// <summary>
	/// �A�j���[�V�����𕪂��邩�ǂ���
	/// 0�̎��͑S�g��������
	/// 1�̎��͏㔼�g�Ɖ����g�ŕʂ̃A�j���[�V��������������
	/// </summary>
	void LockOnTargetE(float range, int lockdir);
	void ChangeAnimationLayer(int layer);
#if _DEBUG
	void Draw();
#endif

};
