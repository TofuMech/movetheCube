#include "Character.h"
#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;  

Character::Character() :
    m_position(0.0f, 0.0f, 0.0f),
    m_rotation(0.0f),
    m_speed(0.5f),
    m_rotationSpeed(5.0f)
{
}

Character::~Character() {}

void Character::Update(float deltaTime) {
    float moveDelta = m_speed * deltaTime;
    float rotateDelta = m_rotationSpeed * deltaTime;

    float rotationRadians = XM_PI * m_rotation / 180.0f;  // conversion to radians

    DirectX::XMFLOAT2 forward(sinf(rotationRadians), cosf(rotationRadians));
    DirectX::XMFLOAT2 right(cosf(rotationRadians), -sinf(rotationRadians));

    bool anyKeyPressed = false;

    if (m_controls.IsKeyDown(0x57)) { // w - forward
        m_position.x += forward.x * moveDelta;
        m_position.z += forward.y * moveDelta;
        anyKeyPressed = true;
    }
    if (m_controls.IsKeyDown(0x53)) { // s -  backward
        m_position.x -= forward.x * moveDelta;
        m_position.z -= forward.y * moveDelta;
        anyKeyPressed = true;
    }
    if (m_controls.IsKeyDown(0x41)) { // a - left
        m_position.x -= right.x * moveDelta;
        m_position.z -= right.y * moveDelta;
        anyKeyPressed = true;
    }
    if (m_controls.IsKeyDown(0x44)) { // d - right
        m_position.x += right.x * moveDelta;
        m_position.z += right.y * moveDelta;
        anyKeyPressed = true;
    }
    if (m_controls.IsKeyDown(VK_UP)) { // up arrow 
        m_position.y += moveDelta;
        anyKeyPressed = true;
    }
    if (m_controls.IsKeyDown(VK_DOWN)) { // down arrow 
        m_position.y -= moveDelta;
        anyKeyPressed = true;
    }
    if (m_controls.IsKeyDown(VK_SPACE)) { // space
        m_rotation += rotateDelta;
        if (m_rotation >= 360.0f) {
            m_rotation -= 360.0f;
        }
        anyKeyPressed = true;
    }

    if (anyKeyPressed) {
        wchar_t debug[256];
        swprintf_s(debug, L"Position: %f, %f, %f Rotation: %f\n",
            m_position.x, m_position.y, m_position.z, m_rotation);
        OutputDebugString(debug);
    }
}

void Character::OnKeyDown(WPARAM key) {
    m_controls.KeyDown(key);
}

void Character::OnKeyUp(WPARAM key) {
    m_controls.KeyUp(key);
}