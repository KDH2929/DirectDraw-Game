#include "stdafx.h"
#include "ColliderManager.h"
#include "AABBCollider.h"
#include "GameObject.h"
#include <algorithm>
#include <unordered_map>

ColliderManager* ColliderManager::s_instance = nullptr;

ColliderManager* ColliderManager::GetInstance() {
    if (!s_instance)
        s_instance = new ColliderManager();
    return s_instance;
}

void ColliderManager::Initialize() {
    DestroyInstance();
    s_instance = new ColliderManager();
}

void ColliderManager::DestroyInstance() {
    delete s_instance;
    s_instance = nullptr;
}

ColliderManager::ColliderManager() {}
ColliderManager::~ColliderManager() {
    ClearColliders();
}

void ColliderManager::AddCollider(Collider* collider) {
    m_colliders.push_back(collider);
}

void ColliderManager::RemoveCollider(Collider* collider) {
    auto it = std::find(m_colliders.begin(), m_colliders.end(), collider);
    if (it != m_colliders.end())
        m_colliders.erase(it);
}

void ColliderManager::ClearColliders() {
    m_colliders.clear(); // Collider의 메모리 해제는 외부에서 관리
}

// 완전탐색기반.  추후 공간분할 자료구조를 사용하여 최적화필요..
std::vector<std::pair<Collider*, Collider*>> ColliderManager::CheckAllCollisions() {
    std::vector<std::pair<Collider*, Collider*>> collisions;
    for (size_t i = 0; i < m_colliders.size(); ++i) {
        for (size_t j = i + 1; j < m_colliders.size(); ++j) {
            if (m_colliders[i]->CheckCollision(m_colliders[j])) {
                collisions.push_back({ m_colliders[i], m_colliders[j] });
            }
        }
    }
    return collisions;
}

void ColliderManager::ProcessCollisions() {
    auto collisions = CheckAllCollisions();

    for (auto& pair : collisions) {
        Collider* colliderA = pair.first;
        Collider* colliderB = pair.second;
        GameObject* objA = colliderA->GetOwner();
        GameObject* objB = colliderB->GetOwner();
        if (!objA || !objB) continue;

        // 충돌 반응 결정
        CollisionResponse responseA = colliderA->GetCollisionResponse();
        CollisionResponse responseB = colliderB->GetCollisionResponse();
        if (responseA == CollisionResponse::Ignore || responseB == CollisionResponse::Ignore)
            continue;

        // MTV 계산
        Vector2<float> mtv = ComputeAABBMTV(colliderA, colliderB);

        // 충돌 이벤트 전달
        Vector2<float> normal = mtv.Length() > 0 ? mtv.Normalized() : Vector2<float>{ 0,0 };
        Vector2<float> collisionPoint = (objA->GetPosition() + objB->GetPosition()) * 0.5f;
        float penetration = mtv.Length();

        objA->OnCollision({ objB, responseA, normal, collisionPoint, penetration });
        objB->OnCollision({ objA, responseB, -normal, collisionPoint, penetration });

        // Block일 경우 단순 MTV 보정
        if (responseA == CollisionResponse::Block && responseB == CollisionResponse::Block) {
            if (objA->GetPhysicsType() == PhysicsType::Dynamic &&
                objB->GetPhysicsType() == PhysicsType::Dynamic) {
                objA->SetPosition(objA->GetPosition() + mtv * 0.5f);
                objB->SetPosition(objB->GetPosition() - mtv * 0.5f);
            }
            else if (objA->GetPhysicsType() == PhysicsType::Dynamic) {
                objA->SetPosition(objA->GetPosition() + mtv);
            }
            else if (objB->GetPhysicsType() == PhysicsType::Dynamic) {
                objB->SetPosition(objB->GetPosition() - mtv);
            }
        }
    }
}

// AABB 충돌 MTV 계산
Vector2<float> ColliderManager::ComputeAABBMTV(Collider* a, Collider* b) {
    AABBCollider* boxA = dynamic_cast<AABBCollider*>(a);
    AABBCollider* boxB = dynamic_cast<AABBCollider*>(b);
    if (!boxA || !boxB)
        return { 0.0f, 0.0f };

    float aLeft = boxA->GetX();
    float aTop = boxA->GetY();
    float aRight = aLeft + boxA->GetWidth();
    float aBottom = aTop + boxA->GetHeight();

    float bLeft = boxB->GetX();
    float bTop = boxB->GetY();
    float bRight = bLeft + boxB->GetWidth();
    float bBottom = bTop + boxB->GetHeight();

    float overlapX = std::min<float>(aRight, bRight) - std::max<float>(aLeft, bLeft);
    float overlapY = std::min<float>(aBottom, bBottom) - std::max<float>(aTop, bTop);

    if (overlapX <= 0 || overlapY <= 0)
        return { 0.0f, 0.0f };

    Vector2<float> diff = { (aLeft + boxA->GetWidth() * 0.5f) - (bLeft + boxB->GetWidth() * 0.5f),
                            (aTop + boxA->GetHeight() * 0.5f) - (bTop + boxB->GetHeight() * 0.5f) };

    if (overlapX < overlapY)
        return { diff.x < 0 ? -overlapX : overlapX, 0.0f };
    else
        return { 0.0f, diff.y < 0 ? -overlapY : overlapY };
}

const std::vector<Collider*>& ColliderManager::GetAllColliders() const {
    return m_colliders;
}
