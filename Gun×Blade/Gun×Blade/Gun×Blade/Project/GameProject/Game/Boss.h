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
	CVector3D m_rot_target;		//��]�ڕW�l

	EventCamera* mp_camera;

	float m_cnt;
	//�W���X�g�����I������
	int m_reset_time;
	//�҂�����
	int m_wait_time;
	//�U����
	int m_attack_pow;
	//�U�����肩�ǂ���
	bool m_attack_flag;
	//������΂����肩�ǂ���
	bool m_burst_flag;
	//��Z�����ɑł������ǂ���
	bool m_attackS_flag;
	bool m_sound_flag;
	//����ƃp���B���ꂽ���̃t���O
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
	//�C�x���g�V�[�����I��������ǂ���
	bool m_event_flag;

	CModelA3M m_model;

	//�U�����ꂽ��
	int m_hit;
	int m_state;
	//�W�����v�U�������̈ړ����x
	float jmove_speed;
	//���g����v���C���[�ւ̃x�N�g��
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

	//�o�[�X�g�p�̔��a
	float m_b_rad;
	//�t���[���擾�p
	float m_frame;
	//�ːi�p������
	float m_rush_cnt;
	//���b�N�I������
	float m_look_cnt;
	//�㌄
	int m_wait_cnt;
	//�ːi�N�[���^�C��
	float m_rush_recharge;
	//�Ƃт�����N�[���^�C��
	float m_jump_recharge;
	//����s���̃N�[���^�C��
	float m_roll_recharge;
	//�J�E���^�[�̃N�[���^�C��
	float m_trick_recharge;
	//��Z���o���Ƃ��̈ʒu
	int m_set_point;
	//��Z�̃X�e�b�v����
	int m_attck_step;

	float move_speed;
	float m_hp;
	//���j�U�����̋�����l
	float m_bom_pow;

	CVector3D m_burst;

	Boss(const CVector3D& pos);

	void Update();
	void Render();

	void Damage(Base* attaker, int damage, bool breakflag, bool leanback);
	void LookToTarget();
	//���
	void LerpTarget();
	void Collision(Base* b);
	void Recharge();
	//����ƃp���B�̏���
	void Avoid(Player* p,Boss* b);
	void Parry(Player* p,Boss* b);
#if _DEBUG
	void Draw();
#endif
};
