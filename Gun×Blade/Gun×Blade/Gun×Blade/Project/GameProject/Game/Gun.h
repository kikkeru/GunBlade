#pragma once

#include "../stdafx.h"
#include "../Base/Base.h"

class Gun : public Base {
public:
	//モデルオブジェクト
	CModelObj m_model;
	Base* mp_parent_obj;
	CMatrix m_local_matrix;
	CMatrix m_world_matrix;
	int		m_parent_idx;
	bool	m_shot;
	int		m_pow;
public:
	//コンストラクタ
	Gun();
	//更新
	void Update();
	//描画
	void Render();
	void SetDir(const CVector3D& dir);
	void Shot(float sp, int pow);
};