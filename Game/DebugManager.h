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

    // ȭ�鿡 �޽����� ǥ���ϴ� �Լ� (duration�� �� ����)
    void AddOnScreenMessage(const std::wstring& msg, float duration);
    // �ܼ�/����� ��¿� �޽����� ����ϴ� �Լ�
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
