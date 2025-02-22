#include "stdafx.h"
#include "ColliderManager.h"
#include "AABBCollider.h"
#include "GameObject.h"
#include <algorithm>
#include <sstream>
#include <unordered_map>


ColliderManager::ColliderManager() {
}

ColliderManager::~ColliderManager() {
    ClearColliders();
}

void ColliderManager::AddCollider(Collider* collider) {
    m_colliders.push_back(collider);
}

void ColliderManager::RemoveCollider(Collider* collider) {
    auto it = std::find(m_colliders.begin(), m_colliders.end(), collider);
    if (it != m_colliders.end()) {
        m_colliders.erase(it);
    }
}

void ColliderManager::ClearColliders() {
    for (auto collider : m_colliders) {
        delete collider;
    }
    m_colliders.clear();
}

std::vector<std::pair<Collider*, Collider*>> ColliderManager::CheckAllCollisions() {
    std::vector<std::pair<Collider*, Collider*>> collisions;
    size_t count = m_colliders.size();
    for (size_t i = 0; i < count; ++i) {
        for (size_t j = i + 1; j < count; ++j) {
            if (m_colliders[i]->CheckCollision(m_colliders[j])) {
                collisions.push_back({ m_colliders[i], m_colliders[j] });
            }
        }
    }
    return collisions;
}


void ColliderManager::ProcessCollisions() {
    // 모든 충돌 쌍을 가져옴
    auto collisions = CheckAllCollisions();

    // 각 GameObject에 대해 누적된 MTV(최소 이동 벡터)를 저장할 맵
    std::unordered_map<GameObject*, Vector2> mtvMap;

    // 모든 충돌 쌍을 순회하며 처리
    for (auto& pair : collisions) {
        Collider* colliderA = pair.first;
        Collider* colliderB = pair.second;
        GameObject* objA = colliderA->GetOwner();
        GameObject* objB = colliderB->GetOwner();

        if (objA && objB) {
            CollisionResponse responseA = colliderA->GetCollisionResponse();
            CollisionResponse responseB = colliderB->GetCollisionResponse();

            // 둘 중 하나라도 Block이면 Block, 아니면 Overlap, 둘 다 Ignore이면 무시
            CollisionResponse finalResponse = CollisionResponse::Ignore;
            if (responseA == CollisionResponse::Block || responseB == CollisionResponse::Block)
                finalResponse = CollisionResponse::Block;
            else if (responseA == CollisionResponse::Overlap || responseB == CollisionResponse::Overlap)
                finalResponse = CollisionResponse::Overlap;

            if (finalResponse == CollisionResponse::Block) {
                // ComputeAABBMTV()는 두 Collider 간의 최소 이동 벡터(MTV)를 계산하는 함수
                Vector2 mtv = ComputeAABBMTV(colliderA, colliderB);

                // 각 객체의 질량의 역수를 가져옴
                // 질량의 역수를 쓰면 고정된 물체나 질량이 매우 큰(무한한 질량) 물체에 대해 계산할 때 용이
                // 이 경우, 고정된 물체와 질량이 매우 큰 물체의 경우 질량의 역수는 0으로 처리가능
                float invMassA = objA->GetInvMass();
                float invMassB = objB->GetInvMass();
                float invMassSum = invMassA + invMassB;


                //  m1/(m1+m2) 대신 역질량 사용
                // 역질량 합이 0인 경우는 둘 다 정적이거나 충돌 처리를 할 필요 없는 경우
                if (invMassSum > 0.0f) {
                    // 각 객체에 대해 MTV를 질량 비율에 따라 분담하여 보정값 계산
                    Vector2 correctionA = mtv * (invMassA / invMassSum);
                    Vector2 correctionB = mtv * (invMassB / invMassSum);
                    mtvMap[objA] = mtvMap[objA] + correctionA;
                    mtvMap[objB] = mtvMap[objB] - correctionB;
                }
            }
        }
    }

    // 누적된 MTV를 각 GameObject에 한 번에 적용함
    for (auto& entry : mtvMap) {
        GameObject* obj = entry.first;
        Vector2 totalMTV = entry.second;
        obj->SetPosition(obj->GetPosition() + totalMTV);
    }
}




// AABB 충돌에 대한 MTV(최소 이동 벡터)를 계산하는 함수
// 두 Collider 객체가 AABB 타입이라고 가정함
Vector2 ColliderManager::ComputeAABBMTV(Collider* a, Collider* b) {
    // Collider가 AABB 타입인지 확인
    AABBCollider* boxA = dynamic_cast<AABBCollider*>(a);
    AABBCollider* boxB = dynamic_cast<AABBCollider*>(b);
    if (!boxA || !boxB)
        return Vector2{ 0.0f, 0.0f };

    // 각 박스의 좌상단 좌표와 크기
    int aLeft = boxA->GetX();
    int aTop = boxA->GetY();
    int aRight = aLeft + boxA->GetWidth();
    int aBottom = aTop + boxA->GetHeight();

    int bLeft = boxB->GetX();
    int bTop = boxB->GetY();
    int bRight = bLeft + boxB->GetWidth();
    int bBottom = bTop + boxB->GetHeight();

    // 두 박스가 겹치는 영역의 폭과 높이 계산
    float overlapX = std::min<int>(aRight, bRight) - std::max<int>(aLeft, bLeft);
    float overlapY = std::min<int>(aBottom, bBottom) - std::max<int>(aTop, bTop);

    // 겹침이 없다면 (이 경우는 충돌이 감지되지 않았어야 함)
    if (overlapX < 0 || overlapY < 0)
        return Vector2{ 0.0f, 0.0f };

    // 각 박스의 중심 좌표 계산
    float aCenterX = aLeft + boxA->GetWidth() / 2.0f;
    float aCenterY = aTop + boxA->GetHeight() / 2.0f;
    float bCenterX = bLeft + boxB->GetWidth() / 2.0f;
    float bCenterY = bTop + boxB->GetHeight() / 2.0f;

    // 두 중심 간 차이 벡터 계산 (내 객체에서 상대 객체를 빼는 방식)
    Vector2 diff = { aCenterX - bCenterX, aCenterY - bCenterY };

    // 최소 이동 벡터(MTV) 결정: 겹침이 적은 축을 기준으로 이동
    Vector2 mtv = { 0.0f, 0.0f };
    if (overlapX < overlapY) {
        // x축 방향으로 분리
        mtv.x = (diff.x < 0) ? -overlapX : overlapX;
    }
    else {
        // y축 방향으로 분리
        mtv.y = (diff.y < 0) ? -overlapY : overlapY;
    }
    return mtv;
}