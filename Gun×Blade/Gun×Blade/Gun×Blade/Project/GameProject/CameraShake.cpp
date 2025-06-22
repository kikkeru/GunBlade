#include "CameraShake.h"

CameraShake::CameraShake()
    : m_shake_intensity(0.0f),
    m_shake_duration(0.0f),
    m_shake_time(0.0f) {
    m_shake_offset = CVector3D(0.0f, 0.0f, 0.0f);
}

//カメラの揺れ(強さ、秒数)
void CameraShake::StartShake(float intensity, float duration) {
    m_shake_intensity = intensity;
    m_shake_duration = duration;
    m_shake_time = duration;
}

void CameraShake::Update() {
    if (m_shake_time > 0.0f) {
        m_shake_time -= CFPS::GetDeltaTime();
        //シェイクの強さ
        float shake_amount = m_shake_intensity * (m_shake_time / m_shake_duration);
        m_shake_offset = CVector3D(((float)rand() / RAND_MAX - 0.5f) * shake_amount,
            ((float)rand() / RAND_MAX - 0.5f) * shake_amount,
            ((float)rand() / RAND_MAX - 0.5f) * shake_amount);
    }
    else {
        m_shake_offset = CVector3D(0.0f, 0.0f, 0.0f);
    }
}
