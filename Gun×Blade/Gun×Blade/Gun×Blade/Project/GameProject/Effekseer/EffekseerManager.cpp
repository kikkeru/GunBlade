
#include "EffekseerManager.h"

EffekseerManager *EffekseerManager::m_instance = nullptr;

EffekseerManager::EffekseerManager() :
	Base(eEffectManager)
{
	//�G�t�F�N�g�̐�ǂ�
	struct EFK_DATA {
		std::string name;
		std::u16string efk;
	}
	efk_data[] = {
		//���K
		{ "Roar",u"Effect/01_Pierre01/Roar.efk" },
		//��]�؂�
		{ "Rotarycut",u"Effect/MAGICALxSPIRAL/Rotarycut.efk" },
		//�e�������������̃G�t�F�N�g
		{ "BulletImpact",u"Effect/MAGICALxSPIRAL/BulletImpact.efk" },
		//�����Ă���Ƃ��̑����̃G�t�F�N�g
		{ "foot_smoke1",u"Effect/MAGICALxSPIRAL/foot_smoke1.efk" },
		//�v���C���[�_���[�W
		{ "Blow11",u"Effect/tktk01/Blow11.efk" },
		//�ːi�_���[�W
		{ "Blow3",u"Effect/tktk01/Blow3.efk" },
		//����
		{ "Fire2",u"Effect/MAGICALxSPIRAL/Salamander4.efk" },
		//�v���C���[�̒ʏ�U��
		{ "Sword1",u"Effect/NextSoft01/Sword1.efk" },
		//�h�A�̃G�t�F�N�g
		{ "MagicShield",u"Effect/NextSoft01/MagicShield.efk" },
		//�G�̎��S���o
		{ "Death",u"Effect/NextSoft01/Death.efk" }
	};


	// Create a manager of effects
	// �G�t�F�N�g�̃}�l�[�W���[�̍쐬
	m_manager = ::Effekseer::Manager::Create(8000);

	// Create a  graphics device
	// �`��f�o�C�X�̍쐬
	auto graphicsDevice = ::EffekseerRendererGL::CreateGraphicsDevice(::EffekseerRendererGL::OpenGLDeviceType::OpenGL3);
	// Create a renderer of effects
	// �G�t�F�N�g�̃����_���[�̍쐬
	m_renderer = ::EffekseerRendererGL::Renderer::Create(graphicsDevice,8000);
	
	
	//std::shared_ptr<EffekseerRenderer::ExternalShaderSettings> shader_setting(new EffekseerRenderer::ExternalShaderSettings());
    //shader_setting->StandardShader = graphicsDevice->CreateShaderFromCodes({ model_unlit_vs_gl3 }, { model_unlit_ps_gl3 });
   // shader_setting->Blend = Effekseer::AlphaBlendType.Blend;
    //m_renderer->SetExternalShaderSettings(shader_setting);
	
	
	// Sprcify rendering modules
	// �`�惂�W���[���̐ݒ�
	m_manager->SetSpriteRenderer(m_renderer->CreateSpriteRenderer());
	m_manager->SetRibbonRenderer(m_renderer->CreateRibbonRenderer());
	m_manager->SetRingRenderer(m_renderer->CreateRingRenderer());																							
	m_manager->SetModelRenderer(m_renderer->CreateModelRenderer());

	//�E����W�n�ɐݒ�
	m_manager->SetCoordinateSystem(Effekseer::CoordinateSystem::RH);

	// Specify a texture, model, curve and material loader
	// It can be extended by yourself. It is loaded from a file on now.
	// �e�N�X�`���A���f���A�J�[�u�A�}�e���A�����[�_�[�̐ݒ肷��B
	// ���[�U�[���Ǝ��Ŋg���ł���B���݂̓t�@�C������ǂݍ���ł���B
	// �`��p�C���X�^���X����e�N�X�`���̓Ǎ��@�\��ݒ�
	// �Ǝ��g���\�A���݂̓t�@�C������ǂݍ���ł���B
	m_manager->SetTextureLoader(m_renderer->CreateTextureLoader());
	m_manager->SetModelLoader(m_renderer->CreateModelLoader());
	m_manager->SetMaterialLoader(m_renderer->CreateMaterialLoader());
	m_manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

	
	// ���Đ��p�C���X�^���X�̐���
	m_sound = EffekseerSound::Sound::Create(32);

	// ���Đ��p�C���X�^���X����Đ��@�\���w��
	m_manager->SetSoundPlayer(m_sound->CreateSoundPlayer());

	// ���Đ��p�C���X�^���X����T�E���h�f�[�^�̓Ǎ��@�\��ݒ�
	// �Ǝ��g���\�A���݂̓t�@�C������ǂݍ���ł���B
	m_manager->SetSoundLoader(m_sound->CreateSoundLoader());


	//�G�t�F�N�g�̐�ǂ�
	unsigned int efk_data_size = sizeof(efk_data) / sizeof(efk_data[0]);
	for (int i = 0; i < efk_data_size; i++) {
		EFK_DATA *d = &efk_data[i];
		for(int i=0;i< m_inctane_size;i++)
			m_effect_list[d->name].m_effets.push_back(Effekseer::Effect::Create(m_manager, d->efk.c_str()));
	}
	
	if (!m_instance) m_instance = this;
}

EffekseerManager::~EffekseerManager()
{
	// �G�t�F�N�g�̒�~
	m_manager->StopAllEffects();

	for (auto& effects : m_effect_list) {
		for (auto& e : effects.second.m_effets) {
			// �G�t�F�N�g�̔j��
		//	ES_SAFE_RELEASE(e);
		}
	}

	m_sound.Reset();
	m_renderer.Reset();
	m_manager.Reset();

	m_instance = nullptr;

}

void EffekseerManager::ClearInstance()
{
	if (!m_instance) return;
	EffekseerManager::GetInstance()->SetKill();
}
void EffekseerManager::Update() {
	// �G�t�F�N�g�̍X�V�������s��
	//m_manager->Update();
	CVector3D pos = CCamera::GetCurrent()->GetPos();
	CVector3D at = pos + CCamera::GetCurrent()->GetDir();
	CVector3D up = CCamera::GetCurrent()->GetUp();
	m_sound->SetListener(Effekseer::Vector3D(pos.x, pos.y, pos.z), Effekseer::Vector3D(at.x, at.y, at.z), Effekseer::Vector3D(up.x, up.y, up.z));


	// Update the manager
	// �}�l�[�W���[�̍X�V
	Effekseer::Manager::UpdateParameter updateParameter;
	updateParameter.DeltaFrame = 60 * CFPS::GetDeltaTime();
	m_manager->Update(updateParameter);

}
void EffekseerManager::Render()
{
	

	if (CShadow::isDoShadow()) return;
	// ���e�s���ݒ�
	::Effekseer::Matrix44 mat;
	memcpy(mat.Values, CCamera::GetCurrent()->GetProjectionMatrix().f, sizeof(float) * 16);
	m_renderer->SetProjectionMatrix(mat);
	memcpy(mat.Values, CCamera::GetCurrent()->GetViewMatrix().f, sizeof(float) * 16);
	// �J�����s���ݒ�
	m_renderer->SetCameraMatrix(mat);

	
	// Render effects
	// �G�t�F�N�g�̕`����s���B
	m_renderer->BeginRendering();
	Effekseer::Manager::DrawParameter drawParameter;
	drawParameter.ZNear = 0.0f;
	drawParameter.ZFar = 1.0f;
	drawParameter.ViewProjectionMatrix = m_renderer->GetCameraProjectionMatrix();
	// �G�t�F�N�g�̕`����s���B
	m_manager->Draw(drawParameter);

	// �G�t�F�N�g�̕`��I���������s���B
	m_renderer->EndRendering();
}

Effekseer::EffectRef EffekseerManager::GetEffect(const std::string & name)
{
	Effect* e = &m_effect_list[name];
	Effekseer::EffectRef ret = e->m_effets[e->m_index];
	e->m_index = (e->m_index + 1) % e->m_effets.size();
	return ret;
}
