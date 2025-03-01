#include "stdafx.h"
#include "UI_Button.h"
#include "InputManager.h"
#include "DebugManager.h"
#include <windowsx.h>

UI_Button::UI_Button(const std::wstring& text, const RECT& rect)
    : m_text(text), m_rect(rect)
{
}

void UI_Button::Update(float deltaTime)
{
    int mouseX = InputManager::GetInstance().GetMouseX();
    int mouseY = InputManager::GetInstance().GetMouseY();
    bool mouseReleased = InputManager::GetInstance().IsMouseButtonReleased(MouseButton::Left);

    // 버튼 영역 안에 마우스 좌표가 있고, 클릭되었으면 콜백 호출
    if (mouseReleased &&
        mouseX >= m_rect.left && mouseX <= m_rect.right &&
        mouseY >= m_rect.top && mouseY <= m_rect.bottom)
    {
        if (m_clickCallback)
        {
            m_clickCallback();
        }
    }
}

void UI_Button::Render(HDC hdc)
{
    // 배경 그리기: 설정된 배경색 사용
    HBRUSH hBrush = CreateSolidBrush(m_bgColor);
    FillRect(hdc, &m_rect, hBrush);
    DeleteObject(hBrush);

    // 테두리 그리기: 설정된 테두리 색상 사용
    HBRUSH hBorderBrush = CreateSolidBrush(m_borderColor);
    FrameRect(hdc, &m_rect, hBorderBrush);
    DeleteObject(hBorderBrush);

    // 텍스트 그리기
    ::SetBkMode(hdc, TRANSPARENT);
    ::SetTextColor(hdc, m_textColor);

    // Arial 폰트로 사용, m_fontSize를 높이로 지정
    HFONT hFont = CreateFont(
        m_fontSize,                // 글자 높이
        0,                         // 글자 폭 (0이면 기본값)
        0,                         // 기울기
        0,                         // 회전 각도
        FW_NORMAL,                 // 굵기
        FALSE,                     // 이탤릭체 여부
        FALSE,                     // 밑줄 여부
        FALSE,                     // 취소선 여부
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        L"Arial"                   // 폰트 이름
    );

    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    // 텍스트를 중앙에 정렬하여 그리기
    DrawText(hdc, m_text.c_str(), -1, &m_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // 기존 폰트 복원 및 생성한 폰트 삭제
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}

void UI_Button::SetClickCallback(ClickCallback callback)
{
    m_clickCallback = callback;
}
