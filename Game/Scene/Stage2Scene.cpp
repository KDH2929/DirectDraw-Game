#include "stdafx.h"
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
#include "Stage2Scene.h"

#include <algorithm>
#include <Windows.h>
#include <memory>


Stage2Scene::Stage2Scene(CDDrawDevice* pDrawDevice, HWND hWnd)
    : m_pDrawDevice(pDrawDevice), m_hWnd(hWnd)
{
}

Stage2Scene::~Stage2Scene()
{
    Cleanup();
}

void Stage2Scene::Initialize()
{
    ColliderManager::GetInstance()->Initialize();


    // 화면 크기 가져오기
    int screenWidth = static_cast<int>(m_pDrawDevice->GetWidth());
    int screenHeight = static_cast<int>(m_pDrawDevice->GetHeight());

    /*
    PNG파일을 처리하면 좋긴하겠다만...
    새로 이미지파일처리하는 코드를 짜는 건 추후에 기능추가해도 될듯
    */

    /*
    포토샵에서 이미지를 확대처리 때 안티앨리어싱 기법 선택에 따라 배경 투명처리가 깔끔하게 안될 수도 있으니 유의할 것
    추가로 이렇게 동적할당한 이미지들은 할당 해제 해주지 않으면 메모리 누수
    */

    LOG_MESSAGE(L"Character Image Loading Start");

    // 플레이어 이미지 로드
    CTGAImage* pPlayerImage = new CTGAImage;
    if (pPlayerImage->Load24BitsTGA("./data/Warrior_Sheet-Effect.tga", 4))
    {
        DWORD dwColorKey = pPlayerImage->GetPixel(0, 0);
        m_pPlayerImgData = new CImageData;
        m_pPlayerImgData->Create(pPlayerImage->GetRawImage(),
            pPlayerImage->GetWidth(),
            pPlayerImage->GetHeight(),
            dwColorKey);
    }
    delete pPlayerImage;
    pPlayerImage = nullptr;


    LOG_MESSAGE(L"Character Image Loading Complete");


    LOG_MESSAGE(L"TileMap Image Loading Start");

    // 타일맵 생성 및 로드
    m_TileMap = new TileMap2D(0, 0);
    m_TileMap->ReadTileMap("./data/TileMap/level.tilemap3");
    m_bgWidth = m_TileMap->GetTileMapWidth();
    m_bgHeight = m_TileMap->GetTileMapHeight();

    LOG_MESSAGE(L"TileMap Image Loading Finished");



    LOG_MESSAGE(L"Monster Image Loading Start");

    // 몬스터(centipede) 이미지 로드
    CTGAImage* pCentipedeImage = new CTGAImage;
    if (pCentipedeImage->Load24BitsTGA("./data/Centipede.tga", 4))
    {
        DWORD dwColorKey = pCentipedeImage->GetPixel(0, 0);
        m_pCentipedeImgData = new CImageData;
        m_pCentipedeImgData->Create(pCentipedeImage->GetRawImage(),
            pCentipedeImage->GetWidth(),
            pCentipedeImage->GetHeight(),
            dwColorKey);
    }
    delete pCentipedeImage;
    pCentipedeImage = nullptr;

    LOG_MESSAGE(L"Monster Image Loading Completed");

    LOG_MESSAGE(L"Portal Image Loading Start");

    // 포탈 이미지 로드
    CTGAImage* pPortalImage = new CTGAImage;
    if (pPortalImage->Load24BitsTGA("./data/Portal_sprite.tga", 4))
    {
        DWORD dwColorKey = pPortalImage->GetPixel(0, 0);
        m_pPortalImgData = std::make_unique<CImageData>();
        m_pPortalImgData->Create(pPortalImage->GetRawImage(),
            pPortalImage->GetWidth(),
            pPortalImage->GetHeight(),
            dwColorKey);
    }
    delete pPortalImage;
    pPortalImage = nullptr;

    LOG_MESSAGE(L"Portal Image Loading Completed");



    // 플레이어 초기 위치 설정
    int playerPosX = 50;
    int playerPosY = 800;
    m_pPlayer = new Character(m_pPlayerImgData, static_cast<float>(playerPosX), static_cast<float>(playerPosY));

    // 몬스터 생성
    m_vMonsters.push_back(new Centipede(m_pCentipedeImgData, 700.0f, 800.0f));
    m_vMonsters.push_back(new Centipede(m_pCentipedeImgData, 1200.0f, 800.0f));

    m_pPortal = std::make_unique<Portal>(m_pPortalImgData.get(), 2500, 955);
    m_pPortal->SetNextSceneType(SceneType::GameOver);


    m_pDeathOverlapBox = std::make_unique<DeathOverlapBox>(-5000, 1500, 10000, 20);
    m_pDeathOverlapBox->SetNextSceneType(SceneType::GameOver);

}

void Stage2Scene::Cleanup()
{
    // Portal, Portal 이미지는 unique_ptr로 자동해제

    if (m_pPlayer)
    {
        delete m_pPlayer;
        m_pPlayer = nullptr;
    }


    for (auto monster : m_vMonsters)
    {
        if (monster)
        {
            delete monster;
        }
    }

    m_vMonsters.clear();


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


    if (m_TileMap)
    {
        delete m_TileMap;
        m_TileMap = nullptr;
    }


    if (ColliderManager::GetInstance())
    {
        ColliderManager::GetInstance()->DestroyInstance();
    }

    if (DebugManager::GetInstance())
    {
        DebugManager::GetInstance()->ClearAllDebugData();
    }

}

void Stage2Scene::Update(float deltaTime)
{
    // 화면 크기가 변경될 수 있으므로 여기서 다시 화면크기를 다시 계산해줘야함
    int screenWidth = static_cast<int>(m_pDrawDevice->GetWidth());
    int screenHeight = static_cast<int>(m_pDrawDevice->GetHeight());

    Vector2<float> playerPos = Vector2<float>(0.0f, 0.0f);

    if (m_pPlayer)
    {
        m_pPlayer->Update(deltaTime);
        playerPos = m_pPlayer->GetPosition();
    }

    DebugManager::GetInstance()->AddOnScreenMessage(L"PlayerPos: " + std::to_wstring(static_cast<int>(playerPos.x)) +
        L", " + std::to_wstring(static_cast<int>(playerPos.y)), 0.03f);


    // 모든 몬스터 업데이트
    for (auto monster : m_vMonsters)
    {
        monster->Update(deltaTime);
    }

    if (m_TileMap)
    {
        m_TileMap->Update(deltaTime);
    }

    if (m_pPortal)
    {
        m_pPortal->Update(deltaTime);
    }

    UpdateCamera(screenWidth, screenHeight);        // UpdateCamera 함수에서 카메라 위치를 설정 후, 그에 따른 RenderPosition 설정할 것

    ColliderManager::GetInstance()->ProcessCollisions();
    DebugManager::GetInstance()->Update(deltaTime);
    InputManager::GetInstance().Update(deltaTime);

}

void Stage2Scene::Render()
{
    // DirectDraw Device로 Bitmap 이미지들 Render
    // Lock, UnLock을 통해 BackBuffer에 직접 접근하여 픽셀값들을 하나씩 지정하여 Render
    m_pDrawDevice->BeginDraw();


    if (m_TileMap)
    {
        m_TileMap->Render(m_pDrawDevice);
    }

    if (m_pPortal)
    {
        m_pPortal->Render(m_pDrawDevice);
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
    // GDI(Device Context)를 통한 렌더링으로 winAPI 기능으로 렌더를 하기위함
    HDC hDC = nullptr;

    // BackBuffer에 그리기 위해 BeginGDI 수행
    // BackBuffer에 그려야 화면 깜빡임 문제가 안 생김
    if (m_pDrawDevice->BeginGDI(&hDC))
    {
        m_pDrawDevice->DrawInfo(hDC);
        DebugManager::GetInstance()->Render(hDC);

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

void Stage2Scene::ResetInterpolation()
{
    if (m_pPlayer)
        m_pPlayer->ResetInterpolation();

    if (m_pPortal)
        m_pPortal->ResetInterpolation();

    for (auto monster : m_vMonsters)
    {
        if (monster)
            monster->ResetInterpolation();
    }
}

void Stage2Scene::InterpolatePosition(float alpha)
{
    if (m_pPlayer)
        m_pPlayer->UpdateInterpolation(alpha);

    if (m_pPortal)
        m_pPortal->UpdateInterpolation(alpha);

    for (auto monster : m_vMonsters)
    {
        if (monster)
            monster->UpdateInterpolation(alpha);
    }
}

const SceneType Stage2Scene::GetSceneType()
{
    return SceneType::Stage1;
}


void Stage2Scene::UpdateCamera(int screenWidth, int screenHeight)
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
    DebugManager::GetInstance()->SetCameraOffset(Vector2<float>(-clampedOffsetX, -clampedOffsetY));

    for (auto monster : m_vMonsters)
    {
        // 몬스터의 월드 좌표에 배경 오프셋을 더해 실제 렌더될 좌표 계산
        Vector2<float> monsterWorldPos = monster->GetPosition();
        Vector2<float> monsterRenderPos = monsterWorldPos + Vector2<float>(m_backgroundPosX, m_backgroundPosY);

        monster->SetRenderPosition(monsterRenderPos);
    }


    DebugManager::GetInstance()->AddOnScreenMessage(L"Camera Offset (" + std::to_wstring(static_cast<int>(clampedOffsetX)) +
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

    if (m_pPortal)
    {
        m_pPortal->SetRenderPosition(m_pPortal->GetPosition() + Vector2<float>(m_backgroundPosX, m_backgroundPosY));
    }
}
