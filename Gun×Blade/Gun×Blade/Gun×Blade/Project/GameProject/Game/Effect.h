#pragma once
#include "../Base/Base.h"

//�G�t�F�N�g�N���X
class CParticleEffect : public Base {
private:
	//Base�N���X�Œ�`�ς�
	CModelObj	m_model;		//���f��
	//	CVector3D	m_pos;			//���W
	CVector3D	m_vec;			//�ړ���
	CVector3D   m_accel;
	float		m_scale;		//�傫��
	float		m_alpha;		//�s�����x
	float       m_change_scale; //�ω�������X�P�[���l

	bool		m_buil_bord;    //�\���^�C�v
	bool		m_blend;		//�u�����h�^�C�v
public:
	/*!
	@brief	�R���X�g���N�^
	@param	model			[in] �g�p���f��
	@param	pos				[in] �����ʒu
	@param	vec				[in] �ړ���
	@param	scale			[in] �����X�P�[��
	@param	change_scale	[in] �X�P�[���ω���
	@param	buil_bord		[in] �r���{�[�h�\��
	@param	blend			[in] ���Z�u�����h
	**/
	CParticleEffect(const char* model, const CVector3D& pos, const CVector3D& vec, float scale, float change_scale, bool buil_bord, bool blend);
	/*!
	@brief	�f�X�g���N�^
	**/
	~CParticleEffect();
	/*!
	@brief	�X�V
	**/
	void Update();
	/*!
	@brief	�`��
	**/
	void Render();
};


class CMagicEffect : public Base {
	CModelObj	m_model;		//���f��
	float		m_scale;		//�傫��
	int			m_time;
	float		m_alpha;		//�s�����x
public:
	/*!
		@brief	�R���X�g���N�^
		@param	model			[in] �g�p���f��
		@param	pos				[in] �����ʒu
		@param	scale			[in] �傫��
		@param	time			[in] �\������
	**/
	CMagicEffect(const char* model, const CVector3D& pos, float scale, int time);
	/*!
	@brief	�f�X�g���N�^
	**/
	~CMagicEffect();
	/*!
	@brief	�X�V
	**/
	void Update();
	/*!
	@brief	�`��
	**/
	void Render();
};
class CUVEffect : public Base {
	CModelObj	m_model;		//���f��
	float		m_scale;		//�傫��
	int			m_time;
	float		m_alpha;		//�s�����x
	CVector2D	m_st;			//UV�X���C�h
	CVector2D	m_st_vec;		//UV�X���C�h��
public:
	/*!
		@brief	�R���X�g���N�^
		@param	model			[in] �g�p���f��
		@param	pos				[in] �����ʒu
		@param	rot				[in] ��]�l
		@param	scale			[in] �傫��
		@param	vec				[in] UV�X���C�h��
		@param	time			[in] �\������
	**/
	CUVEffect(const char* model, const CVector3D& pos, const CVector3D& rot, float scale, const CVector2D& vec, int time);
	/*!
	@brief	�f�X�g���N�^
	**/
	~CUVEffect();
	/*!
	@brief	�X�V
	**/
	void Update();
	/*!
	@brief	�`��
	**/
	void Render();
};

class CEffect :public Base {
private:
	CModelObj	m_model;		//���f��
	//	CVector3D	m_pos;			//!���W
	CVector3D	m_vec;			//!�ړ���
	CVector3D	m_vec_accel;	//!�ړ��ʑ���
	CVector3D	m_rot;			//!��]�l
	CVector3D	m_rot_vec;		//!��]��
	int			m_blend;		//!�u�����h���
	bool		m_builbord;		//!�r���{�[�h�`��ݒ�
	float		m_scale;		//!�X�P�[��
	float		m_scale_speed;	//!�X�P�[���ω���
	float		m_alpha;		//!�A���t�@�l
	float		m_alpha_speed;	//!�A���t�@�l�ω���
	int			m_time;			//!�\������
public:
	enum {
		eBlendAlpha,		//�ʏ�u�����h
		eBlendAdd,			//���Z�u�����h
	};
	/*!
		@brief	�R���X�g���N�^
		@param	model			[in] �g�p���f��
		@param	pos				[in] �����ʒu
		@param	vec				[in] �ړ���
		@param	accel			[in] �ړ��ω���
		@param	scale			[in] �����X�P�[��
		@param	change_scale	[in] �X�P�[���ω���
		@param	change_alpha	[in] �A���t�@�l�ω���
		@param	rot				[in] ��]�l
		@param	rot_vec			[in] ��]�l�ω���
		@param	blend			[in] �u�����h���@
		@param	buil_bord		[in] �r���{�[�h�\��
		@param	time			[in] �\������
	**/
	CEffect(const char* model, const CVector3D& pos, const CVector3D& vec, const CVector3D& accel, float scale, float change_scale, float change_alpha, int blend, bool builbord, const CVector3D& rot, const CVector3D& rot_vec, int time = -1);
	~CEffect();
	void Update();
	void Render();

};