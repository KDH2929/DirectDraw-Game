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
    return nChar < 256 ? m_keyStates[nChar] : false;
}