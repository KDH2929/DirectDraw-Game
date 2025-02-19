#pragma once
#include <string>
#include <vector>
#include <Windows.h>

struct DebugMessage {
    std::wstring text;  // ����� �ؽ�Ʈ
    float duration;     // ȭ�鿡 ���� ���� �ð� (ms ����)
    float elapsed;      // ����� �ð�
};

class DebugManager {
public:
    static DebugManager& GetInstance() {
        static DebugManager instance;
        return instance;
    }

    void AddMessage(const std::wstring& msg, float duration);   // duration�� s ����
    void Update(float deltaTime);
    void Render(HDC hDC);

private:

    // �̱��� ���� ����
    DebugManager() = default;
    ~DebugManager() = default;
    DebugManager(const DebugManager&) = delete;
    DebugManager& operator=(const DebugManager&) = delete;

    std::vector<DebugMessage> m_messages;
};