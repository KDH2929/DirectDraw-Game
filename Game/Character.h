#pragma once
#include "GameObject.h"
#include "CharacterAnim.h"

// 전방 선언
class CImageData;
class SpriteManager;


// 캐릭터 클래스의 현재 로컬좌표계의 원점은 이미지의 좌상단

class Character : public GameObject {
public:
    Character(CImageData* pPlayerImgData, float startX, float startY);
    virtual ~Character();

    virtual void Update(float deltaTime) override;
    virtual void Render(CDDrawDevice* pDevice) override;

    int GetHealth() const { return m_health; }
    void SetHealth(int health) { m_health = health; }
    int GetSpeed() const { return m_speed; }
    void SetSpeed(int speed) { m_speed = speed; }


    // Component들 초기화 관련 함수
    void InitCollider(int x, int y, int width, int height);
    void InitSpriteManager(CImageData* spriteSheet, int frameWidth, int frameHeight);
    void InitAnimation();


    int GetSpriteFrameWidth() const;
    int GetSpriteFrameHeight() const;


    virtual void OnCollision(const CollisionInfo& collisionInfo) override;

    void Character::CheckColliderHitFromPlayer(float rayLength);


private:
    void UpdateAnimFSM();


private:
   
    int m_health;
    int m_speed;

    Vector2<float> m_forwardVec = Vector2<float>(1.0f, 0.0f);
    
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


    // 연속공격 처리
    bool m_isAttacking = false;
    bool m_comboInputReceived = false;
    int m_attackComboStage = 0;
    int m_currentAttackEndFrame = 0;
    int m_firstAttackEndFrame = 0;
    int m_secondAttackEndFrame = 0;



    // 캐릭터 전용 중력 변수
    float m_verticalVelocity;
    bool m_isGrounded;
    const float JUMP_VELOCITY = -0.8f;
    const float GRAVITY_ACCEL = 0.0015f;

};
