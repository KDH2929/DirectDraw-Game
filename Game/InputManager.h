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
    bool IsKeyPressed(UINT nChar) const;
    // deltaTime�� �и��� ������ ����
    void Update(float deltaTime);

    // �¿�Ű�� ���� double press ���� ��ȯ
    bool IsDoublePressedLeft() const;
    bool IsDoublePressedRight() const;

private:
    InputManager() = default;
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    bool m_keyStates[256] = { false };
    bool m_prevKeyStates[256] = { false };

    // �¿� Ű�� ���� double press Ÿ�̸� (ms ����)
    float m_leftKeyTimer = 0.0f;
    float m_rightKeyTimer = 0.0f;

    
    bool m_leftKeyComboInputActive = false;
    bool m_rightKeyComboInputActive = false;

    bool m_leftKeyDoublePressed = false;
    bool m_rightKeyDoublePressed = false;


    // �����Է� �Ӱ�ġ (ms ����)
    static constexpr float DOUBLE_PRESS_THRESHOLD = 200.0f;   
};
