#include "stdafx.h"
#include "InputManager.h"

void InputManager::OnKeyDown(UINT nChar) {
    if (nChar < 256)
        m_keyStates[nChar] = true;
}

void InputManager::OnKeyUp(UINT nChar) {
    if (nChar < 256)
        m_keyStates[nChar] = false;
}

bool InputManager::IsKeyDown(UINT nChar) const {
    return (nChar < 256) ? m_keyStates[nChar] : false;
}

bool InputManager::IsKeyPressed(UINT nChar) const {
    if (nChar >= 256)
        return false;

    // 새롭게 눌린지 파악
    return m_keyStates[nChar] && !m_prevKeyStates[nChar];
}

void InputManager::Update() {
    for (int i = 0; i < 256; ++i)
        m_prevKeyStates[i] = m_keyStates[i];
}
