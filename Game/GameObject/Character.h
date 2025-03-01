#pragma once
#include "GameObject.h"
#include "CharacterAnim.h"


enum class CharacterAttackType {
    ComboAttack1,   // 연속공격1 (C키 한 번)
    ComboAttack2,   // 연속공격2 (C키 두 번)
    DashAttack      // 대시공격
};
    

// 전방 선언
class CImageData;
class SpriteManager;


// 캐릭터 클래스의 현재 로컬좌표계의 원점은 이미지의 좌상단

class Character : public GameObject {
public:
    Character(CImageData* pPlayerImgData, float startX, float startY);
    virtual ~Character();

    virtual void ProcessInput();
    virtual void Update(float deltaTime) override;
    virtual void Render(CDDrawDevice* pDevice) override;

    float GetHealth() const { return m_health; }
    float GetSpeed() const { return m_speed; }
    void SetHealth(float health) { m_health = health; }
    void SetSpeed(float speed) { m_speed = speed; }


    // Component들 초기화 관련 함수
    void InitCollider(float x, float y, float width, float height);
    void InitSpriteManager(CImageData* spriteSheet, int frameWidth, int frameHeight);
    void InitAnimation();


    int GetSpriteFrameWidth() const;
    int GetSpriteFrameHeight() const;

    void SetCameraOffset(Vector2<float> cameraOffset);
    Vector2<float> GetCameraOffset() const;
    Vector2<float> GetForwardVector() const;

    bool CheckLeftWall();
    bool CheckRightWall();
    bool CheckGround(float& GroundHitDistance);     // GroundHitDistance 값을 참조자 매개변수를 통해 얻을 수 있음

    virtual void OnCollision(const CollisionInfo& collisionInfo) override;
    virtual void TakeDamage(const DamageInfo& damageInfo) override;

    void Attack(CharacterAttackType attackType);
    void Dead();


private:
    void UpdateAnimFSM();



private:
   
    float m_health;
    float m_speed;

    bool m_isHurt = false;
    int m_hurtEndFrame = 0;
    bool m_isDead = false;

    float m_deathTimer = 0.0f;
    const float GAME_OVER_DELAY = 3.5f;
    const float SECOND_TO_MS = 1000.0f;

    Vector2<float> m_forwardVec = Vector2<float>(1.0f, 0.0f);
    Vector2<float> m_cameraOffset = Vector2<float>(0.0f, 0.0f);

    bool m_leftWallDetected = false;
    bool m_rightWallDetected = false;
    
    // 애니메이션 관련
    SpriteManager* m_spriteManager;
    CharacterAnim* m_anim;

    bool m_isUpToFall = false;
    bool m_isRunning= false;

    // 대시 처리
    bool m_isDashing = false;
    int m_DashEndFrame = 0;
    const float m_dashSpeed = 6.0f;
    bool m_isDashAttacking = false;
    int m_DashAttackEndFrame = 0;
    bool m_dashAttackOverlapBoxSpawned = false;


    // 연속공격 처리
    bool m_isAttacking = false;
    bool m_comboInputReceived = false;
    int m_attackComboStage = 0;
    int m_currentAttackEndFrame = 0;
    int m_firstAttackEndFrame = 0;
    int m_secondAttackEndFrame = 0;

    bool m_firstAttackOverlapBoxSpawned = false;


    // 캐릭터 전용 중력 변수
    float m_verticalVelocity;
    bool m_isGrounded;
    const float JUMP_VELOCITY = -0.8f;
    const float GRAVITY_ACCEL = 0.0015f;


};
