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
#include "ColliderManager.h"
#include "Centipede.h"
#include "TileMap2D.h"

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

    
    // PNG������ ó���ϸ� �����ϰڴٸ�...
    // ���� �̹�������ó���ϴ� �ڵ带 ¥�� �� ���Ŀ� ����߰��ص� �ɵ�

    // �÷��̾� �̹��� �ε�
    // ���伥���� �̹����� Ȯ��ó�� �� ��Ƽ�ٸ���� ��� ���ÿ� ���� ��� ����ó���� ����ϰ� �ȵ� ���� ������ ������ �� 
    CTGAImage* pPlayerImage = new CTGAImage;
    if (pPlayerImage->Load24BitsTGA("./data/Warrior_Sheet-Effect.tga", 4))
    {
        DWORD dwColorKey = pPlayerImage->GetPixel(0, 0);
        m_pPlayerImgData = new CImageData;
        m_pPlayerImgData->Create(pPlayerImage->GetRawImage(), pPlayerImage->GetWidth(), pPlayerImage->GetHeight(), dwColorKey);
    }
    delete pPlayerImage;
    pPlayerImage = nullptr;

    
    m_TileMap = new TileMap2D(0,0);
    m_TileMap->ReadTileMap("./data/level.tilemap");
    

    // �÷��̾� �ʱ� ��ġ
    int playerPosX = 50;
    int playerPosY = 50;

    // �÷��̾� ĳ���� ����
    m_pPlayer = new Character(m_pPlayerImgData, static_cast<float>(playerPosX), static_cast<float>(playerPosY));
    ColliderManager::GetInstance().AddCollider(m_pPlayer->GetCollider());


    // ���� ����
    CTGAImage* pCentipedeImage = new CTGAImage;
    CImageData* pCentipedeImgData = nullptr;
    if (pCentipedeImage->Load24BitsTGA("./data/Centipede.tga", 4))
    {
        DWORD dwColorKey = pCentipedeImage->GetPixel(0, 0);
        pCentipedeImgData = new CImageData;
        pCentipedeImgData->Create(pCentipedeImage->GetRawImage(), pCentipedeImage->GetWidth(), pCentipedeImage->GetHeight(), dwColorKey);
    }
    delete pCentipedeImage;
    pCentipedeImage = nullptr;

    m_vMonsters.push_back(new Centipede(pCentipedeImgData, 100.0f, 400.0f));
    m_vMonsters.push_back(new Centipede(pCentipedeImgData, 300.0f, 400.0f));
    m_vMonsters.push_back(new Centipede(pCentipedeImgData, 500.0f, 400.0f));
    m_vMonsters.push_back(new Centipede(pCentipedeImgData, 700.0f, 400.0f));
    
    for (auto monster : m_vMonsters) {
        ColliderManager::GetInstance().AddCollider(monster->GetCollider());
    }

    return TRUE;
}


void CGame::Process()
{
    LARGE_INTEGER currentCounter = QCGetCounter();
    float elapsedTick = QCMeasureElapsedTick(currentCounter, m_prevCounter);
    ULONGLONG currentTick = GetTickCount64();

    // 60 FPS ���� ���� ���� ������Ʈ; ������ ������ ���� ó��
    if (elapsedTick > m_fTicksPerFrame)
    {
        ResetInterpolation();
        UpdateGameFrame(currentTick);       // Update
        m_prevFrameTick = currentTick;
        m_prevCounter = currentCounter;
    }
    else
    {
        float alpha = elapsedTick / m_fTicksPerFrame;
        InterpolatePosition(alpha);
    }

    DrawScene();        // Render
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

    if (m_pDrawDevice)
    {
        delete m_pDrawDevice;
        m_pDrawDevice = nullptr;
    }

    if (m_TileMap)
    {
        delete m_TileMap;
        m_TileMap = nullptr;
    }
}


void CGame::UpdateGameFrame(ULONGLONG currentTick)
{

    int screenWidth = static_cast<int>(m_pDrawDevice->GetWidth());
    int screenHeight = static_cast<int>(m_pDrawDevice->GetHeight());

    UpdatePosition(m_fTicksPerFrame, screenWidth, screenHeight);
    
    ColliderManager::GetInstance().ProcessCollisions();

    DebugManager::GetInstance().Update(m_fTicksPerFrame);

    InputManager::GetInstance().Update();
}


void CGame::UpdatePosition(float deltaTime, int screenWidth, int screenHeight)
{

    m_pPlayer->Update(deltaTime);

    Vector2 playerPos = m_pPlayer->GetPosition();


    DebugManager::GetInstance().AddOnScreenMessage(L"PlayerPos: " + std::to_wstring(static_cast<int>(playerPos.x)) +
        L", " + std::to_wstring(static_cast<int>(playerPos.y)), 0.03f);


    // ��� ���� ������Ʈ
    for (auto monster : m_vMonsters)
    {
        monster->Update(deltaTime);
    }

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

    
    int bgWidth = static_cast<int>(m_TileMap->GetTileMapWidth());
    int bgHeight = static_cast<int>(m_TileMap->GetTileMapHeight());
    int clampedOffsetX = std::max<int>(0, std::min<int>(computedOffsetX, bgWidth - screenWidth));
    int clampedOffsetY = std::max<int>(0, std::min<int>(computedOffsetY, bgHeight - screenHeight));
    m_TileMap->SetOffset(clampedOffsetX, clampedOffsetY);
    

    // ī�޶� �� ���� �����Ͽ� �� �̻� �������� ���� ��, �÷��̾ �����̰� ó��
    m_playerRenderX = cameraCenterX - (clampedOffsetX - computedOffsetX);
    m_playerRenderY = cameraCenterY - (clampedOffsetY - computedOffsetY);

    m_pPlayer->SetRenderPosition(Vector2{ static_cast<float>(m_playerRenderX),  static_cast<float>(m_playerRenderY) });

    for (auto monster : m_vMonsters)
    {
        // ������ ���� ��ǥ�� ��� �������� ���� ���� ������ ��ǥ ���
        Vector2 monsterWorldPos = monster->GetPosition();
        Vector2 monsterRenderPos = monsterWorldPos + Vector2{ static_cast<float>(m_backgroundPosX), static_cast<float>(m_backgroundPosY) };

        monster->SetRenderPosition(monsterRenderPos);
    }


    DebugManager::GetInstance().AddOnScreenMessage(L"Camera Offset (" + std::to_wstring(clampedOffsetX) +
        L", " + std::to_wstring(clampedOffsetY) + L")", 0.03f);

    for (auto monster : m_vMonsters)
    {
        if (monster)
        {
            Vector2 renderPos = monster->GetRenderPosition();
            Vector2 actualPos = monster->GetPosition();
            std::wstring msg = L"Monster: RenderPos (" + std::to_wstring(static_cast<int>(renderPos.x)) + L", " +
                std::to_wstring(static_cast<int>(renderPos.y)) + L"), ActualPos (" +
                std::to_wstring(static_cast<int>(actualPos.x)) + L", " +
                std::to_wstring(static_cast<int>(actualPos.y)) + L")";
            DebugManager::GetInstance().AddOnScreenMessage(msg, 0.03f);
        }
    }
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
    // DirectDraw Device�� Bitmap �̹����� Render
    m_pDrawDevice->BeginDraw();

    m_pDrawDevice->Clear();
    
    if (m_TileMap)
    {
        m_TileMap->Render(m_pDrawDevice);
    }
    


    if (m_pPlayer)
    {
        m_pPlayer->Render(m_pDrawDevice);
    }


    for (auto monster : m_vMonsters)
    {
        if (monster)
        {
            monster->Render(m_pDrawDevice);
        }
    }
    


    m_pDrawDevice->EndDraw();


    // hDC�� ����� ���û��׵� ������
    HDC hDC = nullptr;
    if (m_pDrawDevice->BeginGDI(&hDC))
    {
        m_pDrawDevice->DrawInfo(hDC);
        DebugManager::GetInstance().Render(hDC);

        m_pPlayer->GetCollider()->Render(hDC, m_playerRenderX, m_playerRenderY);
        
        for (auto monster : m_vMonsters)
        {
            if (monster && monster->GetCollider())
            {
                // ������ ȭ��� ������ ��ġ�� ������
                Vector2 monsterRenderPos = monster->GetRenderPosition();
                monster->GetCollider()->Render(hDC, static_cast<int>(monsterRenderPos.x), static_cast<int>(monsterRenderPos.y));
            }
        }

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
