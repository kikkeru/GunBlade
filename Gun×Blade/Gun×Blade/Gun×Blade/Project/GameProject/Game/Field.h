#pragma once
#include "../Base/Base.h"
#include "SoundManager.h"


class Field : public Base {
private:
	int m_area;
	CModelObj m_model;
	
public:
	SoundManager sound;

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	Field(int area);
	/// <summary>
	/// �X�V����
	/// </summary>
	void Update();
	/// <summary>
	/// �`�揈��
	/// </summary>
	void Render();

	/// <summary>
	/// ���f���̎擾(Base����I�[�o�[���C�h)
	/// </summary>
	/// <returns></returns>
	CModel* GetModel();
};