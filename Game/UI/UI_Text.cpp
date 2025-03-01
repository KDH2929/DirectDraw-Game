#include "stdafx.h"
#include "UI_Text.h"
#include <windows.h>

UI_Text::UI_Text(const std::wstring& text, const RECT& rect)
    : m_text(text), m_rect(rect)
{
}

void UI_Text::Update(float deltaTime)
{

}

void UI_Text::Render(HDC hdc)
{
    // 배경 모드 설정 및 텍스트 색상 설정
    // hdc를 인자로 넘겨야 함에 주의
    // Windows API 함수를 호출할 때 전역 범위 연산자(::)를 사용하여 호출 할 것 (동일 이름의 함수가 존재)
    ::SetBkMode(hdc, m_bkMode);
    ::SetTextColor(hdc, m_textColor);

    // 만약 OPAQUE 모드로 텍스트 배경을 표시하고 싶다면, 배경 색상도 설정
    if (m_bkMode == OPAQUE) {
        SetBkColor(hdc, m_textBkColor);
    }

    // 폰트 생성: m_fontSize를 사용하여 폰트 크기를 지정
    HFONT hFont = CreateFont(
        m_fontSize,                // 글자 높이
        0,                         // 글자 폭 (0이면 기본값)
        0,                         // 기울기
        0,                         // 회전 각도
        FW_NORMAL,                 // 굵기
        FALSE,                     // 이탤릭 여부
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

    // 텍스트 렌더링: m_rect 영역에 텍스트를 그리기
    // DT_CENTER (수평 중앙), DT_VCENTER (수직 중앙)
    DrawText(hdc, m_text.c_str(), -1, &m_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // 기존 폰트 복원 및 생성한 폰트 삭제
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}
