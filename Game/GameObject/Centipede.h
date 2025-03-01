#pragma once
#include "Monster.h"
#include "CentipedeAnim.h"  

// 전방 선언
class CImageData;
class SpriteManager;
class Character;

class Centipede : public Monster {
public:
    // 생성자: 시작 좌표와 스프라이트 데이터를 전달함
    Centipede(CImageData* pMonsterImgData, float startX, float startY);
    virtual ~Centipede();

    virtual void Update(float deltaTime) override;
    virtual void Render(CDDrawDevice* pDevice) override;

    
    // Component 초기화 관련 함수들
    void InitCollider(int x, int y, int width, int height);
    void InitSpriteManager(CImageData* spriteSheet, int frameWidth, int frameHeight);
    void InitAnimation();


    int GetHealth() const { return m_health; }
    void SetHealth(int health) { m_health = health; }
    int GetSpeed() const { return m_speed; }
    void SetSpeed(int speed) { m_speed = speed; }

    int GetSpriteFrameWidth() const;
    int GetSpriteFrameHeight() const;

    virtual MonsterType GetMonsterType() const override;
    
    virtual void TakeDamage(const DamageInfo& damageInfo) override;
    
    void Attack();
    void Chase();
    void Dead();


private:

    // 애니메이션 상태 전환 로직 (FSM)
    virtual void UpdateAnimFSM() override;
    virtual void UpdateAI(float deltaTime) override;


private:

    float m_health;

    float m_speed;         // 현재 이동 속도
    float m_baseSpeed;     // 원래의 이동 속도

    bool m_isHurt = false;
    bool m_isDead = false;
    int m_hurtEndFrame = 0;

    // 애니메이션 관련 구성 요소
    SpriteManager* m_spriteManager;
    CentipedeAnim* m_anim;     // CentipedeAnim은 템플릿 기반의 SpriteAnim<CentipedeAnimState>

    // Centipede 전용 변수 (예: 중력, 이동 관련)
    float m_verticalVelocity;
    const float GRAVITY_ACCEL = 0.0015f;

    bool m_isGrounded = false;


    // 맵 경계에서 대기하기 위한 변수
    float m_waitTime = 0.0f;       // 대기 시간 (초)
    float m_currentWaitTimer = 0.0f;
    
    // 공격관련 변수
    float m_attackDelay = 0.0f;     // 초 단위
    float m_attackDelayTimer = 0.0f;
    float m_attackRange = 0.0f;
    bool m_isAttacking = false;
    bool m_attack2OverlapBoxSpawned = false;
    int m_attack2EndFrame = 0;
    
    float m_chaseStopDistance = 0.0f;


    MonsterAIState m_AIState;
    Character* m_chaseTarget;
};
