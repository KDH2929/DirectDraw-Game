#include "stdafx.h"
#include "Centipede.h"
#include "AABBCollider.h"
#include "../ImageData/ImageData.h"
#include "../DDrawLib/DDrawDevice.h"
#include "DebugManager.h"
#include "SpriteManager.h"
#include "CentipedeAnim.h"
#include "InputManager.h"

#include <algorithm>
#include <cmath>

Centipede::Centipede(CImageData* pMonsterImgData, float startX, float startY)
    : m_health(100),
    m_speed(100),
    m_verticalVelocity(0.0f),
    m_renderPosition(Vector2{ 0, 0 }),
    m_spriteManager(nullptr),
    m_anim(nullptr)
{
    SetPosition(Vector2{ startX, startY });
    InitCollider(0, 0, 216, 216);
    InitSpriteManager(pMonsterImgData, 216, 216);
    InitAnimation();
    SetPhysicsType(PhysicsType::Static);

    SetSpeed(0);
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
    const float MS_TO_SEC = 0.001f;

    Vector2 pos = GetTransform().position;
    pos.x += m_speed * deltaTime * MS_TO_SEC;
    SetPosition(pos);

    // Collider ������Ʈ (��ġ ����ȭ)
    if (GetCollider())
        GetCollider()->Update(GetTransform());

    // �ִϸ��̼� ������Ʈ
    if (m_anim) {
        m_anim->Update(deltaTime);
        UpdateAnimFSM();
    }

    GameObject::Update(deltaTime);
}

void Centipede::Render(CDDrawDevice* pDevice) {
    if (pDevice && m_spriteManager && m_anim) {
        // ���� �ִϸ��̼� �����ӿ� �ش��ϴ� srcRect�� SpriteManager���� ������
        int currentFrame = m_anim->GetCurrentFrame();
        const RECT& srcRect = m_spriteManager->GetFrameRect(currentFrame);

        // ȭ����� ������ ��ġ
        Vector2 renderPos = GetRenderPosition();

        pDevice->DrawSprite(static_cast<int>(renderPos.x), static_cast<int>(renderPos.y),
            m_spriteManager->GetSpriteSheet(), srcRect);
    }

    GameObject::Render(pDevice);
}

void Centipede::SetRenderPosition(const Vector2& pos) {
    m_renderPosition = pos;
}

Vector2 Centipede::GetRenderPosition() const {
    return m_renderPosition;
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

void Centipede::InitCollider(int x, int y, int width, int height) {
    if (m_pCollider)
        delete m_pCollider;

    Vector2 pos = GetTransform().position;
    m_pCollider = new AABBCollider(this, x, y, width, height);
}

void Centipede::InitSpriteManager(CImageData* spriteSheet, int frameWidth, int frameHeight) {
    if (m_spriteManager)
        delete m_spriteManager;

    m_spriteManager = new SpriteManager;
    m_spriteManager->SetSpriteSheet(spriteSheet, frameWidth, frameHeight);
}

void Centipede::InitAnimation() {
    if (!m_spriteManager) {
        DebugManager::GetInstance().AddOnScreenMessage(L"SpriteManager is not initialized in Centipede::InitAnimation()", 20.0f);
    }

    if (m_anim)
        delete m_anim;

    m_anim = new CentipedeAnim;

    std::vector<CentipedeAnim::AnimSequence> sequences;
    const float SECONDS_TO_MS = 1000.0f;
    int currentFrame = 0;

    // ������ ������ ��������Ʈ �̹����� ������ ��

    // Attack1: 4 ������
    sequences.push_back({ CentipedeAnimState::Attack1, currentFrame, currentFrame + 3, 0.1f * SECONDS_TO_MS });
    currentFrame += 6;

    // Attack2: 6 ������
    sequences.push_back({ CentipedeAnimState::Attack2, currentFrame, currentFrame + 5, 0.08f * SECONDS_TO_MS });
    currentFrame += 6;

    // Attack3: 6 ������
    sequences.push_back({ CentipedeAnimState::Attack3, currentFrame, currentFrame + 5, 0.08f * SECONDS_TO_MS });
    currentFrame += 6;

    // Attack4: 4 ������
    sequences.push_back({ CentipedeAnimState::Attack4, currentFrame, currentFrame + 3, 0.1f * SECONDS_TO_MS });
    currentFrame += 6;

    // Death: 4 ������
    sequences.push_back({ CentipedeAnimState::Death, currentFrame, currentFrame + 3, 0.15f * SECONDS_TO_MS });
    currentFrame += 6;

    // Hurt: 2 ������
    sequences.push_back({ CentipedeAnimState::Hurt, currentFrame, currentFrame + 1, 0.2f * SECONDS_TO_MS });
    currentFrame += 6;

    // Idle: 4 ������
    sequences.push_back({ CentipedeAnimState::Idle, currentFrame, currentFrame + 3, 0.15f * SECONDS_TO_MS });
    currentFrame += 6;

    // Sneer: 6 ������
    sequences.push_back({ CentipedeAnimState::Sneer, currentFrame, currentFrame + 5, 0.1f * SECONDS_TO_MS });
    currentFrame += 6;

    // Walk: 4 ������
    sequences.push_back({ CentipedeAnimState::Walk, currentFrame, currentFrame + 3, 0.12f * SECONDS_TO_MS });
    currentFrame += 6;

    m_anim->SetSequences(sequences);
    m_anim->SetState(CentipedeAnimState::Idle);
}

void Centipede::UpdateAnimFSM() {
    // ������ FSM ����: ���� �ִϸ��̼� ���¿� ���� ���ǿ� ���� ���� ��ȯ�� ������
    CentipedeAnimState currentAnim = m_anim->GetState();

    switch (currentAnim) {
    case CentipedeAnimState::Idle:
        // ���ǿ� ���� Walk �Ǵ� Attack ���·� ��ȯ ����
        break;
    case CentipedeAnimState::Walk:
        // �߰� ���� ���� ����
        break;
    default:
        break;
    }
}
