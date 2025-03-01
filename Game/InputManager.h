#pragma once
#include <windows.h>

enum class MouseButton {
    Left = 0,
    Right = 1,
    Middle = 2
};

class InputManager {
public:
    static InputManager& GetInstance() {
        static InputManager instance;
        return instance;
    }

    // 키입력 관련
    void OnKeyDown(UINT nChar);
    void OnKeyUp(UINT nChar);
    bool IsKeyDown(UINT nChar) const;
    bool IsKeyPressed(UINT nChar) const;

    // 좌우키에 대해 double press 여부 반환
    bool IsDoublePressedLeft() const;
    bool IsDoublePressedRight() const;

    // 마우스 입력 처리
    void OnMouseMove(int x, int y);
    void OnMouseDown(MouseButton button);   // button: 0-좌클릭, 1-우클릭, 2-휠
    void OnMouseUp(MouseButton button);


    // 마우스 상태 확인 메서드
    int GetMouseX() const;
    int GetMouseY() const;
    bool IsMouseButtonDown(MouseButton button) const;
    bool IsMouseButtonUp(MouseButton button) const;
    bool IsMouseButtonReleased(MouseButton button) const;       // "눌렀다 떼었는지" 확인하는 함수


    void Update(float deltaTime);

private:
    InputManager() = default;
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    // 키 입력 상태
    bool m_keyStates[256] = { false };
    bool m_prevKeyStates[256] = { false };

    // 마우스 입력 상태
    int m_mouseX = 0;
    int m_mouseY = 0;
    bool m_mouseButtonStates[3] = { false, false, false };
    bool m_prevMouseButtonStates[3] = { false, false, false };


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
