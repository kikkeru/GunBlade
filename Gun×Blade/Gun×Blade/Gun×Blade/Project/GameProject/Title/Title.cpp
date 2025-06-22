#include"Title.h"
#include"Game/Game.h"
#include"Game/Credit.h"
#include"Game/Tutorial.h"

Title::Title() :Base(eType_Scene),
m_title_text("C:\\\Windows\\Fonts\\msgothic.ttc", 32){
	m_font_img = COPY_RESOURCE("TitleFont", CImage);
	m_menu_img = COPY_RESOURCE("Menu", CImage);
	m_menu2_img = COPY_RESOURCE("Menu2", CImage);
	font_alpha = 0.0f;
	//�r�f�I�ǂݍ���
	m_title = new CVideo("Movie/GameTitle.mp4");
	m_loop = new CVideo("Movie/Loop.mp4");
	m_lord = new CVideo("Movie/NowLording.mp4");

	m_title->Play(false);
	m_loop_flag = false;
	m_push_font = true;
	m_lord_flag = false;
	m_menu_select = 0;
	m_menu_now = 0;
	m_menu_cnt = 1.0f;
}

Title::~Title() {
	m_lord->Stop();
	delete m_loop;
	delete m_title;
	delete m_lord;

	//�S�ẴI�u�W�F�N�g��j��
	Base::KillALL();

	//�Q�[���X�^�[�g
	if (m_menu_select == 0) {
		Base::Add(new Game());
	}
	//�`���[�g���A��
	else if (m_menu_select == 1) {
		Base::Add(new Tutorial());
	}
	else {
		Base::Add(new Credit());
	}
}


void Title::Update() {

	//�^�C�g���r�f�I���I������烋�[�v�f���ɐ؂�ւ�
	if (m_title && m_title->isEnd() && m_loop_flag == false) {
		//�^�C�g���r�f�I���폜
		m_title->Stop();
		delete m_title;
		m_title = nullptr;

		sound.Loop("TitleBGM");
		//���[�v����^�C�g���ɕύX����
		m_loop->Play(true);

		m_loop_flag = true;
	}

	//�^�C�g����Z�L�[�ŃX�L�b�v
	if (m_title && m_title->isPlay()) {
		if (PUSH(CInput::eButton1)) {
			m_title->Stop();
			delete m_title;
			m_title = nullptr;

			sound.Loop("TitleBGM");
			m_loop->Play(true);
			m_loop_flag = true;
		}
	}
	//���[�v�^�C�g���Ȃ烁�j���[����
	else {
		if (m_loop_flag == true) {

			if (PUSH(CInput::eButton1) && m_menu_now == 0) {
				m_push_font = !m_push_font;
				m_menu_now = 1;
				m_menu_cnt = 1.0f;
			}
		}
	}

	if (m_lord_flag) {
		//���[�h�����ǂ���
		if (CLoadThread::GetInstance()->CheckEnd()) {
			//����
			SetKill();
		}
	}
}

void Title::Draw() {
	if (m_title) {
		m_title->SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		m_title->Draw();
	}
	else if(m_lord_flag==false){
		m_loop->SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		m_loop->Draw();
		if (m_push_font) {
			Font();
		}
		else {
			Menu();
		}
	}
	else {
		m_lord->SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		m_lord->Draw();
	}
		
}

void Title::Font()
{
	CVector2D font_pos(600, 700);

	//�Ɖ��Z
	font_alpha += 0.05f;
	if (font_alpha > DtoR(180)) {
		font_alpha = 0.0f;
	}

	//�ʏ�̕����̕`��
	m_font_img.SetSize(678, 52);
	m_font_img.SetPos(font_pos);
	m_font_img.SetRect(0, 0, 678, 52);
	m_font_img.SetColor(1, 1, 1, 1);
	m_font_img.Draw();

	//���镶�����d�˂ĕ`��
	m_font_img.SetRect(0, 70, 678, 125);
	//sin�J�[�u�̓������g���A0��1��0�փ��l���ω�
	float alpha = (sin(font_alpha) + 1) / 2;
	m_font_img.SetColor(1, 1, 1, sin(font_alpha));
	m_font_img.Draw();

}

//���j���[���
void Title::Menu()
{
	CVector2D menu_pos(700, 700);
	m_title_text.Draw(100, 1000, 1.0f, 1.0f, 1.0f, "Z�L�[�Ō���@�����L�[�܂���WS�L�[�ňړ�");

	if (PUSH(CInput::eUp) || PUSH(CInput::eUpKey)) {
		if (m_menu_select > 0) {
			sound.Play("cursor");
			m_menu_select--;
		}
	}

	if (PUSH(CInput::eDown) || PUSH(CInput::eDownKey)) {
		if (m_menu_select < 2) {
			sound.Play("cursor");
			m_menu_select++;
		}
	}

	//�c�ɕ��ׂĉ摜��\��
	for (int i = 0; i < 3; i++) {
		int x = 0;
		int y = i * 64;

		m_menu_img.SetRect(0, i * 64, 360, (i + 1) * 64);
		m_menu_img.SetSize(357, 64);
		m_menu_img.SetPos(menu_pos + CVector2D(x, y));
		m_menu_img.Draw();

		//�I�𒆂̉摜�͌��点��
		if (m_menu_select == i) {
			m_menu2_img.SetRect(0, i * 64, 360, (i + 1) * 64);
			m_menu2_img.SetSize(357, 64);
			m_menu2_img.SetPos(menu_pos + CVector2D(x, y));
			m_menu2_img.Draw();
		}
	}

	//�d������ň�C�ɐi�܂Ȃ��悤�ɂ���
	if (m_menu_cnt > 0) {
		m_menu_cnt--;

	}
	//�I������
	else if (PUSH(CInput::eButton1)) {
		if (m_menu_select == 0) {
			sound.Play("select");
			m_lord_flag = true;
			sound.Stop("TitleBGM");
			//���[�h�J�n
			CLoadThread::GetInstance()->LoadStart(Load);
			m_loop->Stop();
			//���[�h�����[�r�[
			m_lord->Play(true);
		}
		else {
			//�Q�[���X�^�[�g�ȊO���[�h�͂Ȃ��Ă���
			SetKill();
		}
	}
}

void Title::Load()
{
	//�L�����N�^�[
	ADD_RESOURCE("Player", CModel::CreateModel("Charactor/swat/swat.a3m"));
	ADD_RESOURCE("Enemy", CModel::CreateModel("Charactor/assassin/assassin.a3m"));
	ADD_RESOURCE("Mutant", CModel::CreateModel("Charactor/mutant/mutant.a3m"));
	//�t�B�[���h
	ADD_RESOURCE("Field", CModel::CreateModel("Field/TestStage/TestStage.obj", 16, 1, 16));
	ADD_RESOURCE("Field2", CModel::CreateModel("Field/Dungeon/Dungeon.obj", 16, 1, 16));

	//����ȃI�u�W�F�N�g
	ADD_RESOURCE("Pillar", CModel::CreateModel("Field/TestStage/Pillar.a3m", 16, 1, 16));

	//����
	ADD_RESOURCE("Sword", CModel::CreateModel("Wepon/Sword/Sword.obj", 16, 1, 16));
	ADD_RESOURCE("Gun", CModel::CreateModel("Wepon/Gun/Gun.obj", 16, 1, 16));

	//UI
	ADD_RESOURCE("Gauge", CImage::CreateImage("UI/gauge.png"));
	ADD_RESOURCE("Capacity", CImage::CreateImage("UI/Bullets.png"));

	//�G�t�F�N�g
	ADD_RESOURCE("CircleW", CModel::CreateModel("Effect/CircleW.obj"));
	ADD_RESOURCE("CircleR", CModel::CreateModel("Effect/CircleR.obj"));
	ADD_RESOURCE("CircleM", CModel::CreateModel("Effect/CircleM1.obj"));

}
