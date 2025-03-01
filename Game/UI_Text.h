#pragma once

#include "UI.h"
#include <string>
#include <windows.h>

class UI_Text : public UI {
public:
    // Rect를 통해 폰트 위치조절
    // Rect 안에 폰트를 그리는 방식
    UI_Text(const std::wstring& text, const RECT& rect);
    virtual ~UI_Text() = default;

    virtual bool Initialize() override { return true; }
    virtual void Update(float deltaTime) override;
    virtual void Render(HDC hdc) override;

    // 텍스트 변경
    void SetText(const std::wstring& text) { m_text = text; }
    const std::wstring& GetText() const { return m_text; }

    // 위치/크기 변경
    void SetRect(const RECT& rect) { m_rect = rect; }
    const RECT& GetRect() const { return m_rect; }

    // 텍스트 색상 변경 (기본값은 검정색)
    void SetTextColor(COLORREF color) { m_textColor = color; }
    COLORREF GetTextColor() const { return m_textColor; }

    // 텍스트 배경 색상 변경 (기본값은 투명 처리; 만약 배경을 사용하려면 OPAQUE 모드로 설정)
    void SetTextBackgroundColor(COLORREF color) { m_textBkColor = color; }
    COLORREF GetTextBackgroundColor() const { return m_textBkColor; }

    // 배경 모드 설정 (기본값은 TRANSPARENT)
    void SetBkMode(int mode) { m_bkMode = mode; }
    int GetBkMode() const { return m_bkMode; }

    // 폰트 크기 설정 (기본값은 14)
    void SetFontSize(int fontSize) { m_fontSize = fontSize; }
    int GetFontSize() const { return m_fontSize; }

private:
    std::wstring m_text;
    RECT m_rect;
    COLORREF m_textColor = RGB(0, 0, 0); // 기본 텍스트 색상: 검정
    COLORREF m_textBkColor = RGB(0, 0, 0); // 기본 텍스트 배경 색상 (실제로 사용하려면 OPAQUE 모드와 함께 사용)
    int m_bkMode = TRANSPARENT;          // 기본 배경 모드: 투명
    int m_fontSize = 14;                 // 기본 폰트 크기
};
