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
    // deltaTime은 밀리초 단위로 전달
    void Update(float deltaTime);

    // 좌우키에 대해 double press 여부 반환
    bool IsDoublePressedLeft() const;
    bool IsDoublePressedRight() const;

private:
    InputManager() = default;
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    bool m_keyStates[256] = { false };
    bool m_prevKeyStates[256] = { false };

    // 좌우 키에 대한 double press 타이머 (ms 단위)
    float m_leftKeyTimer = 0.0f;
    float m_rightKeyTimer = 0.0f;

    
    bool m_leftKeyComboInputActive = false;
    bool m_rightKeyComboInputActive = false;

    bool m_leftKeyDoublePressed = false;
    bool m_rightKeyDoublePressed = false;


    // 연속입력 임계치 (ms 단위)
    static constexpr float DOUBLE_PRESS_THRESHOLD = 200.0f;   
};
