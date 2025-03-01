#include "stdafx.h"
#include "ColliderManager.h"
#include "AABBCollider.h"
#include "GameObject.h"
#include "MathUtils.h"
#include <algorithm>
#include <sstream>
#include <unordered_map>

ColliderManager* ColliderManager::s_instance = nullptr;


ColliderManager* ColliderManager::GetInstance() {
    if (!s_instance) {
        s_instance = new ColliderManager();
    }
    return s_instance;
}


// Scene 클래스의 Initialize에서 호출할 초기화 함수
void ColliderManager::Initialize()
{
    if (s_instance)
    {
        delete s_instance;
        s_instance = nullptr;
    }

    s_instance = new ColliderManager();
}

void ColliderManager::DestroyInstance()
{
    delete s_instance;
    s_instance = nullptr;
}


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

    // 동적할당 해제는 Collider를 생성한 곳에서 관리할 것
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
    std::unordered_map<GameObject*, Vector2<float>> mtvMap;

    // 모든 충돌 쌍을 순회하며 처리
    for (auto& pair : collisions) {
        Collider* colliderA = pair.first;
        Collider* colliderB = pair.second;
        GameObject* objA = colliderA->GetOwner();
        GameObject* objB = colliderB->GetOwner();

        if (objA && objB) {
            CollisionResponse responseA = colliderA->GetCollisionResponse();
            CollisionResponse responseB = colliderB->GetCollisionResponse();


            CollisionResponse finalResponse = CollisionResponse::Ignore;

            if (responseA == CollisionResponse::Ignore || responseB == CollisionResponse::Ignore) 
            {
                finalResponse = CollisionResponse::Ignore;
            }

            else if (responseA == CollisionResponse::Block && responseB == CollisionResponse::Block) 
            {
                finalResponse = CollisionResponse::Block;
            }

            else if (responseA == CollisionResponse::Overlap || responseB == CollisionResponse::Overlap) 
            {
                finalResponse = CollisionResponse::Overlap;
            }


            Vector2<float> mtv = ComputeAABBMTV(colliderA, colliderB);

            // Block와 Overlap 인 경우 이벤트 발생
            if (finalResponse != CollisionResponse::Ignore) {                

                // MTV의 방향을 법선, 길이를 침투 깊이로 사용
                Vector2<float> normal = mtv;
                float penetrationDepth = normal.Length();
                if (penetrationDepth > 0.0f)
                    normal = normal / penetrationDepth;

                // 두 객체의 중간 위치를 충돌 지점으로 사용
                Vector2<float> collisionPoint = (Vector2<float>(objA->GetPosition()) + Vector2<float>(objB->GetPosition())) * 0.5f;


                // CollisionInfo 구조체 생성
                CollisionInfo infoA{ objB, finalResponse, normal, collisionPoint, penetrationDepth };
                CollisionInfo infoB{ objA, finalResponse, -normal, collisionPoint, penetrationDepth };

                objA->OnCollision(infoA);
                objB->OnCollision(infoB);
            }


            if (finalResponse == CollisionResponse::Block) {
                // 각 객체의 역질량(InvMass)를 구함
                float invMassA = objA->GetInvMass();
                float invMassB = objB->GetInvMass();

                // 두 객체 모두 정적이거나 충돌 처리가 필요 없는 경우는 건너뛰기
                if ((invMassA + invMassB) > 0.0f) {
                    // 질량의 역수를 이용해 MTV를 분담함
                    Vector2<float> offsetA = mtv * (invMassA / (invMassA + invMassB));
                    Vector2<float> offsetB = mtv * (invMassB / (invMassA + invMassB));

                    // Dynamic인 경우에만 적용 (Static이면 움직이지 않음)
                    if (objA->GetPhysicsType() == PhysicsType::Dynamic) {
                        // 기존에 저장된 MTV가 없거나, 새 offset의 크기가 더 크면 업데이트
                        if (mtvMap.find(objA) == mtvMap.end()) {
                            mtvMap[objA] = offsetA;
                        }
                        else {
                            if (offsetA.Length() > mtvMap[objA].Length()) {
                                mtvMap[objA] = offsetA;
                            }
                        }
                    }

                    // objB의 경우 (단, offset은 반대 방향이므로 -offsetB 사용)
                    if (objB->GetPhysicsType() == PhysicsType::Dynamic) {
                        if (mtvMap.find(objB) == mtvMap.end()) {
                            mtvMap[objB] = -offsetB;
                        }
                        else {
                            if (offsetB.Length() > mtvMap[objB].Length()) {
                                mtvMap[objB] = -offsetB;
                            }
                        }
                    }
                }
            }
        }
    }

    // MTV를 각 GameObject들에 한 번에 처리
    for (auto& entry : mtvMap) {
        GameObject* obj = entry.first;
        Vector2<float> MTV = entry.second;
        obj->SetPosition(obj->GetPosition() + Vector2<float>(MTV));
    }


    collisions.clear();
}




// AABB 충돌에 대한 MTV(최소 이동 벡터)를 계산하는 함수
// 두 Collider 객체가 AABB 타입이라고 가정함
Vector2<float> ColliderManager::ComputeAABBMTV(Collider* a, Collider* b) {
    // Collider가 AABB 타입인지 확인
    AABBCollider* boxA = dynamic_cast<AABBCollider*>(a);
    AABBCollider* boxB = dynamic_cast<AABBCollider*>(b);
    if (!boxA || !boxB)
        return Vector2<float>{ 0.0f, 0.0f };

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
        return Vector2<float>{ 0.0f, 0.0f };

    // 각 박스의 중심 좌표 계산
    float aCenterX = aLeft + boxA->GetWidth() / 2.0f;
    float aCenterY = aTop + boxA->GetHeight() / 2.0f;
    float bCenterX = bLeft + boxB->GetWidth() / 2.0f;
    float bCenterY = bTop + boxB->GetHeight() / 2.0f;

    // 두 중심 간 차이 벡터 계산 (내 객체에서 상대 객체를 빼는 방식)
    Vector2<float> diff = { aCenterX - bCenterX, aCenterY - bCenterY };

    // 최소 이동 벡터(MTV) 결정: 겹침이 적은 축을 기준으로 이동
    Vector2<float> mtv = { 0.0f, 0.0f };
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

const std::vector<Collider*>& ColliderManager::GetAllColliders() const
{
    return m_colliders;
}
