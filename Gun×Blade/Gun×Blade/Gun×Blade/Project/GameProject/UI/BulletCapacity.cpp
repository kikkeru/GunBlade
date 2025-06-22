#include "BulletCapacity.h"

BulletCapacity::BulletCapacity(GaugeType gauge_type, float scale) :Base(eUI), m_visibility(true) {
	m_capacity_img = COPY_RESOURCE("Capacity", CImage);
	m_par = 0;
	m_gauge_type = gauge_type;
	m_gauge_scale = scale;

}

void BulletCapacity::Draw() {
	//イベント中は描画しない
	if (Base::FindObject(eEventCamera)) return;

	if (!m_visibility) return;

	//Z値が負の数（座標が画面の後ろにある）の場合は表示しない
	if (m_pos.z < 0) return;
	struct SGaugeData {		//元画像の切り抜きデータ
		CRect frame_rect;	//フレームの矩形
		CRect bar_rect;		//バーの矩形
		CVector2D bar_pos;	//バーの表示位置（フレームからの相対的な位置）
	}gauge_data[(unsigned int)GaugeType::eMax] = {
		{CRect(1,183,1059,325),CRect(1, 1, 1059, 144),CVector2D(0, 0)},
	};
	SGaugeData* d = &gauge_data[(unsigned int)m_gauge_type];

	//枠の表示
	m_capacity_img.SetRect(d->frame_rect.m_left, d->frame_rect.m_top, d->frame_rect.m_right, d->frame_rect.m_bottom);
	m_capacity_img.SetSize(d->frame_rect.m_width* m_gauge_scale, d->frame_rect.m_height* m_gauge_scale);
	m_capacity_img.SetPos(CVector2D(m_pos.x, m_pos.y));
	m_capacity_img.Draw();


	//ゲージの表示
	m_capacity_img.SetRect(d->bar_rect.m_left, d->bar_rect.m_top, d->bar_rect.m_right - d->bar_rect.m_width * (1.0 - m_par), d->bar_rect.m_bottom);
	m_capacity_img.SetSize(d->bar_rect.m_width * m_gauge_scale * m_par, d->bar_rect.m_height * m_gauge_scale);
	m_capacity_img.SetPos(CVector2D(m_pos.x, m_pos.y) + d->bar_pos * m_gauge_scale);
	m_capacity_img.Draw();
}

void BulletCapacity::SetValue(float par)
{
	m_par = par;
}
