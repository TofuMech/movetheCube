#include "Controls.h"
#include <Windows.h>
Controls::Controls() {}

Controls::~Controls() {}

void Controls::KeyDown(WPARAM key) {
    m_keyStates[key] = true;
    char debug[256];
    sprintf_s(debug, "Key pressed: %d\n", key);
}

void Controls::KeyUp(WPARAM key) {
    m_keyStates[key] = false;
}

bool Controls::IsKeyDown(WPARAM key) const {
    auto it = m_keyStates.find(key);
    if (it != m_keyStates.end()) {
        return it->second;
    }
    return false;
}
