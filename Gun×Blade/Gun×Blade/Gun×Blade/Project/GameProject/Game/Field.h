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
	/// コンストラクタ
	/// </summary>
	Field(int area);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Render();

	/// <summary>
	/// モデルの取得(Baseからオーバーライド)
	/// </summary>
	/// <returns></returns>
	CModel* GetModel();
};