#include "stdafx.h"
#include "Portal.h"
#include "AABBCollider.h"
#include "../ImageData/ImageData.h"
#include "../DDrawLib/DDrawDevice.h"
#include "DebugManager.h"
#include "SpriteManager.h"
#include "Character.h"
#include "SceneManager.h"


Portal::Portal(CImageData* pPortalImgData, float startX, float startY)
    : m_spriteManager(nullptr), m_anim(nullptr)
{
    // 초기 위치 설정
    SetPosition(Vector2<float>{ startX, startY });

    // SpriteManager 초기화
    // 스프라이트 시트 전체 크기: 300 x 178, 프레임 수: 4, 각 프레임 크기: 75 x 178
    InitSpriteManager(pPortalImgData, 75, 178);
    InitAnimation();

    InitCollider(startX, startY, 75, 178);
}


Portal::~Portal() {
    if (m_spriteManager) {
        delete m_spriteManager;
        m_spriteManager = nullptr;
    }
    if (m_anim) {
        delete m_anim;
        m_anim = nullptr;
    }
}

void Portal::Update(float deltaTime)
{
    if (m_anim) {
        m_anim->Update(deltaTime);
    }

    GameObject::Update(deltaTime);
}

void Portal::Render(CDDrawDevice* pDevice)
{
    if (pDevice && m_spriteManager && m_anim) {
        // 현재 애니메이션 프레임에 해당하는 srcRect를 SpriteManager에서 가져옴
        int currentFrame = m_anim->GetCurrentFrame();
        const RECT& srcRect = m_spriteManager->GetFrameRect(currentFrame);

        // 화면상의 렌더링 위치
        Vector2<float> renderPos = GetRenderPosition();

        // 스프라이트 렌더링
        pDevice->DrawSprite(static_cast<int>(renderPos.x), static_cast<int>(renderPos.y),
            m_spriteManager->GetSpriteSheet(), srcRect);
    }

    GameObject::Render(pDevice);
}

void Portal::OnCollision(const CollisionInfo& collisionInfo)
{
    switch (collisionInfo.response) {
    case CollisionResponse::Block:
        break;
    case CollisionResponse::Overlap:
        if (collisionInfo.other) {
            if (dynamic_cast<Character*>(collisionInfo.other)) {

                if (!m_isPortalActivated)
                {
                    SceneManager::GetInstance()->AsyncChangeScene(m_nextScene);   
                    m_isPortalActivated = true;
                }
            }
        }
        break;
    default:
        break;
    }
}


void Portal::SetNextSceneType(SceneType Scene)
{
    m_nextScene = Scene;
}

const SceneType Portal::GetNextSceneType()
{
    return m_nextScene;
}


void Portal::InitSpriteManager(CImageData* spriteSheet, int frameWidth, int frameHeight)
{
    if (m_spriteManager)
        delete m_spriteManager;

    m_spriteManager = new SpriteManager;
    m_spriteManager->SetSpriteSheet(spriteSheet, frameWidth, frameHeight);
}


void Portal::InitAnimation()
{
    if (!m_spriteManager) {
        DebugManager::GetInstance()->AddOnScreenMessage(L"SpriteManager is not initialized in Portal::InitAnimation()", 20.0f);
    }

    if (m_anim)
        delete m_anim;

    m_anim = new PortalAnim;

    std::vector<PortalAnim::AnimSequence> sequences;
    const float SECONDS_TO_MS = 1000.0f;
    int currentFrame = 0;

    sequences.push_back({ PortalAnimState::Idle, currentFrame, currentFrame + 3, 0.15f * SECONDS_TO_MS });

    m_anim->SetSequences(sequences);
    m_anim->SetState(PortalAnimState::Idle);
}

void Portal::InitCollider(int x, int y, int width, int height)
{
    if (m_pCollider)
        delete m_pCollider;

    m_pCollider = new AABBCollider(this, x, y, width, height);
    m_pCollider->SetCollisionLayer(OVERLAP_COLLISION_LAYER);
    m_pCollider->SetCollisionResponse(CollisionResponse::Overlap);
    ColliderManager::GetInstance()->AddCollider(GetCollider());
}
