#pragma once

#include "../stdafx.h"
#include "../Base/Base.h"

class Gun : public Base {
public:
	//���f���I�u�W�F�N�g
	CModelObj m_model;
	Base* mp_parent_obj;
	CMatrix m_local_matrix;
	CMatrix m_world_matrix;
	int		m_parent_idx;
	bool	m_shot;
	int		m_pow;
public:
	//�R���X�g���N�^
	Gun();
	//�X�V
	void Update();
	//�`��
	void Render();
	void SetDir(const CVector3D& dir);
	void Shot(float sp, int pow);
};