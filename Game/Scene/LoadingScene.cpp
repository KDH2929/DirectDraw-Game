#include "stdafx.h"
#include "../DDrawLib/DDrawDevice.h"
#include "LoadingScene.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "UI_Text.h" 
#include <windows.h>
#include <string>

// Loading 문구 상태 배열 (4단계)
static const wchar_t* loadingStates[] = { L"Loading", L"Loading.", L"Loading..", L"Loading..." };
static const int numStates = sizeof(loadingStates) / sizeof(loadingStates[0]);

LoadingScene::LoadingScene(CDDrawDevice* pDrawDevice, HWND hWnd)
    : m_pDrawDevice(pDrawDevice)
    , m_hWnd(hWnd)
    , m_pLoadingText(nullptr)
    , m_timer(0.0f)
    , m_currentState(0)
{
}

LoadingScene::~LoadingScene()
{
    Cleanup();
}

void LoadingScene::Initialize()
{
    // 화면 크기를 DirectDraw 장치에서 가져옴
    int screenWidth = static_cast<int>(m_pDrawDevice->GetWidth());
    int screenHeight = static_cast<int>(m_pDrawDevice->GetHeight());

    // Loading 텍스트 영역 계산: 화면 중앙에, 폭은 화면의 1/2, 높이는 화면의 1/6
    int textWidth = screenWidth / 2;
    int textHeight = screenHeight / 6;
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;
    RECT textRect;
    textRect.left = centerX - textWidth / 2;
    textRect.right = centerX + textWidth / 2;
    // 텍스트를 중앙에 위치시키기 위해, 텍스트 영역의 아래쪽을 중앙보다 약간 아래로 배치하고,
    // 위쪽은 자동으로 결정되도록 함 (여기서는 중앙에 위치)
    textRect.top = centerY - textHeight / 2;
    textRect.bottom = textRect.top + textHeight;

    // UI_Text 생성, 초기 문구는 loadingStates[0] ("Loading")
    m_pLoadingText = new UI_Text(loadingStates[m_currentState], textRect);
    // 텍스트 색상: 흰색
    m_pLoadingText->SetTextColor(RGB(255, 255, 255));
    // 배경은 투명 (이미 배경은 검정색)
    m_pLoadingText->SetBkMode(TRANSPARENT);
    // 초기 폰트 크기 설정: 텍스트 영역 높이의 1/2
    m_pLoadingText->SetFontSize(textHeight / 2);
}

void LoadingScene::Cleanup()
{
    if (m_pLoadingText) {
        delete m_pLoadingText;
        m_pLoadingText = nullptr;
    }
}

void LoadingScene::Update(float deltaTime)
{
    // 화면 크기 업데이트 (반응형 UI)
    int screenWidth = static_cast<int>(m_pDrawDevice->GetWidth());
    int screenHeight = static_cast<int>(m_pDrawDevice->GetHeight());
    int textWidth = screenWidth / 2;
    int textHeight = screenHeight / 6;
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;
    RECT textRect;
    textRect.left = centerX - textWidth / 2;
    textRect.right = centerX + textWidth / 2;
    textRect.top = centerY - textHeight / 2;
    textRect.bottom = textRect.top + textHeight;

    if (m_pLoadingText) {
        m_pLoadingText->SetRect(textRect);
        m_pLoadingText->SetFontSize(textHeight / 2);
        m_pLoadingText->Update(deltaTime);
    }

    // 타이머 누적
    m_timer += deltaTime;

    const float SEC_TO_MS = 1000.0f; // 1초 = 1000ms

    if (m_timer >= 1.0f * SEC_TO_MS)
    {
        m_timer -= 1.0f * SEC_TO_MS;
        m_currentState = (m_currentState + 1) % numStates;
        // UI_Text의 텍스트 업데이트
        if (m_pLoadingText)
        {
            m_pLoadingText->SetText(loadingStates[m_currentState]);
        }
    }
}

void LoadingScene::Render()
{
    // DirectDraw의 BackBuffer에 렌더를 안하면 화면 깜빡임 문제 생김    
    HDC hdc = nullptr;
    if (m_pDrawDevice->BeginGDI(&hdc))
    {
        // 배경을 검정색으로 채움
        int screenWidth = static_cast<int>(m_pDrawDevice->GetWidth());
        int screenHeight = static_cast<int>(m_pDrawDevice->GetHeight());
        RECT fullRect = { 0, 0, screenWidth, screenHeight };
        HBRUSH hBackBrush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &fullRect, hBackBrush);
        DeleteObject(hBackBrush);

        if (m_pLoadingText)
            m_pLoadingText->Render(hdc);

        m_pDrawDevice->EndGDI(hdc);
    }
    m_pDrawDevice->OnDraw();
}

void LoadingScene::ResetInterpolation()
{
    return;
}

void LoadingScene::InterpolatePosition(float alpha)
{
    return;
}

const SceneType LoadingScene::GetSceneType()
{
    return SceneType::Loading;
}
