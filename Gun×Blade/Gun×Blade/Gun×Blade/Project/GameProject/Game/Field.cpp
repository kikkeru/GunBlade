#include "Field.h"
#include "Wall.h"
#include "SoundManager.h"


Field::Field(int area)
	:Base(eField)
{
	m_area = area;
	//ステージ切り替え
	switch (m_area)
	{
	case 0:
		sound.Loop("FieldBGM1");
		m_model = COPY_RESOURCE("Field2", CModelObj);
		break;
	case 1:
		sound.Stop("FieldBGM1");
		sound.Loop("BossBGM");
		m_model = COPY_RESOURCE("Field", CModelObj);
		Base::Add(new Wall(CVector3D(-15, -1, -4), CVector3D(0, 0, 0)));
		Base::Add(new Wall(CVector3D(-14.5, -1, -19.5), CVector3D(0, 0, 0)));
		Base::Add(new Wall(CVector3D(-0, -1, -20), CVector3D(0, 0, 0)));
		Base::Add(new Wall(CVector3D(15, -1, -19.5), CVector3D(0, 0, 0)));
		Base::Add(new Wall(CVector3D(15, -1, -4), CVector3D(0, 0, 0)));
	//チュートリアル用のステージ
	case 2:
		m_model = COPY_RESOURCE("Field", CModelObj);
		break;
	}
	
}

void Field::Update()
{
}

void Field::Render()
{
	m_model.Render();
}
CModel* Field::GetModel() {
	return &m_model;
}