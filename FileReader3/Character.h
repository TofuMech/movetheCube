#ifndef CHARACTER_H
#define CHARACTER_H

#include <DirectXMath.h>
#include "Controls.h"

class Character {
public:
    Character();
    ~Character();

    void Update(float deltaTime);
    void OnKeyDown(WPARAM key);
    void OnKeyUp(WPARAM key);
    DirectX::XMFLOAT3 GetPosition() const { return m_position; }
    float GetRotation() const { return m_rotation; }

private:
    DirectX::XMFLOAT3 m_position;
    float m_rotation;
    Controls m_controls;
    float m_speed;
    float m_rotationSpeed;
};

#endif // CHARACTER_H
