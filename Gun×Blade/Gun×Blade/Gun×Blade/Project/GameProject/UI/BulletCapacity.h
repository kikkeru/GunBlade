#pragma once
#include "../Base/Base.h"

class BulletCapacity :public Base {
public:
	enum class GaugeType {
		//‘•“U‚³‚ê‚Ä‚¢‚é’e
		eBulletsGauge,
		eBulletsGauge2,
		eMax
	};

private:
	CImage m_capacity_img;

	float m_par;
	GaugeType m_gauge_type;
	CImage m_img;
	float m_gauge_scale;
	bool m_visibility;

public:
	BulletCapacity(GaugeType gauge_type, float scale);
	void Draw();
	void SetValue(float par);
	void SetVisibility(bool b) {
		m_visibility = b;
	}
};
