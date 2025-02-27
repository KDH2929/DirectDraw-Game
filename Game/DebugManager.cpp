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
    OutputDebugStringW(msg.c_str());
    OutputDebugStringW(L"\n");
    std::wcout << msg << std::endl;
}

void DebugManager::SetCameraOffset(Vector2<float> cameraOffset)
{
    m_cameraOffset = cameraOffset;
}

void DebugManager::AddDebugBox(const DebugBox& box)
{
    m_debugBoxes.push_back(box);
}

void DebugManager::AddDebugLine(const DebugLine& line)
{
    m_debugLines.push_back(line);
}

void DebugManager::Update(float deltaTime) {
    // 메시지 업데이트
    for (auto it = m_messages.begin(); it != m_messages.end(); ) {
        it->elapsed += deltaTime;
        if (it->elapsed > it->duration)
            it = m_messages.erase(it);
        else
            ++it;
    }

    // 디버그 박스 업데이트
    for (auto it = m_debugBoxes.begin(); it != m_debugBoxes.end(); ) {
        it->elapsed += deltaTime;
        if (it->elapsed > it->duration)
            it = m_debugBoxes.erase(it);
        else
            ++it;
    }

    // 디버그 선 업데이트
    for (auto it = m_debugLines.begin(); it != m_debugLines.end(); ) {
        it->elapsed += deltaTime;
        if (it->elapsed > it->duration)
            it = m_debugLines.erase(it);
        else
            ++it;
    }
}

void DebugManager::Render(HDC hDC) {
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(0, 0, 0));

    int posY = 10;
    // 메시지 렌더링
    for (const auto& msg : m_messages) {
        RECT rect = { 10, posY, 500, posY + 20 };
        DrawTextW(hDC, msg.text.c_str(), -1, &rect, DT_LEFT | DT_SINGLELINE);
        posY += 25;
    }

    // 디버그 박스 렌더링
    // 박스를 회전시켜 렌더하기 위해서는, 각 박스의 꼭짓점을 계산하고 Polygon으로 그림
    for (const auto& box : m_debugBoxes) {

        Vector2<float> renderBoxCenter = box.center + m_cameraOffset;

        // 박스의 반 크기
        float halfW = box.width * 0.5f;
        float halfH = box.height * 0.5f;
        float rad = box.rotation * PI / 180.0f;
        // 회전된 로컬 좌표 (시계 방향 순서)
        Vector2<float> axisX(std::cos(rad), std::sin(rad));
        Vector2<float> axisY(-std::sin(rad), std::cos(rad));

        // 각 꼭짓점 계산: center ± axisX*halfW ± axisY*halfH
        Vector2<float> corners[4];
        corners[0] = renderBoxCenter + axisX * halfW + axisY * halfH;
        corners[1] = renderBoxCenter - axisX * halfW + axisY * halfH;
        corners[2] = renderBoxCenter - axisX * halfW - axisY * halfH;
        corners[3] = renderBoxCenter + axisX * halfW - axisY * halfH;

        // GDI의 Polyline 함수로 박스의 테두리 그리기
        POINT points[5];
        for (int i = 0; i < 4; ++i) {
            points[i].x = static_cast<LONG>(corners[i].x);
            points[i].y = static_cast<LONG>(corners[i].y);
        }
        points[4] = points[0];  // 닫힌 Polygon을 위해 시작점으로 복사

        // 색상 설정
        HPEN hPen = CreatePen(PS_SOLID, 2, box.color);
        HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

        Polyline(hDC, points, 5);

        SelectObject(hDC, hOldPen);
        DeleteObject(hPen);
    }


    // 디버그 선 렌더링
    for (const auto& line : m_debugLines) {
        // 카메라 오프셋 적용
        Vector2<float> renderStart = line.start + m_cameraOffset;
        Vector2<float> renderEnd = line.end + m_cameraOffset;

        HPEN hPen = CreatePen(PS_SOLID, line.thickness, line.color);
        HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

        MoveToEx(hDC, static_cast<int>(renderStart.x), static_cast<int>(renderStart.y), NULL);
        LineTo(hDC, static_cast<int>(renderEnd.x), static_cast<int>(renderEnd.y));

        SelectObject(hDC, hOldPen);
        DeleteObject(hPen);
    }
}
