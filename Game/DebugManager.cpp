#include "stdafx.h"
#include "DebugManager.h"
#include <iostream>
#include <fcntl.h>
#include <io.h>

// 생성자: 콘솔 창 할당 및 표준 출력 리다이렉트 (옵션)
DebugManager::DebugManager() {
    //AllocConsole();           // 주석 비활성화시 콘솔창 생성
    FILE* fpOut = nullptr;
    if (freopen_s(&fpOut, "CONOUT$", "w", stdout) == 0) {
        _setmode(_fileno(stdout), _O_U16TEXT);
    }
}

// 소멸자: 콘솔 해제
DebugManager::~DebugManager() {
    FreeConsole();
}

// 화면에 메시지를 표시하는 함수
void DebugManager::AddOnScreenMessage(const std::wstring& msg, float duration) {
    DebugMessage newMsg;
    newMsg.text = msg;
    newMsg.duration = duration * 1000.0f; // 초 -> 밀리초
    newMsg.elapsed = 0.0f;
    m_messages.push_back(newMsg);
}

// 콘솔/디버그 출력에 메시지를 기록하는 함수
void DebugManager::LogMessage(const std::wstring& msg) {
    // OutputDebugString을 이용해 Visual Studio Output 창에 로그를 남김
    OutputDebugStringW(msg.c_str());
    OutputDebugStringW(L"\n");
    // 추가로 콘솔에도 출력
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
