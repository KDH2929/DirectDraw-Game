#include "stdafx.h"
#include <algorithm>
#include <Windows.h>
#include <stdio.h>
#include "../Common/typedef.h"
#include "../Util/Util.h"
#include "../Util/TGAImage.h"
#include "../DDrawLib/DDrawDevice.h"
#include "../Util/QueryPerfCounter.h"
#include "../ImageData/ImageData.h"
#include "GameObject.h"
#include "Character.h"
#include "Game.h"
#include "MathUtils.h"
#include "AABBCollider.h"
#include "InputManager.h"

CGame* g_pGame = nullptr;

CGame::CGame()
{
    m_dwCurrentFPS = 0;
    m_fTicksPerFrame = 1000.0f / static_cast<float>(m_dwGameFPS);
    m_prevFrameTick = GetTickCount64();
    m_prevCounter = QCGetCounter();
}

CGame::~CGame()
{
    delete m_pPlayer;
    Cleanup();
}

BOOL CGame::Initialize(HWND hWnd)
{
    m_hWnd = hWnd;

    m_pDrawDevice = new CDDrawDevice;
    m_pDrawDevice->InitializeDDraw(hWnd);

    int screenWidth = static_cast<int>(m_pDrawDevice->GetWidth());
    int screenHeight = static_cast<int>(m_pDrawDevice->GetHeight());

    srand(GetTickCount());

    
    // PNG파일을 처리하면 좋긴하겠다만...
    // 새로 이미지파일처리하는 코드를 짜는 건 추후에 기능추가해도 될듯

    // 플레이어 이미지 로드
    // 포토샵에서 이미지를 확대처리 때 안티앨리어싱 기법 선택에 따라 배경 투명처리가 깔끔하게 안될 수도 있으니 유의할 것 
    CTGAImage* pPlayerImage = new CTGAImage;
    if (pPlayerImage->Load24BitsTGA("./data/Warrior_Sheet-Effect.tga", 4))
    {
        DWORD dwColorKey = pPlayerImage->GetPixel(0, 0);
        m_pPlayerImgData = new CImageData;
        m_pPlayerImgData->Create(pPlayerImage->GetRawImage(), pPlayerImage->GetWidth(), pPlayerImage->GetHeight(), dwColorKey);
    }
    delete pPlayerImage;
    pPlayerImage = nullptr;

    // 배경 이미지 로드
    m_pBackgroundImage = new CTGAImage;
    m_pBackgroundImage->Load24BitsTGA("./data/Background_01.tga", 4);


    // 플레이어 초기 위치 (화면 중앙)
    int playerPosX = (screenWidth - static_cast<int>(m_pPlayerImgData->GetWidth())) / 2;
    int playerPosY = (screenHeight - static_cast<int>(m_pPlayerImgData->GetHeight())) / 2;

    // 플레이어 캐릭터 생성
    m_pPlayer = new Character(m_pPlayerImgData, static_cast<float>(playerPosX), static_cast<float>(playerPosY));


    return TRUE;
}

void CGame::Process()
{
    LARGE_INTEGER currentCounter = QCGetCounter();
    float elapsedTick = QCMeasureElapsedTick(currentCounter, m_prevCounter);
    ULONGLONG currentTick = GetTickCount64();

    // 60 FPS 기준 게임 로직 업데이트; 프레임 사이의 보간 처리
    if (elapsedTick > m_fTicksPerFrame)
    {
        ResetInterpolation();
        UpdateGameFrame(currentTick);
        m_prevFrameTick = currentTick;
        m_prevCounter = currentCounter;
    }
    else
    {
        float alpha = elapsedTick / m_fTicksPerFrame;
        InterpolatePosition(alpha);
    }

    DrawScene();
}

void CGame::Cleanup()
{
    if (m_pPlayer)
    {
        delete m_pPlayer;
        m_pPlayer = nullptr;
    }

    if (m_pPlayerImgData)
    {
        delete m_pPlayerImgData;
        m_pPlayerImgData = nullptr;
    }

    if (m_pBackgroundImage)
    {
        delete m_pBackgroundImage;
        m_pBackgroundImage = nullptr;
    }

    if (m_pDrawDevice)
    {
        delete m_pDrawDevice;
        m_pDrawDevice = nullptr;
    }
}

void CGame::UpdateGameFrame(ULONGLONG currentTick)
{
    int screenWidth = static_cast<int>(m_pDrawDevice->GetWidth());
    int screenHeight = static_cast<int>(m_pDrawDevice->GetHeight());

    UpdatePosition(m_fTicksPerFrame, screenWidth, screenHeight);
    DebugManager::GetInstance().Update(m_fTicksPerFrame);
}

void CGame::UpdatePosition(float deltaTime, int screenWidth, int screenHeight)
{
 
    int bgWidth = static_cast<int>(m_pBackgroundImage->GetWidth());
    int bgHeight = static_cast<int>(m_pBackgroundImage->GetHeight());

    int playerWidth = 0;
    int playerHeight = 0;

    AABBCollider* pPlayerCollider = static_cast<AABBCollider*>(m_pPlayer->GetCollider());

    if (pPlayerCollider)
    {
        playerWidth = pPlayerCollider->GetWidth();
        playerHeight = pPlayerCollider->GetHeight();
    }

    // 월드 경계 설정: 캐릭터가 스프라이트 전체가 화면 내에 남도록 하기 위한 용도 (화면 바깥을 벗어나지 않도록)
    m_pPlayer->SetWorldBounds(bgWidth - playerWidth, bgHeight - playerHeight);
    m_pPlayer->Update(deltaTime);

    Vector2 playerPos = m_pPlayer->GetPosition();

    DebugManager::GetInstance().AddMessage(L"PlayerPos: " + std::to_wstring(static_cast<int>(playerPos.x)) +
        L", " + std::to_wstring(static_cast<int>(playerPos.y)), 0.03f);


    UpdateCamera(screenWidth, screenHeight);
}

void CGame::UpdateCamera(int screenWidth, int screenHeight)
{
    Vector2 playerPos = m_pPlayer->GetPosition();

    int screenCenterX = screenWidth / 2;
    int screenCenterY = screenHeight / 2;
    int cameraOffsetX = -200;
    int cameraOffsetY = 20;
    int cameraCenterX = screenCenterX + cameraOffsetX;
    int cameraCenterY = screenCenterY + cameraOffsetY;

    int computedOffsetX = playerPos.x - cameraCenterX;
    int computedOffsetY = playerPos.y - cameraCenterY;

    int bgWidth = static_cast<int>(m_pBackgroundImage->GetWidth());
    int bgHeight = static_cast<int>(m_pBackgroundImage->GetHeight());
    int clampedOffsetX = std::max<int>(0, std::min<int>(computedOffsetX, bgWidth - screenWidth));
    int clampedOffsetY = std::max<int>(0, std::min<int>(computedOffsetY, bgHeight - screenHeight));

    m_backgroundPosX = -clampedOffsetX;
    m_backgroundPosY = -clampedOffsetY;

    m_playerRenderX = cameraCenterX - (clampedOffsetX - computedOffsetX);
    m_playerRenderY = cameraCenterY - (clampedOffsetY - computedOffsetY);

    m_pPlayer->SetRenderPosition(Vector2{ static_cast<float>(m_playerRenderX),  static_cast<float>(m_playerRenderY) });

    DebugManager::GetInstance().AddMessage(L"Camera: Offset (" + std::to_wstring(clampedOffsetX) +
        L", " + std::to_wstring(clampedOffsetY) + L")", 0.03f);
}

void CGame::InterpolatePosition(float alpha)
{
    if (m_pPlayer)
        m_pPlayer->UpdateInterpolation(alpha);
}

void CGame::ResetInterpolation()
{
    if (m_pPlayer)
        m_pPlayer->ResetInterpolation();
}

void CGame::DrawScene()
{
    // DirectDraw Device로 Bitmap 이미지들 Render
    m_pDrawDevice->BeginDraw();

    if (m_pBackgroundImage)
    {
        m_pDrawDevice->DrawBitmap(m_backgroundPosX, m_backgroundPosY,
            m_pBackgroundImage->GetWidth(),
            m_pBackgroundImage->GetHeight(),
            m_pBackgroundImage->GetRawImage());
    }
    else {
        m_pDrawDevice->Clear();
    }


    if (m_pPlayer)
    {
        m_pPlayer->Render(m_pDrawDevice);
    }


    m_pDrawDevice->EndDraw();


    // hDC로 디버깅 관련사항들 렌더링
    HDC hDC = nullptr;
    if (m_pDrawDevice->BeginGDI(&hDC))
    {
        m_pDrawDevice->DrawInfo(hDC);
        DebugManager::GetInstance().Render(hDC);
        m_pPlayer->GetCollider()->Render(hDC, m_playerRenderX, m_playerRenderY);
        m_pDrawDevice->EndGDI(hDC);
    }

    m_pDrawDevice->OnDraw();
    m_pDrawDevice->CheckFPS();
}


void CGame::OnKeyDown(UINT nChar, UINT uiScanCode)
{
    InputManager::GetInstance().OnKeyDown(nChar);
}

void CGame::OnKeyUp(UINT nChar, UINT uiScanCode)
{
    InputManager::GetInstance().OnKeyUp(nChar);
}

BOOL CGame::OnSysKeyDown(UINT nChar, UINT uiScanCode, BOOL bAltKeyDown)
{
    if (nChar == VK_F9 && bAltKeyDown)
    {
        CaptureBackBuffer("backbuffer.tga");
        return TRUE;
    }
    return FALSE;
}

BOOL CGame::CaptureBackBuffer(const char* szFileName)
{
    FILE* fp = nullptr;
    fopen_s(&fp, szFileName, "wb");
    if (!fp)
        return FALSE;
    if (m_pDrawDevice)
        m_pDrawDevice->CaptureBackBuffer(fp);
    fclose(fp);
    return TRUE;
}

void CGame::OnUpdateWindowSize()
{
    if (m_pDrawDevice)
        m_pDrawDevice->OnUpdateWindowSize();
}

void CGame::OnUpdateWindowPos()
{
    if (m_pDrawDevice)
        m_pDrawDevice->OnUpdateWindowPos();
}
