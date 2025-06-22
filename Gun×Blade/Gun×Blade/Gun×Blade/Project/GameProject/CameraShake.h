#pragma once

class CameraShake {
private:
    float m_shake_intensity;
    float m_shake_duration;
    float m_shake_time;
    CVector3D m_shake_offset;

public:
    CameraShake();
    void StartShake(float intensity, float duration);
    void Update();
    CVector3D GetShakeOffset() {
        return m_shake_offset;
    }

};


