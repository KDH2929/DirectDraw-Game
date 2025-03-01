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
#include "CollisionQuery.h"
#include "Monster.h"

#include <typeinfo>
#include <algorithm>


Character::Character(CImageData* pPlayerImgData, float startX, float startY)
    : m_health(100.0f), m_speed(8.0f), m_verticalVelocity(0.0f), m_isGrounded(false), 
    m_spriteManager(nullptr), m_anim(nullptr)
{
    SetPosition(Vector2<float>{ startX, startY });
    SetColliderLocalPosition(Vector2<float>{65, 20});
    SetPhysicsType(PhysicsType::Dynamic);
    SetSpeed(8.0f);
    SetHealth(100.0f);
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


void Character::ProcessInput()
{
    if (m_isDead)
        return;

    Vector2<float> pos = GetTransform().position;

    // 좌우 이동 및 달리기 상태 갱신
    if (InputManager::GetInstance().IsKeyDown(VK_LEFT)) {

        m_isRunning = true;

        if (m_leftWallDetected == false)
        {
            pos.x -= GetSpeed();
        }

        if (!m_isDashing && !m_isAttacking)
        {
            m_forwardVec = Vector2<float>(-1.0f, 0.0f);
        }

    }
    if (InputManager::GetInstance().IsKeyDown(VK_RIGHT)) {

        m_isRunning = true;

        if (m_rightWallDetected == false)
        {
            pos.x += GetSpeed();
        }

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


        else if (!m_isAttacking && !m_isDashAttacking) {
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

        DebugManager::GetInstance()->AddOnScreenMessage(L"LeftKey is double preseed", 3.0f);
        m_isAttacking = false;
        m_isDashing = true;
        m_anim->SetState(CharacterAnimState::Dash);
    }


    if (InputManager::GetInstance().IsDoublePressedRight() && !m_isDashing) {
        // 대시 입력은 공격 상태를 캔슬하고 대시 상태로 전환

        DebugManager::GetInstance()->AddOnScreenMessage(L"RightKey is double preseed", 3.0f);
        m_isAttacking = false;
        m_isDashing = true;
        m_anim->SetState(CharacterAnimState::Dash);
    }


    // 점프 입력 처리 (X 키, 땅에 닿은 상태에서만)
    if (InputManager::GetInstance().IsKeyPressed('X') && m_isGrounded) {
        //DebugManager::GetInstance()->AddOnScreenMessage(L"Key X is pressed", 5.0f);
        m_verticalVelocity = JUMP_VELOCITY;
        m_isGrounded = false;
    }

    SetPosition(pos);

}

void Character::Update(float deltaTime)
{
    // Raycast 를 통한 벽 검사
    m_leftWallDetected = CheckLeftWall();
    m_rightWallDetected = CheckRightWall();

    
    // Racycast를 통한 지면검사
    float groundHitDistance = 0.0f;
    m_isGrounded = CheckGround(groundHitDistance);
    

    // 매 프레임 달리기 상태 초기화
    m_isRunning = false;


    ProcessInput();


    Vector2<float> pos = GetTransform().position;


    // 버그 소지가 있을 부분들은 아래와 같이 꼼꼼히 처리할 것
    // 벽이 있는 상태에서 대시를 해도 어차피 밀려나기에 문제는 없으나 잠깐 동안 살짝 벽에 끼는 문제가 있음 
    if (m_isDashing && !m_leftWallDetected && !m_rightWallDetected)
    {
        pos.x += m_forwardVec.x * m_dashSpeed;
    }


    // '떨어지기 직전' 상태 업데이트: 공중 상태이며 하강 중일 때, raycast hitDistance가 일정 범위 내이면 true
    const float upToFallThreshold = 60.0f;
    if (!m_isGrounded && m_verticalVelocity > 0) {
        m_isUpToFall = (groundHitDistance <= upToFallThreshold);
    }
    else {
        m_isUpToFall = false;
    }




    // 중력 적용 (땅에 닿지 않은 경우)
    if (!m_isGrounded) {
        m_verticalVelocity += GRAVITY_ACCEL * deltaTime;
        pos.y += m_verticalVelocity * deltaTime;
    }

    else {

        // DebugManager::GetInstance()->AddOnScreenMessage(L"Player is on ground", 5.0f);
        m_verticalVelocity = 0;
    }

    SetPosition(pos);


    // 애니메이션 업데이트
    if (m_anim)
    {
        m_anim->Update(deltaTime);
        UpdateAnimFSM();
    }




    // 공격 애니메이션 상태에서 콤보 처리
    if (m_isAttacking && m_anim->GetState() == CharacterAnimState::Attack) {

        // 첫번째 연속공격이 끝나기전 3프레임에 공격판정 수행
        if (!m_firstAttackOverlapBoxSpawned && m_anim->GetCurrentFrame() == m_firstAttackEndFrame - 3)
        {
            Attack(CharacterAttackType::ComboAttack1);

            m_firstAttackOverlapBoxSpawned = true;
        }

        // 현재 공격 애니메이션의 종료 프레임에 도달했으면
        if (m_anim->GetCurrentFrame() >= m_currentAttackEndFrame) {
            if (m_comboInputReceived) {
                // 추가 입력이 있었으면 다음 콤보 단계로 전환
                m_attackComboStage++;
      
                if (m_attackComboStage == 2) {
                    m_currentAttackEndFrame = m_secondAttackEndFrame;
                    m_comboInputReceived = false;

                    Attack(CharacterAttackType::ComboAttack2);
                }

                else if (m_attackComboStage >= 3)
                {
                    // 연속공격종료
                    m_isAttacking = false;
                    m_comboInputReceived = false;
                    m_attackComboStage = 0;
                    m_currentAttackEndFrame = m_firstAttackEndFrame;
                    m_firstAttackOverlapBoxSpawned = false;
                }
                

                // 추가 콤보 단계가 더 있다면 else if (m_attackComboStage == 3) ... 등을 추가
            }

            else {
                // 추가 입력이 없으면 공격 종료
                m_isAttacking = false;
                m_comboInputReceived = false;
                m_attackComboStage = 0;
                m_currentAttackEndFrame = m_firstAttackEndFrame;
                m_firstAttackOverlapBoxSpawned = false;
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
        // 대시공격이 끝나기전 5프레임에 공격판정 수행
        if (!m_dashAttackOverlapBoxSpawned && m_anim->GetCurrentFrame() == m_DashAttackEndFrame - 5)
        {
            Attack(CharacterAttackType::DashAttack);

            m_dashAttackOverlapBoxSpawned = true;
        }

        if (m_anim->GetCurrentFrame() >= m_DashAttackEndFrame) {
            m_isDashAttacking = false;
            m_isAttacking = false;
            m_isDashing = false;

            m_dashAttackOverlapBoxSpawned = false;
        }
    }

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
        

        if (GetForwardVector().x >= 0) {
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

void Character::SetCameraOffset(Vector2<float> cameraOffset)
{
    m_cameraOffset = cameraOffset;
}

Vector2<float> Character::GetCameraOffset() const
{
    return m_cameraOffset;
}

Vector2<float> Character::GetForwardVector() const
{
    return m_forwardVec;
}

bool Character::CheckLeftWall()
{
    // 벽 Raycast 검사를 위한 설정
    const float wallCheckDistance = 0.3f;
    float wallHitDistance = 0.0f;
    const float wallCheckRayOffsetY = 5.0f;

    Collider* curCollider = GetCollider();

    if (curCollider) {
        if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(curCollider)) {
            // 플레이어 콜라이더의 경계 구하기
            int left = aabb->GetX();
            int right = aabb->GetX() + aabb->GetWidth();
            int top = aabb->GetY();
            int bottom = aabb->GetY() + aabb->GetHeight();

            Vector2<float> topLeft(static_cast<float>(left), static_cast<float>(top) + wallCheckRayOffsetY);
            Vector2<float> bottomLeft(static_cast<float>(left), static_cast<float>(bottom) - wallCheckRayOffsetY);


            // 좌측의 경우, 좌상단과 좌하단에서 왼쪽으로 Ray를 쏨
            Ray leftRayFromTop(topLeft, Vector2<float>(-1.0f, 0.0f));
            Ray leftRayFromBottom(bottomLeft, Vector2<float>(-1.0f, 0.0f));

            const auto& colliders = ColliderManager::GetInstance()->GetAllColliders();

            // 자기자신 Collider는 무시
            CollisionLayer queryMask = DEFAULT_COLLISION_LAYER_MASK & ~CHARACTER_COLLISION_LAYER;

            // Overlap 객체도 무시
            queryMask = queryMask & ~OVERLAP_COLLISION_LAYER;

            for (Collider* collider : colliders) {
                if (collider == curCollider)
                    continue;

                if (collider->GetOwner()->GetPhysicsType() == PhysicsType::Dynamic)
                {
                    //continue;
                }

                if (CollisionQuery::Raycast(leftRayFromTop, collider, wallCheckDistance, wallHitDistance, queryMask)) {
                    return true;
                }

                if (CollisionQuery::Raycast(leftRayFromBottom, collider, wallCheckDistance, wallHitDistance, queryMask)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Character::CheckRightWall()
{
    Collider* curCollider = GetCollider();

    const float wallCheckDistance = 0.3f;
    float wallHitDistance = 0.0f;
    const float wallCheckRayOffsetY = 5.0f;


    if (curCollider) {
        if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(curCollider)) {
            // 플레이어 콜라이더의 경계 구하기
            int left = aabb->GetX();
            int right = aabb->GetX() + aabb->GetWidth();
            int top = aabb->GetY();
            int bottom = aabb->GetY() + aabb->GetHeight();


            Vector2<float> topRight(static_cast<float>(right), static_cast<float>(top) + wallCheckRayOffsetY);
            Vector2<float> bottomRight(static_cast<float>(right), static_cast<float>(bottom) - wallCheckRayOffsetY);


            Ray rightRayFromTop(topRight, Vector2<float>(1.0f, 0.0f));
            Ray rightRayFromBottom(bottomRight, Vector2<float>(1.0f, 0.0f));


            const auto& colliders = ColliderManager::GetInstance()->GetAllColliders();

            // 자기자신 Collider는 무시
            CollisionLayer queryMask = DEFAULT_COLLISION_LAYER_MASK & ~CHARACTER_COLLISION_LAYER;

            // Overlap 객체도 무시
            queryMask = queryMask & ~OVERLAP_COLLISION_LAYER;

            for (Collider* collider : colliders) {
                if (collider == curCollider)
                    continue;

                if (collider->GetOwner()->GetPhysicsType() == PhysicsType::Dynamic)
                {
                    //continue;
                }


                if (CollisionQuery::Raycast(rightRayFromTop, collider, wallCheckDistance, wallHitDistance, queryMask)) {
                    return true;
                }

                if (CollisionQuery::Raycast(rightRayFromBottom, collider, wallCheckDistance, wallHitDistance, queryMask)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Character::CheckGround(float& groundHitDistance)
{
    // Raycast를 통한 지면 판정 (AABB 콜라이더 기준, 양쪽 하단에서 검사)
    const float groundCheckRayOffsetX = 5.0f;
    const float groundThreshold = 0.3f;

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
            CollisionLayer queryMask = DEFAULT_COLLISION_LAYER_MASK & ~CHARACTER_COLLISION_LAYER;

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

void Character::TakeDamage(const DamageInfo& damageInfo)
{
    int currentHP = GetHealth();
    SetHealth(currentHP - damageInfo.damageAmount);

    CharacterAnimState currentState = m_anim->GetState();

    if (currentState != CharacterAnimState::DashAttack) {
        m_anim->SetState(CharacterAnimState::Hurt);
        m_isHurt = true;
    }


    if (GetHealth() <= 0) {
        Dead();
    }
}

void Character::Attack(CharacterAttackType attackType)
{

    float attackDamage = 0.0f;

    int attackTypeNum = static_cast<int>(attackType);

    DebugManager::GetInstance()->AddOnScreenMessage(L"AttackType: " + (std::to_wstring(attackTypeNum)), 0.8f);

    AABBCollider* curCollider = static_cast<AABBCollider*>(this->GetCollider());

    Vector2<float> ColliderCenterPos = curCollider->GetCenterPosition();

    // 공격 타입에 따른 히트박스(OBB) 파라미터
    Vector2<float> boxCenter;
    float boxWidth = 0.0f, boxHeight = 0.0f, boxRotation = 0.0f;        // rotation: 도 단위

    switch (attackType) {
    case CharacterAttackType::ComboAttack1:
        boxCenter = ColliderCenterPos + Vector2<float>{100.0f * GetForwardVector().x, -30.0f};
        boxWidth = 120.0f;
        boxHeight = 110.0f;
        boxRotation = -30.0f * GetForwardVector().x;
        attackDamage = 10.0f;
        break;

    case CharacterAttackType::ComboAttack2:
        boxCenter = ColliderCenterPos + Vector2<float>{100.0f * GetForwardVector().x, -30.0f};
        boxWidth = 125.0f;
        boxHeight = 112.0f;
        boxRotation = 30.0f * GetForwardVector().x;
        attackDamage = 10.0f;
        break;

    case CharacterAttackType::DashAttack:
        boxCenter = ColliderCenterPos + Vector2<float>{200.0f * GetForwardVector().x, 0.0f};
        boxWidth = 200.0f;
        boxHeight = 115.0f;
        boxRotation = 0.0f * GetForwardVector().x;
        attackDamage = 20.0f;
        break;

    default:
        break;
    }

    DamageInfo damageInfo(attackDamage, this, boxCenter);


    DebugBox debugBox(boxCenter, boxWidth, boxHeight, boxRotation, RGB(255, 0, 0), 2.0f);
    DebugManager::GetInstance()->AddDebugBox(debugBox);


    // 자기자신의 Collider는 무시
    CollisionLayer queryMask = DEFAULT_COLLISION_LAYER_MASK & ~CHARACTER_COLLISION_LAYER;

    const auto& colliders = ColliderManager::GetInstance()->GetAllColliders();
    for (Collider* collider : colliders) {

        if (CollisionQuery::OverlapBox(boxCenter, boxWidth, boxHeight, boxRotation, collider, queryMask)) {
            GameObject* targetObject = collider->GetOwner();
            if (targetObject) {
                Monster* monster = dynamic_cast<Monster*>(targetObject);
                if (monster) {
                    monster->TakeDamage(damageInfo);
                }
            }
        }
    }
}

void Character::Dead()
{
    if (m_isDead)
        return;

    m_isDead = true;

    m_anim->SetState(CharacterAnimState::Death);
    GetCollider()->SetCollisionResponse(CollisionResponse::Ignore);
    DebugManager::GetInstance()->AddOnScreenMessage(L"Character Dead!", 2.0f);
}


void Character::InitCollider(float x, float y, float width, float height)
{
    if (m_pCollider)
        delete m_pCollider;

    Vector2<float> pos = GetTransform().position;
    m_pCollider = new AABBCollider(this, x, y, width, height);
    m_pCollider->SetCollisionLayer(CHARACTER_COLLISION_LAYER);
    ColliderManager::GetInstance()->AddCollider(GetCollider());
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
        DebugManager::GetInstance()->AddOnScreenMessage(L"SpriteManager is not initialized in Character::InitAnimation()", 20.0f);
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
    sequences.push_back({ CharacterAnimState::Death, currentFrame, (currentFrame + 11 - 1), 0.1f * SECONDS_TO_MS, false });
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

    m_firstAttackEndFrame = m_anim->GetSequence(CharacterAnimState::Attack)->endFrame - 3;
    m_secondAttackEndFrame = m_firstAttackEndFrame + 2;
    m_DashEndFrame = m_anim->GetSequence(CharacterAnimState::Dash)->endFrame;
    m_DashAttackEndFrame = m_anim->GetSequence(CharacterAnimState::DashAttack)->endFrame;
    m_hurtEndFrame = m_anim->GetSequence(CharacterAnimState::Hurt)->endFrame;

    // 초기 상태를 Idle로 설정
    m_anim->SetState(CharacterAnimState::Idle);
}



void Character::UpdateAnimFSM()
{
    CharacterAnimState currentState = m_anim->GetState();
    CharacterAnimState newState = currentState;

    // 사망했을 시 무조건 Death 상태
    if (m_isDead) {
        m_anim->SetState(CharacterAnimState::Death);
        return;
    }

    if (m_isHurt) {
        if (m_anim->GetCurrentFrame() >= m_hurtEndFrame) {
            m_isHurt = false;
        }
        else {
            m_anim->SetState(CharacterAnimState::Hurt);
            return;
        }
    }



    // 대시 공격은 대시와 공격보다 우선
    if (m_isDashAttacking) {
        newState = CharacterAnimState::DashAttack;
    }
    // 대시와 공격은 서로 입력 시 서로 캔슬하도록 Update에서 설계됨
    else if (m_isDashing) {
        newState = CharacterAnimState::Dash;
    }
    else if (m_isAttacking) {
        newState = CharacterAnimState::Attack;
    }
    else if (!m_isGrounded) {
        if (m_verticalVelocity < 0)         // 위로 올라가는 중
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