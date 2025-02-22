#pragma once
#include "GameObject.h"
#include "CharacterAnim.h"

// ���� ����
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

    // ���� ��� ���� (Ŭ���ο�)
    void SetWorldBounds(int width, int height);


    // Component�� �ʱ�ȭ ���� �Լ�
    void InitCollider(int x, int y, int width, int height);
    void InitSpriteManager(CImageData* spriteSheet, int frameWidth, int frameHeight);
    void InitAnimation();

    // ���� ��� Ŭ����
    virtual void SetPosition(const Vector2& newPos) override;

    // ������ ��ġ ���� �Լ�
    // ������ ��ġ�� ���� ��ǥ�� ���� ��ġ�� ���Ӽ��迡 ���� ���� ���� �ٸ� ���� ����
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
    
    // �ִϸ��̼� ����
    SpriteManager* m_spriteManager;
    CharacterAnim* m_anim;

    // ĳ���� ���� �߷� ����
    float m_verticalVelocity;
    bool m_isGrounded;

    // ���� ��� (Ŭ���ο�)
    int m_worldWidth;
    int m_worldHeight;

    // ������ ��ġ 
    // DirectX ��ũ�� ��ǥ��󿡼��� ��ġ
    Vector2 m_renderPosition;
};
