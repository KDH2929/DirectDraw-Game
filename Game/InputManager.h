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

    // ���� �����ӿ��� ���Ӱ� ���� Ű�� ��ȯ (���� �����ӿ��� ������ �ʾҴ� ���)
    bool IsKeyPressed(UINT nChar) const;

    void Update();

private:
    InputManager() = default;
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    bool m_keyStates[256] = { false };
    bool m_prevKeyStates[256] = { false };
};
