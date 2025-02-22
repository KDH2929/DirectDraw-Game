#pragma once
#include "Monster.h"
#include "CentipedeAnim.h"  

// ���� ����
class CImageData;
class SpriteManager;

class Centipede : public Monster {
public:
    // ������: ���� ��ǥ�� ��������Ʈ �����͸� ������
    Centipede(CImageData* pMonsterImgData, float startX, float startY);
    virtual ~Centipede();

    // �� ������ ������Ʈ
    virtual void Update(float deltaTime) override;
    // �� ������ ������
    virtual void Render(CDDrawDevice* pDevice) override;

    int GetHealth() const { return m_health; }
    void SetHealth(int health) { m_health = health; }
    int GetSpeed() const { return m_speed; }
    void SetSpeed(int speed) { m_speed = speed; }

    // ������ ��ġ ���� �Լ�
    void SetRenderPosition(const Vector2& pos);
    Vector2 GetRenderPosition() const;

    int GetSpriteFrameWidth() const;
    int GetSpriteFrameHeight() const;

    virtual MonsterType GetMonsterType() const override;

    // SetPosition�� GameObject�� ���� �״�� ���
    virtual void SetPosition(const Vector2& newPos) override {
        GameObject::SetPosition(newPos);
    }

private:
    // Component �ʱ�ȭ ���� �Լ���
    void InitCollider(int x, int y, int width, int height);
    void InitSpriteManager(CImageData* spriteSheet, int frameWidth, int frameHeight);
    void InitAnimation();

    // �ִϸ��̼� ���� ��ȯ ���� (FSM)
    void UpdateAnimFSM();

private:
    int m_health;
    int m_speed;

    // �ִϸ��̼� ���� ���� ���
    SpriteManager* m_spriteManager;
    CentipedeAnim* m_anim;     // CentipedeAnim�� ���ø� ����� SpriteAnim<CentipedeAnimState>

    // Centipede ���� ���� (��: �߷�, �̵� ����)
    float m_verticalVelocity;

    // ������ ��ġ (DirectX ��ũ�� ��ǥ�� ��)
    Vector2 m_renderPosition;
};
