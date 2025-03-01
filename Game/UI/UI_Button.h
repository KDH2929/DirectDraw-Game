#pragma once
#include "UI.h"
#include <string>
#include <functional>
#include <windows.h>


// 배경색, 테두리 색상, 텍스트 색상, 폰트 크기를 설정가능
class UI_Button : public UI {
public:
    // 클릭 시 실행할 콜백 함수
    using ClickCallback = std::function<void()>;

    UI_Button(const std::wstring& text, const RECT& rect);
    virtual ~UI_Button() = default;

    virtual bool Initialize() override { return true; }
    virtual void Update(float deltaTime) override;
    virtual void Render(HDC hdc) override;

    // 클릭 시 콜백 함수 설정
    void SetClickCallback(ClickCallback callback);

    // 버튼 영역 업데이트
    void SetRect(const RECT& rect) { m_rect = rect; }
    const RECT& GetRect() const { return m_rect; }

    // 추가 속성 설정 함수들
    void SetBackgroundColor(COLORREF color) { m_bgColor = color; }
    void SetBorderColor(COLORREF color) { m_borderColor = color; }
    void SetTextColor(COLORREF color) { m_textColor = color; }
    void SetFontSize(int size) { m_fontSize = size; }

private:
    std::wstring m_text;
    RECT m_rect;
    ClickCallback m_clickCallback;

    // 기본 색상 및 폰트 크기 (초기값 설정)
    COLORREF m_bgColor = RGB(180, 180, 180);
    COLORREF m_borderColor = RGB(0, 0, 0);
    COLORREF m_textColor = RGB(0, 0, 0);
    int m_fontSize = 14;
};
