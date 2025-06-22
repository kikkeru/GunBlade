#include"Tutorial.h"
#include"Field.h"
#include"Player.h"
#include"Enemy.h"
#include"Door.h"
#include "FollowCamera.h"
#include"../Title/Title.h"

Tutorial::Tutorial() :Base(eType_Scene) ,
m_tutorial_text("C:\\\Windows\\Fonts\\msgothic.ttc", 32) {
	m_menu_img = COPY_RESOURCE("Dark", CImage);
	m_tutorial_img = COPY_RESOURCE("Tutorial", CImage);
	m_tutorial2_img = COPY_RESOURCE("Tutorial2", CImage);

	m_menu_select = 0;
	m_practice_flag = false;
	m_draw_flag = false;
}

Tutorial::~Tutorial() {
	Base::KillALL();
	Base::Add(new Title());
}

void Tutorial::Update() {

	Base* p = Base::FindObject(ePlayer);

	if (PUSH(CInput::eButton14)) {
		sound.Stop("TitleBGM");
		SetKill();
	}
}

void Tutorial::Draw()
{
	if (m_practice_flag == false) {
		m_draw_flag = false;
		Menu();
	}
	else {
		Practice();
	}
	m_tutorial_text.Draw(100, 1000, 1.0f, 1.0f, 1.0f, "X�L�[�őO�ɖ߂�     K�L�[�Ń^�C�g���ɖ߂�");
}

void Tutorial::Menu()
{
	m_menu_img.Draw();
	CVector2D menu_pos(700, 400);


	if (PUSH(CInput::eUp) || PUSH(CInput::eUpKey)) {
		if (m_menu_select > 0) {
			sound.Play("cursor");
			m_menu_select--;
		}
	}

	if (PUSH(CInput::eDown) || PUSH(CInput::eDownKey)) {
		if (m_menu_select < 3) {
			sound.Play("cursor");
			m_menu_select++;
		}
	}

	//�c�ɕ��ׂĉ摜��\��
	for (int i = 0; i < 4; i++) {
		int x = 0;
		int y = i * 80;

		m_tutorial_img.SetRect(0, i * 80, 438, (i + 1) * 80);
		m_tutorial_img.SetSize(438, 80);
		m_tutorial_img.SetPos(menu_pos + CVector2D(x, y));
		m_tutorial_img.Draw();

		//�I�𒆂̉摜�͌��点��
		if (m_menu_select == i) {
			m_tutorial2_img.SetRect(0, i * 80, 438, (i + 1) * 80);
			m_tutorial2_img.SetSize(438, 80);
			m_tutorial2_img.SetPos(menu_pos + CVector2D(x, y));
			m_tutorial2_img.Draw();
		}
	}

	//������@��
	if (PUSH(CInput::eButton1)) {
		sound.Play("next");
		m_practice_flag = true;
		Practice();
	}
}

void Tutorial::Practice()
{
	m_menu_img.Draw();
	//X�L�[�őO�ɖ߂�
	if (PUSH(CInput::eButton2)) {
		m_practice_flag = false;
	}

	switch (m_menu_select)
	{
		//�ړ��̎d���ɂ���
	case 0:
		m_tutorial_text.Draw(100, 100, 1.0f, 1.0f, 1.0f, "�EWASD�L�[�ňړ�");
		m_tutorial_text.Draw(100, 200, 1.0f, 1.0f, 1.0f, "�Eshift�{�^���Ń_�b�V��");
		m_tutorial_text.Draw(100, 300, 1.0f, 1.0f, 1.0f, "shift�{�^�������������Ȃ��Ă��ړ��L�[�������Ă�����葖��͌p�������");
		m_tutorial_text.Draw(100, 400, 1.0f, 1.0f, 1.0f, "����̂���߂����Ƃ��͂�����xshift�{�^������������x�~�܂�Ή��������");
		m_tutorial_text.Draw(100, 500, 1.0f, 1.0f, 1.0f, "�E�}�E�X�𓮂������ƂŃJ�����𑀍�ł���");
		m_tutorial_text.Draw(100, 600, 1.0f, 1.0f, 1.0f, "�E�}�E�X�̒��{�^���������ƕ���������ւ���");
		break;
		//���̏�Ԃł̍U�����@(�Q�[�W�U��������)
	case 1:
		m_tutorial_text.Draw(100, 100, 1.0f, 1.0f, 1.0f, "�E�}�E�X���{�^���������ƍU��(�ő�R��A���ōU���ł���)");
		m_tutorial_text.Draw(100, 200, 1.0f, 1.0f, 1.0f, "�E�}�E�X���{�^���𒷉�������Ɠ˂��h���U��");
		m_tutorial_text.Draw(100, 300, 1.0f, 1.0f, 1.0f, "���̂܂ܒ���������Ɣ��j�U���ɔh��");
		m_tutorial_text.Draw(100, 400, 1.0f, 1.0f, 1.0f, "�E�}�E�X�E�{�^���������Ȃ��獶�{�^���������ƃQ�[�W����������U��");
		m_tutorial_text.Draw(100, 500, 1.0f, 1.0f, 1.0f, "�E�}�E�X�E�{�^���������Ȃ���X�y�[�X�L�[�������ƈړ������U�������Ă�i�З͂͒Ⴍ�Ȃ�j");
		m_tutorial_text.Draw(100, 600, 1.0f, 1.0f, 1.0f, "���U����G�̍U���ɍ��킹�ē��Ă�ƃp���B������");
		break;
		//�e�̏�Ԃł̍U�����@
	case 2:
		m_tutorial_text.Draw(100, 100, 1.0f, 1.0f, 1.0f, "�E�}�E�X�E�{�^�������������č\����");
		m_tutorial_text.Draw(100, 200, 1.0f, 1.0f, 1.0f, "�\�����Ƀ}�E�X���{�^���������ƒe������");
		m_tutorial_text.Draw(100, 300, 1.0f, 1.0f, 1.0f, "�ER�L�[�Ń����[�h");
		m_tutorial_text.Draw(100, 400, 1.0f, 1.0f, 1.0f, "�����[�h���Ȃ��Ă���莞�Ԃ��Ƃɒe���ꔭ�������񕜂���");
		m_tutorial_text.Draw(100, 500, 1.0f, 1.0f, 1.0f, "�EQ�L�[�œG�����b�N�I���@������x�����ƕʂ̓G�����b�N�I��");
		m_tutorial_text.Draw(100, 600, 1.0f, 1.0f, 1.0f, "�EE�L�[�Ń��b�N�I������");
		break;
		//����ƍU���L�����Z������ɂ���
	case 3:
		m_tutorial_text.Draw(100, 100, 1.0f, 1.0f, 1.0f, "�E�X�y�[�X�L�[�ŉ��");
		m_tutorial_text.Draw(100, 200, 1.0f, 1.0f, 1.0f, "����͘A���œ��܂łł���");
		m_tutorial_text.Draw(100, 300, 1.0f, 1.0f, 1.0f, "�E��x�ڂ̉�����ړ��L�[�������Ă���΂��̂܂ܑ��邱�Ƃ��ł���");
		m_tutorial_text.Draw(100, 400, 1.0f, 1.0f, 1.0f, "�E���g�p���ɂ͍U�����ɂ��X�y�[�X�L�[�������Ɖ�����ł���");
		break;
	}
}