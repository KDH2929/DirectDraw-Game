#pragma once
#include "Monster.h"
#include "CentipedeAnim.h"  

// 전방 선언
class CImageData;
class SpriteManager;

class Centipede : public Monster {
public:
    // 생성자: 시작 좌표와 스프라이트 데이터를 전달함
    Centipede(CImageData* pMonsterImgData, float startX, float startY);
    virtual ~Centipede();

    // 매 프레임 업데이트
    virtual void Update(float deltaTime) override;
    // 매 프레임 렌더링
    virtual void Render(CDDrawDevice* pDevice) override;

    int GetHealth() const { return m_health; }
    void SetHealth(int health) { m_health = health; }
    int GetSpeed() const { return m_speed; }
    void SetSpeed(int speed) { m_speed = speed; }

    // 렌더링 위치 관련 함수
    void SetRenderPosition(const Vector2& pos);
    Vector2 GetRenderPosition() const;

    int GetSpriteFrameWidth() const;
    int GetSpriteFrameHeight() const;

    virtual MonsterType GetMonsterType() const override;

    // SetPosition는 GameObject의 것을 그대로 사용
    virtual void SetPosition(const Vector2& newPos) override {
        GameObject::SetPosition(newPos);
    }

private:
    // Component 초기화 관련 함수들
    void InitCollider(int x, int y, int width, int height);
    void InitSpriteManager(CImageData* spriteSheet, int frameWidth, int frameHeight);
    void InitAnimation();

    // 애니메이션 상태 전환 로직 (FSM)
    void UpdateAnimFSM();

private:
    int m_health;
    int m_speed;

    // 애니메이션 관련 구성 요소
    SpriteManager* m_spriteManager;
    CentipedeAnim* m_anim;     // CentipedeAnim은 템플릿 기반의 SpriteAnim<CentipedeAnimState>

    // Centipede 전용 변수 (예: 중력, 이동 관련)
    float m_verticalVelocity;

    // 렌더링 위치 (DirectX 스크린 좌표계 상)
    Vector2 m_renderPosition;
};
