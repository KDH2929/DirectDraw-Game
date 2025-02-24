#include "stdafx.h"
#include "DebugManager.h"
#include <iostream>
#include <fcntl.h>
#include <io.h>

// ������: �ܼ� â �Ҵ� �� ǥ�� ��� �����̷�Ʈ (�ɼ�)
DebugManager::DebugManager() {
    //AllocConsole();           // �ּ� ��Ȱ��ȭ�� �ܼ�â ����
    FILE* fpOut = nullptr;
    if (freopen_s(&fpOut, "CONOUT$", "w", stdout) == 0) {
        _setmode(_fileno(stdout), _O_U16TEXT);
    }
}

// �Ҹ���: �ܼ� ����
DebugManager::~DebugManager() {
    FreeConsole();
}

// ȭ�鿡 �޽����� ǥ���ϴ� �Լ�
void DebugManager::AddOnScreenMessage(const std::wstring& msg, float duration) {
    DebugMessage newMsg;
    newMsg.text = msg;
    newMsg.duration = duration * 1000.0f; // �� -> �и���
    newMsg.elapsed = 0.0f;
    m_messages.push_back(newMsg);
}

// �ܼ�/����� ��¿� �޽����� ����ϴ� �Լ�
void DebugManager::LogMessage(const std::wstring& msg) {
    // OutputDebugString�� �̿��� Visual Studio Output â�� �α׸� ����
    OutputDebugStringW(msg.c_str());
    OutputDebugStringW(L"\n");
    // �߰��� �ֿܼ��� ���
    std::wcout << msg << std::endl;
}

void DebugManager::Update(float deltaTime) {
    for (auto it = m_messages.begin(); it != m_messages.end(); ) {
        it->elapsed += deltaTime;
        if (it->elapsed > it->duration)
            it = m_messages.erase(it);
        else
            ++it;
    }
}

void DebugManager::Render(HDC hDC) {
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(0, 0, 0));

    int posY = 10;
    for (const auto& msg : m_messages) {
        RECT rect = { 10, posY, 500, posY + 20 };
        DrawTextW(hDC, msg.text.c_str(), -1, &rect, DT_LEFT | DT_SINGLELINE);
        posY += 25;
    }
}
