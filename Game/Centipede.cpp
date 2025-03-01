#include "stdafx.h"
#include "Centipede.h"
#include "AABBCollider.h"
#include "../ImageData/ImageData.h"
#include "../DDrawLib/DDrawDevice.h"
#include "DebugManager.h"
#include "SpriteManager.h"
#include "CentipedeAnim.h"
#include "InputManager.h"
#include "CollisionQuery.h"
#include "Character.h"

#include <algorithm>
#include <cmath>

Centipede::Centipede(CImageData* pMonsterImgData, float startX, float startY)
    : m_health(100.0f), m_speed(4.0f), m_baseSpeed(3.0f),
    m_verticalVelocity(0.0f),
    m_spriteManager(nullptr),
    m_anim(nullptr),
    m_AIState(MonsterAIState::Wait)
{
    SetPosition(Vector2<float>{ startX, startY });
    SetColliderLocalPosition(Vector2<float>{65, 100});
    InitCollider(startX + GetColliderLocalPosition().x, startY + GetColliderLocalPosition().y, 160, 120);
    InitSpriteManager(pMonsterImgData, 216, 216);
    InitAnimation();
    SetPhysicsType(PhysicsType::Dynamic);
    SetHealth(100.0f);
    SetForwardVector(Vector2<float>(-1.0f, 0.0f));
}



Centipede::~Centipede() {
    if (m_spriteManager) {
        delete m_spriteManager;
        m_spriteManager = nullptr;
    }
    if (m_anim) {
        delete m_anim;
        m_anim = nullptr;
    }
}

void Centipede::Update(float deltaTime) {

    // 중력적용 유무를 위한 바닥체크
    m_isGrounded = CheckGround();

    UpdateAI(deltaTime);

    // 중력적용
    Vector2<float> pos = GetTransform().position;
    if (!m_isGrounded) {
        m_verticalVelocity += GRAVITY_ACCEL * deltaTime;
        pos.y += m_verticalVelocity * deltaTime;
    }
    else {
        m_verticalVelocity = 0.0f;
    }

    SetPosition(pos);

    if (m_anim) {
        m_anim->Update(deltaTime);
        UpdateAnimFSM();
    }

    Monster::Update(deltaTime);
}

void Centipede::Render(CDDrawDevice* pDevice) {
    if (pDevice && m_spriteManager && m_anim) {
        // 현재 애니메이션 프레임에 해당하는 srcRect를 SpriteManager에서 가져옴
        int currentFrame = m_anim->GetCurrentFrame();
        const RECT& srcRect = m_spriteManager->GetFrameRect(currentFrame);


        const float FLIP_RENDER_OFFSET_X = 75.0f;


        // 화면상의 렌더링 위치
        Vector2<float> renderPos = GetRenderPosition();

        if (GetForwardVector().x <= 0) {
            pDevice->DrawSprite(static_cast<int>(renderPos.x), static_cast<int>(renderPos.y),
                m_spriteManager->GetSpriteSheet(), srcRect);
        }

        else {
            pDevice->DrawSpriteFlip(static_cast<int>(renderPos.x + FLIP_RENDER_OFFSET_X), static_cast<int>(renderPos.y),
                m_spriteManager->GetSpriteSheet(), srcRect);
        }
    }

    Monster::Render(pDevice);
}


int Centipede::GetSpriteFrameWidth() const {
    return m_spriteManager ? m_spriteManager->GetFrameWidth() : 0;
}

int Centipede::GetSpriteFrameHeight() const {
    return m_spriteManager ? m_spriteManager->GetFrameHeight() : 0;
}

MonsterType Centipede::GetMonsterType() const {
    return MonsterType::Centipede;
}

void Centipede::TakeDamage(const DamageInfo& damageInfo)
{
    int currentHP = GetHealth();
    SetHealth(currentHP - damageInfo.damageAmount);

    CentipedeAnimState currentState = m_anim->GetState();

    if (!m_isAttacking) {
        m_anim->SetState(CentipedeAnimState::Hurt);
        m_isHurt = true;
    }

    if (GetHealth() <= 0) {
        Dead();
    }
}

void Centipede::InitCollider(int x, int y, int width, int height) {
    if (m_pCollider)
        delete m_pCollider;

    Vector2<float> pos = GetTransform().position;
    m_pCollider = new AABBCollider(this, x, y, width, height);
    m_pCollider->SetCollisionLayer(MONSTER_COLLISION_LAYER);
    ColliderManager::GetInstance()->AddCollider(GetCollider());
}

void Centipede::InitSpriteManager(CImageData* spriteSheet, int frameWidth, int frameHeight) {
    if (m_spriteManager)
        delete m_spriteManager;

    m_spriteManager = new SpriteManager;
    m_spriteManager->SetSpriteSheet(spriteSheet, frameWidth, frameHeight);
}

void Centipede::InitAnimation() {
    if (!m_spriteManager) {
        DebugManager::GetInstance()->AddOnScreenMessage(L"SpriteManager is not initialized in Centipede::InitAnimation()", 20.0f);
    }

    if (m_anim)
        delete m_anim;

    m_anim = new CentipedeAnim;

    std::vector<CentipedeAnim::AnimSequence> sequences;
    const float SECONDS_TO_MS = 1000.0f;
    int currentFrame = 0;

    // 프레임 설정은 스프라이트 이미지를 참고할 것

    // Attack1: 4 프레임
    sequences.push_back({ CentipedeAnimState::Attack1, currentFrame, currentFrame + 3, 0.1f * SECONDS_TO_MS });
    currentFrame += 6;

    // Attack2: 6 프레임
    sequences.push_back({ CentipedeAnimState::Attack2, currentFrame, currentFrame + 5, 0.08f * SECONDS_TO_MS });
    currentFrame += 6;

    // Attack3: 6 프레임
    sequences.push_back({ CentipedeAnimState::Attack3, currentFrame, currentFrame + 5, 0.08f * SECONDS_TO_MS });
    currentFrame += 6;

    // Attack4: 4 프레임
    sequences.push_back({ CentipedeAnimState::Attack4, currentFrame, currentFrame + 3, 0.1f * SECONDS_TO_MS });
    currentFrame += 6;

    // Death: 4 프레임
    sequences.push_back({ CentipedeAnimState::Death, currentFrame, currentFrame + 3, 0.15f * SECONDS_TO_MS, false });
    currentFrame += 6;

    // Hurt: 2 프레임
    sequences.push_back({ CentipedeAnimState::Hurt, currentFrame, currentFrame + 1, 0.1f * SECONDS_TO_MS });
    currentFrame += 6;

    // Idle: 4 프레임
    sequences.push_back({ CentipedeAnimState::Idle, currentFrame, currentFrame + 3, 0.15f * SECONDS_TO_MS });
    currentFrame += 6;

    // Sneer: 6 프레임
    sequences.push_back({ CentipedeAnimState::Sneer, currentFrame, currentFrame + 5, 0.1f * SECONDS_TO_MS });
    currentFrame += 6;

    // Walk: 4 프레임
    sequences.push_back({ CentipedeAnimState::Walk, currentFrame, currentFrame + 3, 0.12f * SECONDS_TO_MS });
    currentFrame += 6;

    m_anim->SetSequences(sequences);
    m_anim->SetState(CentipedeAnimState::Idle);

    m_attack2EndFrame = m_anim->GetSequence(CentipedeAnimState::Attack2)->endFrame;
    m_hurtEndFrame = m_anim->GetSequence(CentipedeAnimState::Hurt)->endFrame;
    
}

void Centipede::UpdateAnimFSM() {

    // 사망했을 시 무조건 Death 상태
    if (m_isDead) {
        m_anim->SetState(CentipedeAnimState::Death);
        return;
    }

    if (m_isHurt) {
        if (m_anim->GetCurrentFrame() >= m_hurtEndFrame) {
            m_isHurt = false;
        }
        else {
            m_anim->SetState(CentipedeAnimState::Hurt);
            return;
        }
    }


    if (m_isAttacking) {
        m_anim->SetState(CentipedeAnimState::Attack2);
        return;
    }

    const float speedThreshold = 1.0f;
    if (std::abs(m_speed) > speedThreshold) {
        m_anim->SetState(CentipedeAnimState::Walk);
    }
    else {
        m_anim->SetState(CentipedeAnimState::Idle);
    }

}



void Centipede::UpdateAI(float deltaTime) {

    if (m_isDead)
    {
        return;
    }

    const float MS_TO_SEC = 0.001f;
    const float SEC_TO_MS = 1000.0f;

    m_waitTime = 2.0f * SEC_TO_MS;
    m_attackDelay = 2.0f * SEC_TO_MS;

    // 몬스터가 향하는 방향에 벽이 있는지 판정
    bool forwardObjectDetected = CheckForwardObject();

    /*
    std::wstring ObjectDetectMessage = L"LeftObject: " + std::wstring(leftObjectDetected ? L"true" : L"false") +
        L", RightObject: " + std::wstring(rightObjectDetected ? L"true" : L"false");

    DebugManager::GetInstance()->AddOnScreenMessage(ObjectDetectMessage, 0.03f);
    */
 
    if (m_attackDelayTimer <= 0.0f && m_AIState != MonsterAIState::Chase && m_AIState != MonsterAIState::Attack) {
        // Ray 감지 범위 200.0f
        Character* detectedPlayer = DetectPlayer(200.0f);
        if (detectedPlayer) {
            m_chaseTarget = detectedPlayer;
            m_AIState = MonsterAIState::Chase;
        }
    }


    if (m_attackDelayTimer > 0.0f) {
        m_attackDelayTimer -= deltaTime;
    }


    // AI 관련 FSM (유한상태기기)
    switch (m_AIState) {
    case MonsterAIState::Wait:
    {
        m_speed = 0;

        // 대기 시간 경과 검사
        if (m_currentWaitTimer > 0.0f) {
            m_currentWaitTimer -= deltaTime;
        }
        else {
            if (!CheckForwardGround() || forwardObjectDetected) {
                m_AIState = MonsterAIState::Turn;
            }
            else {
                m_AIState = MonsterAIState::Move;
            }
        }
        break;
    }


    case MonsterAIState::Move:
    {
        // 이동 처리
        if (!CheckForwardGround() || forwardObjectDetected) {
            m_speed = 0;
            m_AIState = MonsterAIState::Wait;
            m_currentWaitTimer = m_waitTime;        // waitTime만큼 대기
        }
        else {
            Vector2<float> pos = GetTransform().position;
            m_speed = m_baseSpeed;
            pos.x += m_speed * GetForwardVector().x;
            SetPosition(pos);
        }
        break;
    }


    case MonsterAIState::Turn:
    {
        SetForwardVector(-GetForwardVector());
        m_AIState = MonsterAIState::Wait;
        m_currentWaitTimer = m_waitTime;
        break;
    }


    case MonsterAIState::Attack:
    {
        if (!m_isAttacking) {
            m_anim->SetState(CentipedeAnimState::Attack2);
            m_isAttacking = true;
        }

        // 공격 상태인 경우, 특정 프레임에 도달하면 한 번만 Attack() 호출
        if (m_isAttacking) {
            if (!m_attack2OverlapBoxSpawned && m_anim->GetCurrentFrame() >= m_attack2EndFrame - 3) {
                Attack();           // overlapBox 공격판정 수행
                m_attack2OverlapBoxSpawned = true;
            }

            // 공격 종료 프레임에 도달 시
            if (m_anim->GetCurrentFrame() >= m_attack2EndFrame) {
                m_isAttacking = false;
                m_attack2OverlapBoxSpawned = false;
                m_attackDelayTimer = m_attackDelay;  // 공격 후 딜레이 타이머 재설정
                m_AIState = MonsterAIState::Chase;    // 추격 상태로 전환
            }
        }
        break;
    }


    case MonsterAIState::Chase:
    {
        Chase();        // Chase 함수에서 AI FSM 전환로직 수행
        break;
    }

    default:
        break;
    }

    /*
    std::wstring AIStateMessage = L"MonsterState: " + MonsterAIStateToWString(m_AIState);
    DebugManager::GetInstance()->AddOnScreenMessage(AIStateMessage, 0.03f);
    */

}



void Centipede::Attack() {
    AABBCollider* curCollider = dynamic_cast<AABBCollider*>(GetCollider());
    if (!curCollider)
        return;

    Vector2<float> ColliderCenterPos = curCollider->GetCenterPosition();
    
    // OBB 박스
    Vector2<float> boxCenter = ColliderCenterPos + Vector2<float>{85.0f * GetForwardVector().x, -35.0f};
    float boxWidth = 120.0f;
    float boxHeight = 80.0f;
    float boxRotation = 0.0f;

    // 디버그 용 박스
    DebugBox debugBox(boxCenter, boxWidth, boxHeight, boxRotation, RGB(255, 255, 0), 2.0f);
    DebugManager::GetInstance()->AddDebugBox(debugBox);

    CollisionLayer queryMask = CHARACTER_COLLISION_LAYER & ~MONSTER_COLLISION_LAYER;
    const auto& colliders = ColliderManager::GetInstance()->GetAllColliders();
    for (Collider* collider : colliders) {
        if (collider == curCollider)
            continue;

        if (CollisionQuery::OverlapBox(boxCenter, boxWidth, boxHeight, boxRotation, collider, queryMask)) {
            GameObject* targetObject = collider->GetOwner();
            if (targetObject) {
                DamageInfo damageInfo(25.0f, this, boxCenter);
                targetObject->TakeDamage(damageInfo);
            }
        }
    }
}

void Centipede::Chase()
{
    if (!m_chaseTarget) {
        // 대상이 없으면 Wait 상태로 전환
        m_AIState = MonsterAIState::Wait;
        return;
    }

    // Collider 중심을 기준으로 위치 계산
    Vector2<float> curPos = GetCollider()->GetCenterPosition();
    Vector2<float> targetPos = m_chaseTarget->GetCollider()->GetCenterPosition();
    Vector2<float> toTargetVec = targetPos - curPos;
    float distance = toTargetVec.Length();

    m_attackRange = 150.0f;
    m_chaseStopDistance = 600.0f;

    // 공격 상태 전환 조건
    if (m_attackDelayTimer <= 0.0f  && distance < m_attackRange) {
        m_AIState = MonsterAIState::Attack;
        return;
    }


    // Chase 상태 중, 만약 플레이어와의 거리가 너무 멀면(m_chaseStopDistance 이상) 대기 상태로 전환
    if (distance >= m_chaseStopDistance) {
        m_AIState = MonsterAIState::Wait;
        m_currentWaitTimer = m_waitTime;
        return;
    }

    /*
    플레이어가 몬스터 바로 위에 있을 시 위의 로직에 의해 좌우로 계속왔다 갔다 거림
        -> 몬스터와 플레이어와의 x축거리가 너무나도 가까울 시 방향전환을 안하도록
    */

    float xDiff = std::abs(targetPos.x - curPos.x);
    const float xDiffThreshold = 20.0f;

    // 플레이어 방향으로 이동: 현재 Forward 벡터와 플레이어 방향(toTargetVec)의 내적으로 판단
    Vector2<float> forwardVec = GetForwardVector();
    Vector2<float> normalizedToTargetVec = Normalize(toTargetVec);
    if (Dot(normalizedToTargetVec, forwardVec) < 0) {
        // 플레이어가 몬스터 뒤에 있다면, forward 벡터 반전

        // 플레이어와의 거리가 너무 가까우면 방향전환을 안하도록
        if (xDiff > xDiffThreshold)
        {
            SetForwardVector(-forwardVec);
        }
    }



    m_speed = m_baseSpeed;
    
    // 플레이어의 위치는 pos를 기준으로 업데이트
    Vector2<float> pos = GetPosition();

    if (CheckForwardGround() && !CheckForwardObject())
    {
        pos += (m_speed * GetForwardVector());
        SetPosition(pos);
    }


    // 디버그: 플레이어와의 거리 출력
    std::wstring chaseMsg = L"Chasing: Distance = " + std::to_wstring(distance);
    DebugManager::GetInstance()->AddOnScreenMessage(chaseMsg, 0.03f);
}


void Centipede::Dead()
{
    if (m_isDead)
        return;

    m_isDead = true;

    m_anim->SetState(CentipedeAnimState::Death);
    GetCollider()->SetCollisionResponse(CollisionResponse::Ignore);
    DebugManager::GetInstance()->AddOnScreenMessage(L"Centipede Dead!", 2.0f);
}
