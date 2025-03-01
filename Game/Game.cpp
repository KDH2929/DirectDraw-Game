#include "stdafx.h"
#include "../DDrawLib/DDrawDevice.h"
#include "../Util/QueryPerfCounter.h"
#include "Game.h"
#include "InputManager.h"
#include "SceneManager.h"

#include <Windows.h>


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
    Cleanup();
}

BOOL CGame::Initialize(HWND hWnd)
{
    m_hWnd = hWnd;

    m_pDrawDevice = new CDDrawDevice;
    m_pDrawDevice->InitializeDDraw(hWnd);

    srand(GetTickCount());

    SceneManager::Initialize(m_pDrawDevice, hWnd);

    m_pSceneManager = SceneManager::GetInstance();

    m_pSceneManager->AsyncChangeScene(SceneType::Title);

    LOG_MESSAGE(L"Game Initialization Complete");

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
        m_pSceneManager->ResetInterpolation();
        m_pSceneManager->Update(m_fTicksPerFrame);
        m_prevFrameTick = currentTick;
        m_prevCounter = currentCounter;
    }
    else
    {
        float alpha = elapsedTick / m_fTicksPerFrame;
        m_pSceneManager->InterpolatePosition(alpha);
    }

    m_pSceneManager->Render();
}


void CGame::Cleanup()
{
    if (m_pDrawDevice)
    {
        delete m_pDrawDevice;
        m_pDrawDevice = nullptr;
    }

    if (m_pSceneManager)
    {
        m_pSceneManager->CleanUp();
    }

    if (DebugManager::GetInstance())
    {
        DebugManager::GetInstance()->DestroyInstance();
    }
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

void CGame::OnMouseMove(int x, int y)
{
    InputManager::GetInstance().OnMouseMove(x, y);
}

void CGame::OnMouseDown(MouseButton button)
{
    InputManager::GetInstance().OnMouseDown(button);
}

void CGame::OnMouseUp(MouseButton button)
{
    InputManager::GetInstance().OnMouseUp(button);
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
