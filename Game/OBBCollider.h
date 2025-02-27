#pragma once
#include "Collider.h"

// OBB 충돌체 클래스 (회전 정보를 포함)
class OBBCollider : public Collider {
public:
    // 생성자: centerX, centerY는 중심 좌표, rotation은 회전 각도 (도 단위)
    // owner: 이 Collider를 소유한 GameObject
    OBBCollider(GameObject* owner, float centerX, float centerY, float width, float height, float rotation)
        : Collider(ColliderType::OBB, owner),
        m_x(centerX), m_y(centerY), m_width(width), m_height(height), m_rotation(rotation) {}
    virtual ~OBBCollider() {}

    // OBB 충돌 검사: Separating Axis Theorem (SAT) 기반 검사
    virtual bool CheckCollision(const Collider* other) const override;
    bool CheckCollisionWithOBB(const OBBCollider* other) const;

    virtual void Update(const Transform& transform) override;
    virtual void Render(HDC hDC, int renderX, int renderY) const override;
    
    virtual const Vector2<float> GetCenterPosition() const override;

    virtual std::string GetDebugString() const override;

    // Getter
    float GetX() const { return m_x; }
    float GetY() const { return m_y; }
    float GetWidth() const { return m_width; }
    float GetHeight() const { return m_height; }
    float GetRotation() const { return m_rotation; }

    virtual void SetRenderPosition(const Vector2<float>& pos) override;
    virtual Vector2<float> GetRenderPosition() const override;

private:
    float m_x, m_y;         // 중심 좌표
    float m_width, m_height;
    float m_rotation;       // 회전 각도 (도 단위)
};
