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

    // �¿� �̵� �� �޸��� ���� ����
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

    // ���� �Է� ó�� (C Ű)
    if (InputManager::GetInstance().IsKeyPressed('C')) {

        // ���� ��� ���̸� ���� �Է��� ��ø� ĵ���ϰ� ���� ��ȯ
        if (m_isDashing && !m_isDashAttacking) {
            m_isDashAttacking = true;
            m_anim->SetState(CharacterAnimState::DashAttack);
        }


        if (!m_isAttacking) {
            // ó�� ���� �Է�: ù ���� ����
            m_isAttacking = true;
            m_attackComboStage = 1;
            m_currentAttackEndFrame = m_firstAttackEndFrame;
            m_comboInputReceived = false;
            m_anim->SetState(CharacterAnimState::Attack);
        }
        else {
            // �̹� ���� ���̸� �߰� �Է� �÷��� ����
            m_comboInputReceived = true;
        }

    }


    if (InputManager::GetInstance().IsDoublePressedLeft() && !m_isDashing) {
        // ��� �Է��� �������¸� ĵ���ϰ� ��� ���·� ��ȯ

        DebugManager::GetInstance().AddOnScreenMessage(L"LeftKey is double preseed", 3.0f);
        m_isAttacking = false;
        m_isDashing = true;
        m_anim->SetState(CharacterAnimState::Dash);
    }


    if (InputManager::GetInstance().IsDoublePressedRight() && !m_isDashing) {
        // ��� �Է��� ���� ���¸� ĵ���ϰ� ��� ���·� ��ȯ

        DebugManager::GetInstance().AddOnScreenMessage(L"RightKey is double preseed", 3.0f);
        m_isAttacking = false;
        m_isDashing = true;
        m_anim->SetState(CharacterAnimState::Dash);
    }


    if (m_isDashing)
    {
        pos.x += m_forwardVec.x * m_dashSpeed;
    }


    // Raycast�� ���� ���� ���� (AABB �ݶ��̴� ����, ���� �ϴܿ��� �˻�)
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


    // '�������� ����' ���� ������Ʈ: ���� �����̸� �ϰ� ���� ��, raycast hitDistance�� ���� ���� ���̸� true
    const float upToFallThreshold = 60.0f;
    if (!m_isGrounded && m_verticalVelocity > 0) {
        m_isUpToFall = (hitDistance <= upToFallThreshold);
    }
    else {
        m_isUpToFall = false;
    }



    // ���� �Է� ó�� (X Ű, ���� ���� ���¿�����)
    if (InputManager::GetInstance().IsKeyPressed('X') && m_isGrounded) {
        //DebugManager::GetInstance().AddOnScreenMessage(L"Key X is pressed", 5.0f);
        m_verticalVelocity = JUMP_VELOCITY;
        m_isGrounded = false;
    }



    // �߷� ���� (���� ���� ���� ���)
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



    // ���� �ִϸ��̼� ���¿��� �޺� ó��
    if (m_isAttacking && m_anim->GetState() == CharacterAnimState::Attack) {
        // ���� ���� �ִϸ��̼��� ���� �����ӿ� ����������
        if (m_anim->GetCurrentFrame() >= m_currentAttackEndFrame) {
            if (m_comboInputReceived) {
                // �߰� �Է��� �־����� ���� �޺� �ܰ�� ��ȯ
                m_attackComboStage++;
      
                if (m_attackComboStage == 2) {
                    m_currentAttackEndFrame = m_secondAttackEndFrame;
                    m_comboInputReceived = false;
                }

                else if (m_attackComboStage >= 3)
                {
                    // ���Ӱ�������
                    m_isAttacking = false;
                    m_comboInputReceived = false;
                    m_attackComboStage = 0;
                    m_currentAttackEndFrame = m_firstAttackEndFrame;
                }
                

                // �߰� �޺� �ܰ谡 �� �ִٸ� else if (m_attackComboStage == 3) ... ���� �߰�
            }

            else {
                // �߰� �Է��� ������ ���� ����
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

    // �ڽ�Ŭ�������� �θ�Ŭ������ Update�� �������� ȣ���ϴ� ������ ������ġ�� ���� ��, 
    // ��ġ������ �θ�Ŭ�������� �����ϵ��� �����Ͽ��� ����
    
    GameObject::Update(deltaTime);
}



void Character::Render(CDDrawDevice* pDevice)
{
    if (pDevice && m_spriteManager && m_anim)
    {
        // ���� �ִϸ��̼� �����ӿ� �ش��ϴ� srcRect�� SpriteManager���� ������.
        int currentFrame = m_anim->GetCurrentFrame();
        const RECT& srcRect = m_spriteManager->GetFrameRect(currentFrame);

        // ȭ����� ������ ��ġ
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

    // �ִϸ��̼� ������ ��� ����
    m_anim->SetSequences(sequences);

    m_firstAttackEndFrame = m_anim->GetSequence(CharacterAnimState::Attack)->endFrame - 2;
    m_secondAttackEndFrame = m_firstAttackEndFrame + 2;
    m_DashEndFrame = m_anim->GetSequence(CharacterAnimState::Dash)->endFrame;
    m_DashAttackEndFrame = m_anim->GetSequence(CharacterAnimState::DashAttack)->endFrame;

    // �ʱ� ���¸� Idle�� ����
    m_anim->SetState(CharacterAnimState::Idle);
}



void Character::UpdateAnimFSM()
{
    CharacterAnimState currentState = m_anim->GetState();
    CharacterAnimState newState = currentState;

    // ��� ������ ��ÿ� ���ݺ��� �켱
    if (m_isDashAttacking) {
        newState = CharacterAnimState::DashAttack;
    }
    // ��ÿ� ������ ���� �Է� �� ���� ĵ��
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

    // ĳ���� �ݶ��̴��� AABB Ÿ���̶�� �����ϰ� ����ȯ
    AABBCollider* playerAABB = dynamic_cast<AABBCollider*>(curCollider);
    if (!playerAABB)
        return;

    // ĳ���� �ݶ��̴� �߾� ���
    int centerX = playerAABB->GetX() + playerAABB->GetWidth() / 2;
    int centerY = playerAABB->GetY() + playerAABB->GetHeight();
    Vector2<float> rayOrigin(static_cast<float>(centerX), static_cast<float>(centerY));

    // ��: �Ʒ������� ray�� ��ٰ� ���� (�ʿ信 ���� ������ ����)
    Vector2<float> rayDirection(0.0f, 1.0f);
    Ray ray(rayOrigin, rayDirection);

    float hitDistance = 0.0f;
    bool hitFound = false;

    // ��� �ݶ��̴� ��ȸ (�ڱ� �ڽ��� ����)
    const auto& colliders = ColliderManager::GetInstance().GetColliders();
    for (Collider* collider : colliders)
    {
        if (collider == curCollider)
            continue;

        // ������ ���� ������ ray�� collider�� ���� ���� Ȯ��
        if (Raycast::IntersectCollider(ray, collider, hitDistance) && hitDistance <= rayLength)
        {
            // �浹�� �ݶ��̴��� AABB Ÿ���̶��
            AABBCollider* hitAABB = dynamic_cast<AABBCollider*>(collider);
            if (hitAABB)
            {
                // ������ ���� ȹ��
                GameObject* owner = hitAABB->GetOwner();
                // RTTI�� ����Ͽ� ������ Ŭ���� �̸��� ���� (���ϴ� ����� �ִٸ�, owner->GetClassName() ���� Ŀ���� �Լ��� ����� ���� ����)
                std::string typeName = typeid(*owner).name();
                std::wstring ownerName(typeName.begin(), typeName.end());

                // ��ġ�� �ʺ� ���� ȹ��
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