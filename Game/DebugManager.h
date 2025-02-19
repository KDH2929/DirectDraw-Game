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

    void AddMessage(const std::wstring& msg, float duration);   // duration은 s 단위
    void Update(float deltaTime);
    void Render(HDC hDC);

private:

    // 싱글톤 패턴 적용
    DebugManager() = default;
    ~DebugManager() = default;
    DebugManager(const DebugManager&) = delete;
    DebugManager& operator=(const DebugManager&) = delete;

    std::vector<DebugMessage> m_messages;
};