#pragma once
#include <windows.h>

class InputManager {
public:
    static InputManager& GetInstance() {
        static InputManager instance;
        return instance;
    }

    void OnKeyDown(UINT nChar);
    void OnKeyUp(UINT nChar);
    bool IsKeyDown(UINT nChar) const;

    // 현재 프레임에서 새롭게 눌린 키를 반환 (이전 프레임에는 눌리지 않았던 경우)
    bool IsKeyPressed(UINT nChar) const;

    void Update();

private:
    InputManager() = default;
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    bool m_keyStates[256] = { false };
    bool m_prevKeyStates[256] = { false };
};
