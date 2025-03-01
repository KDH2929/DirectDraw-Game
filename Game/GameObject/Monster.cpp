#include "stdafx.h"
#include "Monster.h"
#include "CollisionQuery.h"
#include "Character.h"
#include "DebugManager.h"

Monster::Monster() : GameObject() {

}

Monster::~Monster() {

}

void Monster::Update(float deltaTime)
{
    GameObject::Update(deltaTime);
}

void Monster::Render(CDDrawDevice* pDevice)
{
    GameObject::Render(pDevice);
}


const Vector2<float> Monster::GetForwardVector()
{
    return m_forwardVec;
}

void Monster::SetForwardVector(Vector2<float> forwardVec)
{
    m_forwardVec = forwardVec;
}

bool Monster::CheckLeftObject()
{
    // 벽 Raycast 검사를 위한 설정
    const float ObjectCheckDistance = 0.3f;
    float ObjectHitDistance = 0.0f;
    const float ObjectCheckRayOffsetY = 5.0f;

    Collider* curCollider = GetCollider();

    if (curCollider) {
        if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(curCollider)) {
            // 플레이어 콜라이더의 경계 구하기
            int left = aabb->GetX();
            int right = aabb->GetX() + aabb->GetWidth();
            int top = aabb->GetY();
            int bottom = aabb->GetY() + aabb->GetHeight();

            Vector2<float> topLeft(static_cast<float>(left), static_cast<float>(top) + ObjectCheckRayOffsetY);
            Vector2<float> bottomLeft(static_cast<float>(left), static_cast<float>(bottom) - ObjectCheckRayOffsetY);


            // 좌측의 경우, 좌상단과 좌하단에서 왼쪽으로 Ray를 쏨
            Ray leftRayFromTop(topLeft, Vector2<float>(-1.0f, 0.0f));
            Ray leftRayFromBottom(bottomLeft, Vector2<float>(-1.0f, 0.0f));

            const auto& colliders = ColliderManager::GetInstance()->GetAllColliders();

            // 자기자신 Collider는 무시
            CollisionLayer queryMask = DEFAULT_COLLISION_LAYER_MASK & ~MONSTER_COLLISION_LAYER;

            for (Collider* collider : colliders) {
                if (collider == curCollider)
                    continue;

                if (collider->GetOwner()->GetPhysicsType() == PhysicsType::Dynamic)
                {
                    //continue;
                }

                if (CollisionQuery::Raycast(leftRayFromTop, collider, ObjectCheckDistance, ObjectHitDistance, queryMask)) {
                    return true;
                }

                if (CollisionQuery::Raycast(leftRayFromBottom, collider, ObjectCheckDistance, ObjectHitDistance, queryMask)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Monster::CheckRightObject()
{
    Collider* curCollider = GetCollider();

    const float ObjectCheckDistance = 0.3f;
    const float ObjectCheckRayOffsetY = 5.0f;
    float ObjectHitDistance = 0.0f;


    if (curCollider) {
        if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(curCollider)) {
            // 플레이어 콜라이더의 경계 구하기
            int left = aabb->GetX();
            int right = aabb->GetX() + aabb->GetWidth();
            int top = aabb->GetY();
            int bottom = aabb->GetY() + aabb->GetHeight();


            Vector2<float> topRight(static_cast<float>(right), static_cast<float>(top) + ObjectCheckRayOffsetY);
            Vector2<float> bottomRight(static_cast<float>(right), static_cast<float>(bottom) - ObjectCheckRayOffsetY);


            Ray rightRayFromTop(topRight, Vector2<float>(1.0f, 0.0f));
            Ray rightRayFromBottom(bottomRight, Vector2<float>(1.0f, 0.0f));


            const auto& colliders = ColliderManager::GetInstance()->GetAllColliders();

            // 자기자신 Collider는 무시
            CollisionLayer queryMask = DEFAULT_COLLISION_LAYER_MASK & ~MONSTER_COLLISION_LAYER;

            for (Collider* collider : colliders) {
                if (collider == curCollider)
                    continue;

                if (collider->GetOwner()->GetPhysicsType() == PhysicsType::Dynamic)
                {
                    //continue;
                }


                if (CollisionQuery::Raycast(rightRayFromTop, collider, ObjectCheckDistance, ObjectHitDistance, queryMask)) {
                    return true;
                }

                if (CollisionQuery::Raycast(rightRayFromBottom, collider, ObjectCheckDistance, ObjectHitDistance, queryMask)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Monster::CheckForwardObject()
{
    const Vector2<float> forward = GetForwardVector();

    if (forward.x < 0)
    {
        return CheckLeftObject();
    }
    else if (forward.x > 0)
    {
        return CheckRightObject();
    }

    return false;
}

bool Monster::CheckGround()
{
    // Raycast를 통한 지면 판정 (AABB 콜라이더 기준, 양쪽 하단에서 검사)
    const float groundCheckRayOffsetX = 5.0f;
    const float groundThreshold = 0.3f;
    float groundHitDistance = 0.0f;

    Collider* curCollider = GetCollider();

    if (curCollider) {
        if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(curCollider)) {
            int left = aabb->GetX();
            int right = aabb->GetX() + aabb->GetWidth();
            int bottom = aabb->GetY() + aabb->GetHeight();

            Vector2<float> leftOrigin(static_cast<float>(left), static_cast<float>(bottom));
            Vector2<float> rightOrigin(static_cast<float>(right), static_cast<float>(bottom));
            leftOrigin.x += groundCheckRayOffsetX;
            rightOrigin.x -= groundCheckRayOffsetX;

            Ray leftRay(leftOrigin, Vector2<float>(0.0f, 1.0f));
            Ray rightRay(rightOrigin, Vector2<float>(0.0f, 1.0f));

            // 자기자신 Collider는 무시
            CollisionLayer queryMask = DEFAULT_COLLISION_LAYER_MASK & ~MONSTER_COLLISION_LAYER;

            const auto& colliders = ColliderManager::GetInstance()->GetAllColliders();
            for (Collider* collider : colliders) {
                if (collider == curCollider)
                    continue;

                if (CollisionQuery::Raycast(leftRay, collider, groundThreshold, groundHitDistance, queryMask)) {
                    return true;
                }
                if (CollisionQuery::Raycast(rightRay, collider, groundThreshold, groundHitDistance, queryMask)) {
                    return true;
                }
            }

        }
    }

    return false;
}


bool Monster::CheckForwardGround() {

    Collider* curCollider = GetCollider();
    if (!curCollider)
        return false;
    
    AABBCollider* aabb = dynamic_cast<AABBCollider*>(curCollider);
    if (!aabb)
        return false;

    // 몬스터의 Collider 경계 구하기
    int left = aabb->GetX();
    int right = aabb->GetX() + aabb->GetWidth();
    int bottom = aabb->GetY() + aabb->GetHeight();

    // 이동 방향 (예: 오른쪽 이동이면 (1,0), 왼쪽 이동이면 (-1,0))
    Vector2<float> forward = GetForwardVector();

    const float forwardOffset = 20.0f;
    const float groundRayOffsetY = 5.0f; 


    Vector2<float> origin;
    
    // 우측을 바라보는 경우
    if (forward.x > 0) {
        origin = Vector2<float>(right + forwardOffset, static_cast<float>(bottom));
        origin.y -= groundRayOffsetY;
    }

    // 좌측을 바라보는 경우
    else {
        origin = Vector2<float>(left - forwardOffset, static_cast<float>(bottom));
        origin.y -= groundRayOffsetY;
    }

    // 아래 방향으로 Raycast (방향: (0,1))
    Ray forwardGroundRay(origin, Vector2<float>(0.0f, 1.0f));

    const float groundThreshold = 500.0f;
    float groundHitDistance = 0.0f;
    CollisionLayer queryMask = DEFAULT_COLLISION_LAYER_MASK & ~MONSTER_COLLISION_LAYER;

    queryMask &= ~OVERLAP_COLLISION_LAYER;

    const auto& colliders = ColliderManager::GetInstance()->GetAllColliders();

    for (Collider* collider : colliders) {
        if (collider == curCollider)
            continue;
        if (CollisionQuery::Raycast(forwardGroundRay, collider, groundThreshold, groundHitDistance, queryMask)) {
            return true;
        }
    }
    return false;
}



Character* Monster::DetectPlayer(float rayDistance) {

    // 몬스터는 현재 AABB Collider
    Vector2<float> origin = static_cast<AABBCollider*>(GetCollider())->GetCenterPosition();
    Vector2<float> forward = GetForwardVector();
    float hitDistance = 0.0f;

    // Ray 생성
    Ray detectRay(origin, forward);
    CollisionLayer queryMask = CHARACTER_COLLISION_LAYER & ~MONSTER_COLLISION_LAYER;

    const auto& colliders = ColliderManager::GetInstance()->GetAllColliders();
    for (Collider* collider : colliders) {
        // 자기 자신의 Collider는 무시
        if (collider == GetCollider())
            continue;


        Vector2<float> hitPoint = origin + forward * rayDistance;

        // DebugLine 생성: 시작점은 origin, 끝점은 hitPoint, 색상은 빨간색, 두께 2, 지속시간 0.5초
        DebugLine debugLine(origin, hitPoint, RGB(255, 0, 0), 2, 0.03f);
        DebugManager::GetInstance()->AddDebugLine(debugLine);


        if (CollisionQuery::Raycast(detectRay, collider, rayDistance, hitDistance, queryMask)) {


            // 콜라이더의 Owner가 Character 타입인지 확인
            Character* DetectedObject = dynamic_cast<Character*>(collider->GetOwner());
            if (DetectedObject != nullptr) {
              
                // Dynamic cast는 캐스팅 실패 시 nullptr 이라는 점을 활용

                DebugManager::GetInstance()->AddOnScreenMessage(L"Player Detected!", 1.0f);


                DebugLine debugLineHit(origin, hitPoint, RGB(0, 255, 0), 3, 2.0f);
                DebugManager::GetInstance()->AddDebugLine(debugLineHit);

                return DetectedObject;
            }
        }
    }
    return nullptr;
}


std::wstring Monster::MonsterAIStateToWString(MonsterAIState state) {
    switch (state) {
    case MonsterAIState::Wait:
        return L"Wait";
    case MonsterAIState::Move:
        return L"Move";
    case MonsterAIState::Turn:
        return L"Turn";
    case MonsterAIState::Attack:
        return L"Attack";
    case MonsterAIState::Chase:
        return L"Chase";
    case MonsterAIState::Dead:
        return L"Dead";
    default:
        return L"Unknown";
    }
}