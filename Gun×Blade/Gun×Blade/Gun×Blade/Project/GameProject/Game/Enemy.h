#pragma once
#include "../Base/Base.h"
#include"Player.h"
#include "SoundManager.h"

class Enemy : public Base {

	SoundManager sound;

	CVector3D m_key_dir;
	//�G�̎U��΂�
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
	CVector3D m_rot_target;		//��]�ڕW�l

	float m_cnt;
	//�҂�����
	int m_wait_time;
	//�U����
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
	//�����オ��r���ɓ|�����
	void StateDeathF();
	void StateDeathS();
	void StateDeath();

public:
	CModelA3M m_model;

	int m_state;
	//�W�����v�U�������̈ړ����x
	float jmove_speed;
	//���g����v���C���[�ւ̃x�N�g��
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

	//�G�̊e���ʂƂ̔���
	//�J�v�Z���f�[�^�\����
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

	//���ʂ��Ƃ̃J�v�Z��
	static Index m_culsule_idx[eCap_Max];
	Cap m_cupsule[eCap_Max];
	Cap m_arm;

	//�Ƃт�����N�[���^�C��
	float m_jump_recharge;

	float move_speed;
	float m_hp;
	//�W���X�g������ꂽ�Ƃ��ɒx���Ȃ�b��
	float m_slow_time;
	//���j�U�����̋�����l
	float m_bom_pow;

	//��̉�]�l
	float yaw;
	// ��]�l�̏��
	float maxYaw;
	// ��]�l�̉���
	float minYaw;

	//�U�����肩�ǂ���
	bool m_attack_flag;
	//�������U�������ǂ���
	bool m_action_flag;
	//�����ȊO���U�������ǂ���
	bool m_npc_act_flag;
	////����ƃp���B���ꂽ���̃t���O
	bool m_act_flag;
	//�|���ꂽ���ǂ���
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
