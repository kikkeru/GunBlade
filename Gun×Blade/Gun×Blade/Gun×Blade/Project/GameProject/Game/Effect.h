#pragma once
#include "../Base/Base.h"

//エフェクトクラス
class CParticleEffect : public Base {
private:
	//Baseクラスで定義済み
	CModelObj	m_model;		//モデル
	//	CVector3D	m_pos;			//座標
	CVector3D	m_vec;			//移動量
	CVector3D   m_accel;
	float		m_scale;		//大きさ
	float		m_alpha;		//不透明度
	float       m_change_scale; //変化させるスケール値

	bool		m_buil_bord;    //表示タイプ
	bool		m_blend;		//ブレンドタイプ
public:
	/*!
	@brief	コンストラクタ
	@param	model			[in] 使用モデル
	@param	pos				[in] 発生位置
	@param	vec				[in] 移動量
	@param	scale			[in] 初期スケール
	@param	change_scale	[in] スケール変化量
	@param	buil_bord		[in] ビルボード表示
	@param	blend			[in] 加算ブレンド
	**/
	CParticleEffect(const char* model, const CVector3D& pos, const CVector3D& vec, float scale, float change_scale, bool buil_bord, bool blend);
	/*!
	@brief	デストラクタ
	**/
	~CParticleEffect();
	/*!
	@brief	更新
	**/
	void Update();
	/*!
	@brief	描画
	**/
	void Render();
};


class CMagicEffect : public Base {
	CModelObj	m_model;		//モデル
	float		m_scale;		//大きさ
	int			m_time;
	float		m_alpha;		//不透明度
public:
	/*!
		@brief	コンストラクタ
		@param	model			[in] 使用モデル
		@param	pos				[in] 発生位置
		@param	scale			[in] 大きさ
		@param	time			[in] 表示時間
	**/
	CMagicEffect(const char* model, const CVector3D& pos, float scale, int time);
	/*!
	@brief	デストラクタ
	**/
	~CMagicEffect();
	/*!
	@brief	更新
	**/
	void Update();
	/*!
	@brief	描画
	**/
	void Render();
};
class CUVEffect : public Base {
	CModelObj	m_model;		//モデル
	float		m_scale;		//大きさ
	int			m_time;
	float		m_alpha;		//不透明度
	CVector2D	m_st;			//UVスライド
	CVector2D	m_st_vec;		//UVスライド量
public:
	/*!
		@brief	コンストラクタ
		@param	model			[in] 使用モデル
		@param	pos				[in] 発生位置
		@param	rot				[in] 回転値
		@param	scale			[in] 大きさ
		@param	vec				[in] UVスライド量
		@param	time			[in] 表示時間
	**/
	CUVEffect(const char* model, const CVector3D& pos, const CVector3D& rot, float scale, const CVector2D& vec, int time);
	/*!
	@brief	デストラクタ
	**/
	~CUVEffect();
	/*!
	@brief	更新
	**/
	void Update();
	/*!
	@brief	描画
	**/
	void Render();
};

class CEffect :public Base {
private:
	CModelObj	m_model;		//モデル
	//	CVector3D	m_pos;			//!座標
	CVector3D	m_vec;			//!移動量
	CVector3D	m_vec_accel;	//!移動量増加
	CVector3D	m_rot;			//!回転値
	CVector3D	m_rot_vec;		//!回転量
	int			m_blend;		//!ブレンド種類
	bool		m_builbord;		//!ビルボード描画設定
	float		m_scale;		//!スケール
	float		m_scale_speed;	//!スケール変化量
	float		m_alpha;		//!アルファ値
	float		m_alpha_speed;	//!アルファ値変化量
	int			m_time;			//!表示時間
public:
	enum {
		eBlendAlpha,		//通常ブレンド
		eBlendAdd,			//加算ブレンド
	};
	/*!
		@brief	コンストラクタ
		@param	model			[in] 使用モデル
		@param	pos				[in] 発生位置
		@param	vec				[in] 移動量
		@param	accel			[in] 移動変化量
		@param	scale			[in] 初期スケール
		@param	change_scale	[in] スケール変化量
		@param	change_alpha	[in] アルファ値変化量
		@param	rot				[in] 回転値
		@param	rot_vec			[in] 回転値変化量
		@param	blend			[in] ブレンド方法
		@param	buil_bord		[in] ビルボード表示
		@param	time			[in] 表示時間
	**/
	CEffect(const char* model, const CVector3D& pos, const CVector3D& vec, const CVector3D& accel, float scale, float change_scale, float change_alpha, int blend, bool builbord, const CVector3D& rot, const CVector3D& rot_vec, int time = -1);
	~CEffect();
	void Update();
	void Render();

};