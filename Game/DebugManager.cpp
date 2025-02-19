#include "stdafx.h"
#include "DebugManager.h"

void DebugManager::AddMessage(const std::wstring& msg, float duration) {
    DebugMessage newMsg;
    newMsg.text = msg;
    newMsg.duration = duration * 1000.0f; // 초 -> 밀리초
    newMsg.elapsed = 0.0f;
    m_messages.push_back(newMsg);
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