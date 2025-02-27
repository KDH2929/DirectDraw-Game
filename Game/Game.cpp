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

    
    // PNG파일을 처리하면 좋긴하겠다만...
    // 새로 이미지파일처리하는 코드를 짜는 건 추후에 기능추가해도 될듯

    // 플레이어 이미지 로드
    // 포토샵에서 이미지를 확대처리 때 안티앨리어싱 기법 선택에 따라 배경 투명처리가 깔끔하게 안될 수도 있으니 유의할 것 
    // 추가로 이렇게 동적할당한 이미지들은 할당 해제 해주지 않으면 메모리 누수
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


    m_TileMap = new TileMap2D(0, 0);
    m_TileMap->ReadTileMap("./data/TileMap/level.tilemap");

    m_bgWidth = m_TileMap->GetTileMapWidth();
    m_bgHeight = m_TileMap->GetTileMapHeight();


    // 플레이어 초기 위치
    int playerPosX = 50;
    int playerPosY = 800;

    // 플레이어 캐릭터 생성
    m_pPlayer = new Character(m_pPlayerImgData, static_cast<float>(playerPosX), static_cast<float>(playerPosY));
    ColliderManager::GetInstance().AddCollider(m_pPlayer->GetCollider());


    // 몬스터 관련
    CTGAImage* pCentipedeImage = new CTGAImage;
    if (pCentipedeImage->Load24BitsTGA("./data/Centipede.tga", 4))
    {
        DWORD dwColorKey = pCentipedeImage->GetPixel(0, 0);
        m_pCentipedeImgData = new CImageData;
        m_pCentipedeImgData->Create(pCentipedeImage->GetRawImage(), pCentipedeImage->GetWidth(), pCentipedeImage->GetHeight(), dwColorKey);
    }
    delete pCentipedeImage;
    pCentipedeImage = nullptr;


    m_vMonsters.push_back(new Centipede(m_pCentipedeImgData, 700.0f, 800.0f));
    
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

    // 60 FPS 기준 게임 로직 업데이트; 프레임 사이의 보간 처리
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


    if (m_pCentipedeImgData)
    {
        delete m_pCentipedeImgData;
        m_pCentipedeImgData = nullptr;
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
    
    ColliderManager::GetInstance().ProcessCollisions();

    DebugManager::GetInstance().Update(m_fTicksPerFrame);
    InputManager::GetInstance().Update(m_fTicksPerFrame);
}


void CGame::UpdatePosition(float deltaTime, int screenWidth, int screenHeight)
{

    m_pPlayer->Update(deltaTime);

    Vector2<float> playerPos = m_pPlayer->GetPosition();


    DebugManager::GetInstance().AddOnScreenMessage(L"PlayerPos: " + std::to_wstring(static_cast<int>(playerPos.x)) +
        L", " + std::to_wstring(static_cast<int>(playerPos.y)), 0.03f);


    // 모든 몬스터 업데이트
    for (auto monster : m_vMonsters)
    {
        monster->Update(deltaTime);
    }
    
    m_TileMap->Update(deltaTime);


    // UpdateCamera 함수에서 카메라 위치를 설정 후, 그에 따른 RenderPosition 설정할 것
    UpdateCamera(screenWidth, screenHeight);

}


void CGame::UpdateCamera(int screenWidth, int screenHeight)
{
    Vector2<float> playerPos = m_pPlayer->GetPosition();

    float screenCenterX = static_cast<float>(screenWidth) / 2;
    float screenCenterY = static_cast<float>(screenHeight) / 2;
    float cameraOffsetX = -200;
    float cameraOffsetY = 20;
    float cameraCenterX = screenCenterX + cameraOffsetX;
    float cameraCenterY = screenCenterY + cameraOffsetY;


    float computedOffsetX = playerPos.x - cameraCenterX;
    float computedOffsetY = playerPos.y - cameraCenterY;

    // 카메라가 배경화면 바깥을 벗어나지 못하도록 clamp 처리
    float clampedOffsetX = std::max<float>(0, std::min<float>(computedOffsetX, static_cast<float>(m_bgWidth) - screenWidth));
    float clampedOffsetY = std::max<float>(0, std::min<float>(computedOffsetY, static_cast<float>(m_bgHeight) - screenHeight));

    m_backgroundPosX = -clampedOffsetX;
    m_backgroundPosY = -clampedOffsetY;

    // 카메라가 맵 끝에 도착하여 더 이상 움직이지 못할 때, 플레이어만 움직이게 처리
    m_playerRenderX = cameraCenterX - (clampedOffsetX - computedOffsetX);
    m_playerRenderY = cameraCenterY - (clampedOffsetY - computedOffsetY);

    m_pPlayer->SetRenderPosition(Vector2<float>{ static_cast<float>(m_playerRenderX), static_cast<float>(m_playerRenderY) });

    m_pPlayer->SetCameraOffset(Vector2<float>(-clampedOffsetX, -clampedOffsetY));
    DebugManager::GetInstance().SetCameraOffset(Vector2<float>(-clampedOffsetX, -clampedOffsetY));

    for (auto monster : m_vMonsters)
    {
        // 몬스터의 월드 좌표에 배경 오프셋을 더해 실제 렌더될 좌표 계산
        Vector2<float> monsterWorldPos = monster->GetPosition();
        Vector2<float> monsterRenderPos = monsterWorldPos + Vector2<float>{ static_cast<float>(m_backgroundPosX), static_cast<float>(m_backgroundPosY) };

        monster->SetRenderPosition(monsterRenderPos);
    }


    DebugManager::GetInstance().AddOnScreenMessage(L"Camera Offset (" + std::to_wstring(static_cast<int>(clampedOffsetX)) +
        L", " + std::to_wstring(static_cast<int>(clampedOffsetY)) + L")", 0.03f);


    // 이후 TileMap의 Render함수에서 CameraOffset을 고려하여 Render 수행
    m_TileMap->SetCameraOffset(clampedOffsetX, clampedOffsetY);


    const std::vector<Tile*>& blockLayer = m_TileMap->GetBlockLayer();
    for (Tile* tile : blockLayer)
    {
        if (tile && tile->GetCollider())
        {
            Vector2<float> colliderPos = tile->GetPosition();
            Vector2<float> newRenderPos = colliderPos + Vector2<float>({ -clampedOffsetX, -clampedOffsetY });

            tile->GetCollider()->SetRenderPosition(newRenderPos);
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
    // DirectDraw Device로 Bitmap 이미지들 Render
    // Lock, UnLock을 통해 BackBuffer에 직접 접근하여 픽셀값들을 하나씩 지정하여 Render
    m_pDrawDevice->BeginDraw();

    
    if (m_TileMap)
    {
        m_TileMap->Render(m_pDrawDevice);
    }


    for (auto monster : m_vMonsters)
    {
        if (monster)
        {
            monster->Render(m_pDrawDevice);
        }
    }


    if (m_pPlayer)
    {
        m_pPlayer->Render(m_pDrawDevice);
    }
    


    m_pDrawDevice->EndDraw();


    // hDC로 디버깅 관련사항들 렌더링
    // GDI(Device Context)를 통한 렌더링으로 winAPI 기능을 통한 렌더를 하기위함
    HDC hDC = nullptr;
    if (m_pDrawDevice->BeginGDI(&hDC))
    {
        m_pDrawDevice->DrawInfo(hDC);
        DebugManager::GetInstance().Render(hDC);

        // 몬스터와 캐릭터는 현재 둘 다 AABB Collider

        if (m_pPlayer)
        {
            float colliderLocalX = m_pPlayer->GetColliderLocalPosition().x;
            float colliderLocalY = m_pPlayer->GetColliderLocalPosition().y;

            m_pPlayer->GetCollider()->Render(hDC, static_cast<int>(m_playerRenderX + colliderLocalX), static_cast<int>(m_playerRenderY + colliderLocalY));
        }

  
        for (auto monster : m_vMonsters)
        {
            if (monster && monster->GetCollider())
            {
                // 몬스터의 화면상 렌더링 위치를 가져옴
                Vector2<float> monsterColliderRenderPos = monster->GetRenderPosition() + monster->GetColliderLocalPosition();
                monster->GetCollider()->Render(hDC, static_cast<int>(monsterColliderRenderPos.x), static_cast<int>(monsterColliderRenderPos.y));
            }
        }

        if (m_TileMap)
        {
            const std::vector<Tile*>& blockLayer = m_TileMap->GetBlockLayer();
            for (Tile* tile : blockLayer)
            {
                if (tile && tile->GetCollider())
                {
                    Vector2<float> tileRenderPos = tile->GetCollider()->GetRenderPosition();
                    tile->GetCollider()->Render(hDC, static_cast<int>(tileRenderPos.x), static_cast<int>(tileRenderPos.y));
                }
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
