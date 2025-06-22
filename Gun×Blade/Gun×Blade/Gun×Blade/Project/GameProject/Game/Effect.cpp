#include "Effect.h"

CParticleEffect::CParticleEffect(const char* model, const CVector3D& pos, const CVector3D& _vec, float _scale, float _change_scale, bool _buil_bord, bool blend) :
	Base(eEffect),
	m_vec(_vec), m_scale(_scale), m_alpha(1.0f) {
	m_pos = pos;
	m_model = COPY_RESOURCE(model, CModelObj);
	m_accel = _vec;
	m_change_scale = _change_scale;
	m_buil_bord = _buil_bord;
	m_blend = blend;
	m_model.RenderBuilbord();

}
CParticleEffect::~CParticleEffect() {

}
void CParticleEffect::Update() {
	//����
	m_vec += m_accel;
	//�ړ�
	m_pos += m_vec;
	//������������(�傫��)
	m_scale += m_change_scale;
	//������������
	m_alpha -= 0.02f;
	if (m_alpha <= 0) {
		SetKill();
	}

}
void CParticleEffect::Render() {
	//���C�e�B���OOFF
	CLight::SetLighting(false);
	//�[�x�o�b�t�@�̏������݂Ȃ�
	glDepthMask(GL_FALSE);
	//���Z�u�����h
	if (m_blend) glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	m_model.SetPos(m_pos);
	m_model.SetScale(m_scale, m_scale, m_scale);
	m_model.GetMaterial(0)->m_alpha = m_alpha;
	//�r���{�[�h�`��
	if (m_buil_bord) m_model.RenderBuilbord();
	else m_model.Render();

	//���̐ݒ�֖߂�
	if (m_blend) glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);
	CLight::SetLighting(true);

}


CMagicEffect::CMagicEffect(const char* model, const CVector3D& pos, float scale, int time) :
	Base(eEffect),
	m_scale(scale), m_time(time), m_alpha(1.0f) {
	m_pos = pos;
	m_model = COPY_RESOURCE(model, CModelObj);
	m_rot.x = DtoR(90);
}
CMagicEffect::~CMagicEffect() {

}
void CMagicEffect::Update() {
	m_time--;
	if (m_time < 30) {
		m_alpha -= 1.0f / 30;
	}
	if (m_time <= 0) {
		SetKill();
	}
	m_rot.y += DtoR(1.0f);

}
void CMagicEffect::Render() {
	//���C�e�B���OOFF
	CLight::SetLighting(false);
	//�[�x�o�b�t�@�̏������݂Ȃ�
	glDepthMask(GL_FALSE);

	m_model.SetRot(m_rot);
	m_model.SetPos(m_pos);
	m_model.SetScale(m_scale, m_scale, m_scale);
	m_model.GetMaterial(0)->m_alpha = m_alpha;

	m_model.Render();

	glDepthMask(GL_TRUE);
	CLight::SetLighting(true);

}

CUVEffect::CUVEffect(const char* model, const CVector3D& pos, const CVector3D& rot, float scale, const CVector2D& vec, int time) :
	Base(eEffect),
	m_scale(scale), m_time(time), m_st(0, 0), m_st_vec(vec), m_alpha(1.0f) {
	m_pos = pos;
	m_rot = rot;
	m_model = COPY_RESOURCE(model, CModelObj);
}
CUVEffect::~CUVEffect() {

}
void CUVEffect::Update() {
	m_time--;
	m_st += m_st_vec;

	if (m_time <= 0) {
		SetKill();
	}

}
void CUVEffect::Render() {
	//���C�e�B���OOFF
	CLight::SetLighting(false);

	//�[�x�o�b�t�@�̏������݂Ȃ�
	glDepthMask(GL_FALSE);

	m_model.SetRot(m_rot);
	m_model.SetPos(m_pos);
	m_model.SetScale(m_scale, m_scale, m_scale);
	m_model.GetMaterial(0)->m_alpha = m_alpha;
	m_model.GetMaterial(0)->m_st = m_st;

	m_model.Render();

	glDepthMask(GL_TRUE);

	CLight::SetLighting(true);

}
CEffect::CEffect(const char* model, const CVector3D& pos, const CVector3D& vec, const CVector3D& accel, float scale, float change_scale, float change_alpha, int blend, bool builbord, const CVector3D& rot, const CVector3D& rot_vec, int time)
	:Base(eEffect),
	m_alpha(1.0f),
	m_vec(vec),
	m_vec_accel(accel),
	m_rot(rot),
	m_rot_vec(rot_vec),
	m_scale(scale),
	m_scale_speed(change_scale),
	m_alpha_speed(change_alpha),
	m_blend(blend),
	m_builbord(builbord),
	m_time(time) {
	m_pos = pos;
	m_model = COPY_RESOURCE(model, CModelObj);

}
CEffect::~CEffect() {

}
void CEffect::Update() {
	m_vec += m_vec_accel;
	m_pos += m_vec;
	m_rot += m_rot_vec;
	m_scale += m_scale_speed;
	m_alpha += m_alpha_speed;
	if (m_time > 0) m_time--;
	if (m_alpha < 0 || m_scale < 0 || m_time == 0) SetKill();
}
void CEffect::Render() {
	//���C�e�B���O���s��Ȃ�
	CLight::SetLighting(false);
	//�[�x�o�b�t�@�ւ̏�������OFF
	glDepthMask(GL_FALSE);
	switch (m_blend) {
	case eBlendAlpha:
		//�ʏ�u�����h���[�h��
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case eBlendAdd:
		//���Z�u�����h���[�h��
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	}
	//���W�ݒ�
	m_model.SetPos(m_pos);
	//�}�e���A���̃A���t�@�l�ݒ�
	m_model.GetMaterial(0)->m_alpha = m_alpha;
	//�X�P�[���ݒ�
	m_model.SetScale(m_scale, m_scale, m_scale);
	//��]�l�ݒ�
	m_model.SetRot(m_rot);
	if (m_builbord)
		//�r���{�[�h�`��
		m_model.RenderBuilbord(CCamera::GetCurrent()->GetBuilbordMatrix());
	else
		//�ʏ�`��
		m_model.Render();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);
	//CBase::RenderSphere(m_pos, 8.0f, CVector4D(1, 0, 0, 0.5));
	CLight::SetLighting(true);
}