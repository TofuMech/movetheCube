#ifndef CONTROLS_H
#define CONTROLS_H

#include <Windows.h>
#include <unordered_map>

class Controls {
public:
    Controls();
    ~Controls();

    void KeyDown(WPARAM key);
    void KeyUp(WPARAM key);
    bool IsKeyDown(WPARAM key) const;

private:
    std::unordered_map<WPARAM, bool> m_keyStates;
};

#endif 
