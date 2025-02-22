#pragma once
#include <string>
#include <vector>
#include <Windows.h>

struct DebugMessage {
    std::wstring text;  // 출력할 텍스트
    float duration;     // 화면에 남아 있을 시간 (ms 단위)
    float elapsed;      // 경과한 시간
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

    void Update(float deltaTime);
    void Render(HDC hDC);

private:
    DebugManager();
    ~DebugManager();
    DebugManager(const DebugManager&) = delete;
    DebugManager& operator=(const DebugManager&) = delete;

    std::vector<DebugMessage> m_messages;
};
