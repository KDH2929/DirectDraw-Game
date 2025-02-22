#include "stdafx.h"
#include <algorithm>
#include "Character.h"
#include "GameObject.h"
#include "AABBCollider.h"
#include "../ImageData/ImageData.h"
#include "../DDrawLib/DDrawDevice.h"
#include "InputManager.h"
#include "DebugManager.h"
#include "SpriteManager.h"
#include "CharacterAnim.h"


Character::Character(CImageData* pPlayerImgData, float startX, float startY)
    : m_health(100), m_speed(8), m_verticalVelocity(0.0f), 
    m_worldWidth(0), m_worldHeight(0), m_renderPosition(Vector2{0,0}),
    m_spriteManager(nullptr), m_anim(nullptr)
{
    SetPosition(Vector2{ startX, startY });
    SetSpeed(8);
    InitCollider(50, 50, 200, 200);
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
    // ���� ��ġ�� ������
    Vector2 pos = GetTransform().position;

    if (InputManager::GetInstance().IsKeyDown(VK_LEFT))
        pos.x -= GetSpeed();

    if (InputManager::GetInstance().IsKeyDown(VK_RIGHT))
        pos.x += GetSpeed();

    if (InputManager::GetInstance().IsKeyDown(VK_UP))
        pos.y -= GetSpeed();

    if (InputManager::GetInstance().IsKeyDown(VK_DOWN))
        pos.y += GetSpeed();


    
    // �߷� ����
    const float GRAVITY_ACCEL = 0.0015f;
    m_verticalVelocity += GRAVITY_ACCEL * deltaTime;
    pos.y += m_verticalVelocity * deltaTime;
    

    // SetPosition�� ���� ������Ʈ�� ��ġ�� Ȯ������ Clampó��
    SetPosition(pos);

    Transform updatedTransform = GetTransform();
    updatedTransform.position = pos;
    if (GetCollider())
        GetCollider()->Update(updatedTransform);

    if (m_anim) {
        m_anim->Update(deltaTime);
    }

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
        Vector2 renderPos = GetRenderPosition();

        pDevice->DrawSprite(static_cast<int>(renderPos.x), static_cast<int>(renderPos.y), m_spriteManager->GetSpriteSheet(), srcRect);
    }

    GameObject::Render(pDevice);
}

// SetPosition: ���� ��� Ŭ����
void Character::SetPosition(const Vector2& newPos)
{
    Vector2 clampedPos = newPos;
    if (m_worldWidth > 0 && m_worldHeight > 0)
    {
        clampedPos.x = std::max<float>(0.0f, std::min<float>(newPos.x, static_cast<float>(m_worldWidth)));
        clampedPos.y = std::max<float>(0.0f, std::min<float>(newPos.y, static_cast<float>(m_worldHeight)));
    }

    GameObject::SetPosition(clampedPos);
}

void Character::SetWorldBounds(int width, int height)
{
    m_worldWidth = width;
    m_worldHeight = height;
}

void Character::SetRenderPosition(const Vector2& pos)
{
    m_renderPosition = pos;
}

Vector2 Character::GetRenderPosition() const
{
    return m_renderPosition;
}

int Character::GetSpriteFrameWidth() const {
    return m_spriteManager ? m_spriteManager->GetFrameWidth() : 0;
}

int Character::GetSpriteFrameHeight() const {
    return m_spriteManager ? m_spriteManager->GetFrameHeight() : 0;
}

void Character::OnCollision(GameObject* other, CollisionResponse response)
{

}


void Character::InitCollider(int x, int y, int width, int height)
{
    if (m_pCollider)
        delete m_pCollider;

    Vector2 pos = GetTransform().position;
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
    sequences.push_back({ CharacterAnimState::UpToFall, currentFrame, (currentFrame + 2 - 1), 0.12f * SECONDS_TO_MS });
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

    // �ʱ� ���¸� Idle�� ����
    m_anim->SetState(CharacterAnimState::Run);
}



void Character::UpdateAnimFSM()
{
    // ���� �ִϸ��̼� ���¸� ������
    CharacterAnimState currentAnim = m_anim->GetState();

    switch (currentAnim)
    {
    case CharacterAnimState::Idle:
    {
        // �Է¿� ���� Run �Ǵ� Jump�� ��ȯ
        if (InputManager::GetInstance().IsKeyDown(VK_SPACE))
        {
            m_anim->SetState(CharacterAnimState::Jump);
        }
        else if (InputManager::GetInstance().IsKeyDown(VK_LEFT) ||
            InputManager::GetInstance().IsKeyDown(VK_RIGHT))
        {
            m_anim->SetState(CharacterAnimState::Run);
        }
        break;
    }

    case CharacterAnimState::Run:
    {
        // �޸��� ���� ���� �Է��� ������ Jump�� ��ȯ
        if (InputManager::GetInstance().IsKeyDown(VK_SPACE))
        {
            m_anim->SetState(CharacterAnimState::Jump);
        }
        // �¿� �Է��� ������ Idle�� ��ȯ
        else if (!InputManager::GetInstance().IsKeyDown(VK_LEFT) &&
            !InputManager::GetInstance().IsKeyDown(VK_RIGHT))
        {
            m_anim->SetState(CharacterAnimState::Idle);
        }
        break;
    }

    case CharacterAnimState::Jump:
    {
        // ��� �� m_verticalVelocity�� 0�� �Ѿ� (��, �ϰ� ����)�ϸ� Fall�� ��ȯ
        if (m_verticalVelocity > 0.0f)
        {
            m_anim->SetState(CharacterAnimState::Fall);
        }
        break;
    }

    case CharacterAnimState::Fall:
    {
        // ���� ������� (��, IsOnGround()�� true) Idle �Ǵ� Run���� ��ȯ
        if (true)       // �ϴ� true�� ó��
        {
            if (InputManager::GetInstance().IsKeyDown(VK_LEFT) ||
                InputManager::GetInstance().IsKeyDown(VK_RIGHT))
            {
                m_anim->SetState(CharacterAnimState::Run);
            }
            else
            {
                m_anim->SetState(CharacterAnimState::Idle);
            }
        }
        break;
    }

    // �ٸ� ���µ�(Attack, Hurt ��)�� �ʿ信 ���� ó�� ����
    default:
        break;
    }
}