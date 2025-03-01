#include "stdafx.h"
#include "DeathOverlapBox.h"
#include "AABBCollider.h"
#include "../DDrawLib/DDrawDevice.h"
#include "DebugManager.h"
#include "Character.h"
#include "SceneManager.h"


DeathOverlapBox::DeathOverlapBox(float startX, float startY, float width, float height) {
    // 초기 위치 설정
    SetPosition(Vector2<float>{ startX, startY });
    InitCollider(startX, startY, width, height);
}


DeathOverlapBox::~DeathOverlapBox() {
    // Collider는 GameObject에서 할당해제
}

void DeathOverlapBox::Update(float deltaTime)
{
    GameObject::Update(deltaTime);
}

void DeathOverlapBox::Render(CDDrawDevice* pDevice)
{
    GameObject::Render(pDevice);
}

void DeathOverlapBox::OnCollision(const CollisionInfo& collisionInfo)
{
    switch (collisionInfo.response) {
    case CollisionResponse::Block:
        break;
    case CollisionResponse::Overlap:
        if (collisionInfo.other) {
            if (dynamic_cast<Character*>(collisionInfo.other)) {

                if (!m_isDeathOverlapBoxActivated)
                {
                    SceneManager::GetInstance()->AsyncChangeScene(m_nextScene);
                    m_isDeathOverlapBoxActivated = true;
                }
            }
        }
        break;
    default:
        break;
    }
}


void DeathOverlapBox::SetNextSceneType(SceneType Scene)
{
    m_nextScene = Scene;
}

const SceneType DeathOverlapBox::GetNextSceneType()
{
    return m_nextScene;
}


void DeathOverlapBox::InitCollider(int x, int y, int width, int height)
{
    if (m_pCollider)
        delete m_pCollider;

    m_pCollider = new AABBCollider(this, x, y, width, height);
    m_pCollider->SetCollisionLayer(OVERLAP_COLLISION_LAYER);
    m_pCollider->SetCollisionResponse(CollisionResponse::Overlap);
    ColliderManager::GetInstance()->AddCollider(GetCollider());
}
