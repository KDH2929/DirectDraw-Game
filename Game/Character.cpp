#include "stdafx.h"
#include "Character.h"
#include "GameObject.h"
#include "AABBCollider.h"
#include "../ImageData/ImageData.h"
#include "../DDrawLib/DDrawDevice.h"
#include "InputManager.h"
#include "DebugManager.h"
#include "SpriteManager.h"
#include "CharacterAnim.h"
#include "Raycast.h"

#include <typeinfo>
#include <algorithm>


Character::Character(CImageData* pPlayerImgData, float startX, float startY)
    : m_health(100), m_speed(8), m_verticalVelocity(0.0f), m_isGrounded(false), 
    m_spriteManager(nullptr), m_anim(nullptr)
{
    SetPosition(Vector2<float>{ startX, startY });
    SetColliderLocalPosition(Vector2<float>{65, 20});
    SetPhysicsType(PhysicsType::Dynamic);
    SetSpeed(8);
    InitCollider(startX + GetColliderLocalPosition().x, startY + GetColliderLocalPosition().y, 85, 155);
    InitSpriteManager(pPlayerImgData, 276, 176);
    InitAnimation();
}


Character::~Character()
{
    if (m_spriteManager) {
        delete m_spriteManager;
        m_spriteManager = nullptr;
    }
    if (m_anim) {
        delete m_anim;
        m_anim = nullptr;
    }
}


void Character::Update(float deltaTime)
{
    Vector2<float> pos = GetTransform().position;
    m_isRunning = false;

    // 좌우 이동 및 달리기 상태 갱신
    if (InputManager::GetInstance().IsKeyDown(VK_LEFT)) {
        pos.x -= GetSpeed();
        m_isRunning = true;

        if (!m_isDashing && !m_isAttacking)
        {
            m_forwardVec = Vector2<float>(-1.0f, 0.0f);
        }
    
    }
    if (InputManager::GetInstance().IsKeyDown(VK_RIGHT)) {
        pos.x += GetSpeed();
        m_isRunning = true;

        if (!m_isDashing && !m_isAttacking)
        {
            m_forwardVec = Vector2<float>(1.0f, 0.0f);
        }
    }

    if (InputManager::GetInstance().IsKeyDown(VK_UP))
    {
        //pos.y -= GetSpeed();
    }

    if (InputManager::GetInstance().IsKeyDown(VK_DOWN))
    {
        //pos.y += GetSpeed();
    }

    // 공격 입력 처리 (C 키)
    if (InputManager::GetInstance().IsKeyPressed('C')) {

        // 만약 대시 중이면 공격 입력은 대시를 캔슬하고 공격 전환
        if (m_isDashing && !m_isDashAttacking) {
            m_isDashAttacking = true;
            m_anim->SetState(CharacterAnimState::DashAttack);
        }


        if (!m_isAttacking) {
            // 처음 공격 입력: 첫 공격 시작
            m_isAttacking = true;
            m_attackComboStage = 1;
            m_currentAttackEndFrame = m_firstAttackEndFrame;
            m_comboInputReceived = false;
            m_anim->SetState(CharacterAnimState::Attack);
        }
        else {
            // 이미 공격 중이면 추가 입력 플래그 설정
            m_comboInputReceived = true;
        }

    }


    if (InputManager::GetInstance().IsDoublePressedLeft() && !m_isDashing) {
        // 대시 입력은 기존상태를 캔슬하고 대시 상태로 전환

        DebugManager::GetInstance().AddOnScreenMessage(L"LeftKey is double preseed", 3.0f);
        m_isAttacking = false;
        m_isDashing = true;
        m_anim->SetState(CharacterAnimState::Dash);
    }


    if (InputManager::GetInstance().IsDoublePressedRight() && !m_isDashing) {
        // 대시 입력은 공격 상태를 캔슬하고 대시 상태로 전환

        DebugManager::GetInstance().AddOnScreenMessage(L"RightKey is double preseed", 3.0f);
        m_isAttacking = false;
        m_isDashing = true;
        m_anim->SetState(CharacterAnimState::Dash);
    }


    if (m_isDashing)
    {
        pos.x += m_forwardVec.x * m_dashSpeed;
    }


    // Raycast를 통한 지면 판정 (AABB 콜라이더 기준, 양쪽 하단에서 검사)
    Collider* curCollider = GetCollider();
    bool isGrounded = false;
    const float rayOffset = 1.0f;
    const float groundThreshold = 1.0f;
    float hitDistance = 0.0f;

    if (curCollider) {
        if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(curCollider)) {
            int left = aabb->GetX();
            int right = aabb->GetX() + aabb->GetWidth();
            int bottom = aabb->GetY() + aabb->GetHeight();

            Vector2<float> leftOrigin(static_cast<float>(left), static_cast<float>(bottom));
            Vector2<float> rightOrigin(static_cast<float>(right), static_cast<float>(bottom));
            leftOrigin.y += rayOffset;
            rightOrigin.y += rayOffset;

            Ray leftRay(leftOrigin, Vector2<float>(0.0f, 1.0f));
            Ray rightRay(rightOrigin, Vector2<float>(0.0f, 1.0f));
            
            const auto& colliders = ColliderManager::GetInstance().GetColliders();
            for (Collider* collider : colliders) {
                if (collider == curCollider)
                    continue;
                if (Raycast::IntersectCollider(leftRay, collider, hitDistance) && hitDistance <= groundThreshold) {
                    isGrounded = true;
                    break;
                }
                if (Raycast::IntersectCollider(rightRay, collider, hitDistance) && hitDistance <= groundThreshold) {
                    isGrounded = true;
                    break;
                }
            }
            
        }
    }

    m_isGrounded = isGrounded;


    // '떨어지기 직전' 상태 업데이트: 공중 상태이며 하강 중일 때, raycast hitDistance가 일정 범위 내이면 true
    const float upToFallThreshold = 60.0f;
    if (!m_isGrounded && m_verticalVelocity > 0) {
        m_isUpToFall = (hitDistance <= upToFallThreshold);
    }
    else {
        m_isUpToFall = false;
    }



    // 점프 입력 처리 (X 키, 땅에 닿은 상태에서만)
    if (InputManager::GetInstance().IsKeyPressed('X') && m_isGrounded) {
        //DebugManager::GetInstance().AddOnScreenMessage(L"Key X is pressed", 5.0f);
        m_verticalVelocity = JUMP_VELOCITY;
        m_isGrounded = false;
    }



    // 중력 적용 (땅에 닿지 않은 경우)
    if (!m_isGrounded) {
        m_verticalVelocity += GRAVITY_ACCEL * deltaTime;
        pos.y += m_verticalVelocity * deltaTime;
    }

    else {

        // DebugManager::GetInstance().AddOnScreenMessage(L"Player is on ground", 5.0f);
        m_verticalVelocity = 0;
    }

    SetPosition(pos);


    if (m_anim)
        m_anim->Update(deltaTime);



    // 공격 애니메이션 상태에서 콤보 처리
    if (m_isAttacking && m_anim->GetState() == CharacterAnimState::Attack) {
        // 현재 공격 애니메이션의 종료 프레임에 도달했으면
        if (m_anim->GetCurrentFrame() >= m_currentAttackEndFrame) {
            if (m_comboInputReceived) {
                // 추가 입력이 있었으면 다음 콤보 단계로 전환
                m_attackComboStage++;
      
                if (m_attackComboStage == 2) {
                    m_currentAttackEndFrame = m_secondAttackEndFrame;
                    m_comboInputReceived = false;
                }

                else if (m_attackComboStage >= 3)
                {
                    // 연속공격종료
                    m_isAttacking = false;
                    m_comboInputReceived = false;
                    m_attackComboStage = 0;
                    m_currentAttackEndFrame = m_firstAttackEndFrame;
                }
                

                // 추가 콤보 단계가 더 있다면 else if (m_attackComboStage == 3) ... 등을 추가
            }

            else {
                // 추가 입력이 없으면 공격 종료
                m_isAttacking = false;
                m_comboInputReceived = false;
                m_attackComboStage = 0;
                m_currentAttackEndFrame = m_firstAttackEndFrame;
            }
        }
    }



    if (m_isDashing && m_anim->GetState() == CharacterAnimState::Dash) 
    {
        if (m_anim->GetCurrentFrame() >= m_DashEndFrame) {
            m_isDashing = false;
            m_isAttacking = false;
        }
    }

    if (m_isDashAttacking && m_anim->GetState() == CharacterAnimState::DashAttack)
    {
        if (m_anim->GetCurrentFrame() >= m_DashAttackEndFrame) {
            m_isDashAttacking = false;
            m_isAttacking = false;
            m_isDashing = false;
        }
    }


    UpdateAnimFSM();

    // 자식클래스에서 부모클래스의 Update를 마지막에 호출하는 이유는 최종위치를 정한 후, 
    // 위치보간을 부모클래스에서 수행하도록 설계하였기 때문
    
    GameObject::Update(deltaTime);
}



void Character::Render(CDDrawDevice* pDevice)
{
    if (pDevice && m_spriteManager && m_anim)
    {
        // 현재 애니메이션 프레임에 해당하는 srcRect를 SpriteManager에서 가져옴.
        int currentFrame = m_anim->GetCurrentFrame();
        const RECT& srcRect = m_spriteManager->GetFrameRect(currentFrame);

        // 화면상의 렌더링 위치
        Vector2<float> renderPos = GetRenderPosition();
        
        const float FLIP_RENDER_OFFSET_X = -50.0f;

        if (m_forwardVec.x >= 0) {
            pDevice->DrawSprite(static_cast<int>(renderPos.x), static_cast<int>(renderPos.y),
                m_spriteManager->GetSpriteSheet(), srcRect);
        }

        else {
            pDevice->DrawSpriteFlip(static_cast<int>(renderPos.x + FLIP_RENDER_OFFSET_X), static_cast<int>(renderPos.y),
                m_spriteManager->GetSpriteSheet(), srcRect);
        }
    }

    GameObject::Render(pDevice);
}


int Character::GetSpriteFrameWidth() const {
    return m_spriteManager ? m_spriteManager->GetFrameWidth() : 0;
}

int Character::GetSpriteFrameHeight() const {
    return m_spriteManager ? m_spriteManager->GetFrameHeight() : 0;
}

void Character::OnCollision(const CollisionInfo& collisionInfo)
{
    switch (collisionInfo.response) {
    case CollisionResponse::Block:
        break;
    case CollisionResponse::Overlap:
        break;
    default:
        break;
    }
}


void Character::InitCollider(int x, int y, int width, int height)
{
    if (m_pCollider)
        delete m_pCollider;

    Vector2<float> pos = GetTransform().position;
    m_pCollider = new AABBCollider(this, x, y, width, height);
}


void Character::InitSpriteManager(CImageData* spriteSheet, int frameWidth, int frameHeight)
{
    if (m_spriteManager)
        delete m_spriteManager;

    m_spriteManager = new SpriteManager;
    m_spriteManager->SetSpriteSheet(spriteSheet, frameWidth, frameHeight);
}


void Character::InitAnimation()
{
    if (!m_spriteManager)
    {
        DebugManager::GetInstance().AddOnScreenMessage(L"SpriteManager is not initialized in Character::InitAnimation()", 20.0f);
    }

    if (m_anim)
        delete m_anim;

    m_anim = new CharacterAnim;

    std::vector<CharacterAnim::AnimSequence> sequences;

    const float SECONDS_TO_MS = 1000.0f;

    int currentFrame = 0;
    // Idle : 6 frames
    sequences.push_back({ CharacterAnimState::Idle, currentFrame, (currentFrame + 6 - 1), 0.15f * SECONDS_TO_MS });
    currentFrame += 6;

    // Run : 8 frames
    sequences.push_back({ CharacterAnimState::Run, currentFrame, (currentFrame + 8 - 1), 0.1f * SECONDS_TO_MS });
    currentFrame += 8;

    // 2 Combo Attack : 12 frames
    sequences.push_back({ CharacterAnimState::Attack, currentFrame, (currentFrame + 12 - 1), 0.08f * SECONDS_TO_MS });
    currentFrame += 12;

    // Death : 11 frames
    sequences.push_back({ CharacterAnimState::Death, currentFrame, (currentFrame + 11 - 1), 0.2f * SECONDS_TO_MS });
    currentFrame += 11;

    // Hurt : 4 frames
    sequences.push_back({ CharacterAnimState::Hurt, currentFrame, (currentFrame + 4 - 1), 0.1f * SECONDS_TO_MS });
    currentFrame += 4;

    // Jump : 3 frames
    sequences.push_back({ CharacterAnimState::Jump, currentFrame, (currentFrame + 3 - 1), 0.12f * SECONDS_TO_MS });
    currentFrame += 3;

    // UpToFall : 2 frames
    sequences.push_back({ CharacterAnimState::UpToFall, currentFrame, (currentFrame + 2 - 1), 0.15f * SECONDS_TO_MS, false });
    currentFrame += 2;

    // Fall : 3 frames
    sequences.push_back({ CharacterAnimState::Fall, currentFrame, (currentFrame + 3 - 1), 0.15f * SECONDS_TO_MS });
    currentFrame += 3;

    // Edge Grab : 5 frames
    sequences.push_back({ CharacterAnimState::EdgeGrab, currentFrame, (currentFrame + 5 - 1), 0.1f * SECONDS_TO_MS });
    currentFrame += 5;

    // Edge Idle : 6 frames
    sequences.push_back({ CharacterAnimState::EdgeIdle, currentFrame, (currentFrame + 6 - 1), 0.1f * SECONDS_TO_MS });
    currentFrame += 6;

    // WallSlide : 3 frames
    sequences.push_back({ CharacterAnimState::WallSlide, currentFrame, (currentFrame + 3 - 1), 0.1f * SECONDS_TO_MS });
    currentFrame += 3;

    // Crouch : 6 frames
    sequences.push_back({ CharacterAnimState::Crouch, currentFrame, (currentFrame + 6 - 1), 0.1f * SECONDS_TO_MS });
    currentFrame += 6;

    // Dash : 7 frames
    sequences.push_back({ CharacterAnimState::Dash, currentFrame, (currentFrame + 7 - 1), 0.08f * SECONDS_TO_MS });
    currentFrame += 7;

    // Dash-Attack : 10 frames
    sequences.push_back({ CharacterAnimState::DashAttack, currentFrame, (currentFrame + 10 - 1), 0.08f * SECONDS_TO_MS });
    currentFrame += 10;

    // Slide : 5 frames
    sequences.push_back({ CharacterAnimState::Slide, currentFrame, (currentFrame + 5 - 1), 0.1f * SECONDS_TO_MS });
    currentFrame += 5;

    // Ladder-Grab : 8 frames
    sequences.push_back({ CharacterAnimState::LadderGrab, currentFrame, (currentFrame + 8 - 1), 0.1f * SECONDS_TO_MS });
    currentFrame += 8;

    // 애니메이션 시퀀스 목록 설정
    m_anim->SetSequences(sequences);

    m_firstAttackEndFrame = m_anim->GetSequence(CharacterAnimState::Attack)->endFrame - 2;
    m_secondAttackEndFrame = m_firstAttackEndFrame + 2;
    m_DashEndFrame = m_anim->GetSequence(CharacterAnimState::Dash)->endFrame;
    m_DashAttackEndFrame = m_anim->GetSequence(CharacterAnimState::DashAttack)->endFrame;

    // 초기 상태를 Idle로 설정
    m_anim->SetState(CharacterAnimState::Idle);
}



void Character::UpdateAnimFSM()
{
    CharacterAnimState currentState = m_anim->GetState();
    CharacterAnimState newState = currentState;

    // 대시 공격은 대시와 공격보다 우선
    if (m_isDashAttacking) {
        newState = CharacterAnimState::DashAttack;
    }
    // 대시와 공격은 서로 입력 시 서로 캔슬
    else if (m_isDashing) {
        newState = CharacterAnimState::Dash;
    }
    else if (m_isAttacking) {
        newState = CharacterAnimState::Attack;
    }
    else if (!m_isGrounded) {
        if (m_verticalVelocity < 0)
            newState = CharacterAnimState::Jump;
        else
            newState = m_isUpToFall ? CharacterAnimState::UpToFall : CharacterAnimState::Fall;
    }
    else {
        newState = m_isRunning ? CharacterAnimState::Run : CharacterAnimState::Idle;
    }

    if (newState != currentState) {
        m_anim->SetState(newState);
    }
}


void Character::CheckColliderHitFromPlayer(float rayLength)
{
    Collider* curCollider = this->GetCollider();
    if (!curCollider)
        return;

    // 캐릭터 콜라이더가 AABB 타입이라고 가정하고 형변환
    AABBCollider* playerAABB = dynamic_cast<AABBCollider*>(curCollider);
    if (!playerAABB)
        return;

    // 캐릭터 콜라이더 중앙 계산
    int centerX = playerAABB->GetX() + playerAABB->GetWidth() / 2;
    int centerY = playerAABB->GetY() + playerAABB->GetHeight();
    Vector2<float> rayOrigin(static_cast<float>(centerX), static_cast<float>(centerY));

    // 예: 아래쪽으로 ray를 쏜다고 가정 (필요에 따라 방향을 조정)
    Vector2<float> rayDirection(0.0f, 1.0f);
    Ray ray(rayOrigin, rayDirection);

    float hitDistance = 0.0f;
    bool hitFound = false;

    // 모든 콜라이더 순회 (자기 자신은 제외)
    const auto& colliders = ColliderManager::GetInstance().GetColliders();
    for (Collider* collider : colliders)
    {
        if (collider == curCollider)
            continue;

        // 지정한 길이 내에서 ray와 collider의 교차 여부 확인
        if (Raycast::IntersectCollider(ray, collider, hitDistance) && hitDistance <= rayLength)
        {
            // 충돌한 콜라이더가 AABB 타입이라면
            AABBCollider* hitAABB = dynamic_cast<AABBCollider*>(collider);
            if (hitAABB)
            {
                // 소유자 정보 획득
                GameObject* owner = hitAABB->GetOwner();
                // RTTI를 사용하여 소유자 클래스 이름을 얻음 (원하는 방식이 있다면, owner->GetClassName() 같은 커스텀 함수를 사용할 수도 있음)
                std::string typeName = typeid(*owner).name();
                std::wstring ownerName(typeName.begin(), typeName.end());

                // 위치와 너비 정보 획득
                int hitX = hitAABB->GetX();
                int hitY = hitAABB->GetY();
                int width = hitAABB->GetWidth();

                std::wstring debugMsg = L"Hit Collider Owner: " + ownerName +
                    L", Location: (" + std::to_wstring(hitX) + L", " + std::to_wstring(hitY) +
                    L"), Width: " + std::to_wstring(width);

                DebugManager::GetInstance().LogMessage(debugMsg);
                hitFound = true;
                break;
            }
        }
    }

    if (!hitFound)
    {
        DebugManager::GetInstance().LogMessage(L"No collider hit within ray length.");
    }
}