#pragma once
#include "GameObject.h"
#include "CharacterAnim.h"

// 전방 선언
class CImageData;
class SpriteManager;

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

    // 월드 경계 설정 (클램핑용)
    void SetWorldBounds(int width, int height);


    // Component들 초기화 관련 함수
    void InitCollider(int x, int y, int width, int height);
    void InitSpriteManager(CImageData* spriteSheet, int frameWidth, int frameHeight);
    void InitAnimation();

    // 월드 경계 클램핑
    virtual void SetPosition(const Vector2& newPos) override;

    // 렌더링 위치 관련 함수
    // 렌더링 위치와 월드 좌표계 상의 위치는 게임설계에 따라 같을 수도 다를 수도 있음
    void SetRenderPosition(const Vector2& pos);
    Vector2 GetRenderPosition() const;


    int GetSpriteFrameWidth() const;
    int GetSpriteFrameHeight() const;


    virtual void OnCollision(const CollisionInfo& collisionInfo) override;


private:
    void UpdateAnimFSM();


private:
    int m_health;
    int m_speed;
    
    // 애니메이션 관련
    SpriteManager* m_spriteManager;
    CharacterAnim* m_anim;

    // 캐릭터 전용 중력 변수
    float m_verticalVelocity;
    bool m_isGrounded;

    // 월드 경계 (클램핑용)
    int m_worldWidth;
    int m_worldHeight;

    // 렌더링 위치 
    // DirectX 스크린 좌표계상에서의 위치
    Vector2 m_renderPosition;
};
