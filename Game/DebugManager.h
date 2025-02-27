#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include "MathUtils.h"
#include <cmath>

// 기존 DebugMessage 구조체
struct DebugMessage {
    std::wstring text;  // 출력할 텍스트
    float duration;     // 화면에 남아 있을 시간 (ms 단위)
    float elapsed;      // 경과한 시간
};


struct DebugBox {
    // OBB 형태의 박스 정보를 위한 필드
    Vector2<float> center;        // 박스 중심
    float width;                  // 전체 폭
    float height;                 // 전체 높이
    float rotation;               // 회전각 (도 단위)
    COLORREF color;               // 박스 선 색상

    float duration;               // 화면에 남을 시간 (duration 은 초 단위, 내부적으로 ms로 변환해서 사용)
    float elapsed;                // 경과 시간


    // 매개변수 생성자
    DebugBox(const Vector2<float>& _center, float _width, float _height, float _rotation,
        COLORREF _color = RGB(255, 0, 0), float _duration = 1.0f)
        : center(_center), width(_width), height(_height), rotation(_rotation),
        color(_color), duration(_duration * 1000.0f), elapsed(0.0f) 
    {}
};


struct DebugLine {
    Vector2<float> start;         // 시작점
    Vector2<float> end;           // 끝점
    COLORREF color;               // 선 색상
    int thickness;                // 선 두께
    float duration;               // 화면에 남을 시간 (초 단위, 내부적으로 ms로 변환)
    float elapsed;                // 경과한 시간

    DebugLine(const Vector2<float>& _start, const Vector2<float>& _end,
        COLORREF _color = RGB(0, 0, 0), int _thickness = 1, float _duration = 1.0f)
        : start(_start), end(_end), color(_color), thickness(_thickness),
        duration(_duration * 1000.0f), elapsed(0.0f)
    {}
};



class DebugManager {
public:
    static DebugManager& GetInstance() {
        static DebugManager instance;
        return instance;
    }

    // 화면에 메시지를 표시하는 함수 (duration은 초 단위)
    void AddOnScreenMessage(const std::wstring& msg, float duration);
    // 콘솔/디버그 출력에 메시지를 기록하는 함수
    void LogMessage(const std::wstring& msg);

    void SetCameraOffset(Vector2<float> cameraOffset);
    void AddDebugBox(const DebugBox& box);
    void AddDebugLine(const DebugLine& line);

    void Update(float deltaTime);
    void Render(HDC hDC);

private:
    DebugManager();
    ~DebugManager();
    DebugManager(const DebugManager&) = delete;
    DebugManager& operator=(const DebugManager&) = delete;

    std::vector<DebugMessage> m_messages;
    std::vector<DebugBox> m_debugBoxes;
    std::vector<DebugLine> m_debugLines;

    Vector2<float> m_cameraOffset = Vector2<float>(0.0f, 0.0f);
};

