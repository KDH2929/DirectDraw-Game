#pragma once
#include "Collider.h"


// OBB 충돌체 클래스 (회전 정보를 포함)
class OBBCollider : public Collider {
public:
    OBBCollider(int centerX, int centerY, int width, int height, float rotation)
        : Collider(ColliderType::OBB),
        m_x(centerX), m_y(centerY), m_width(width), m_height(height), m_rotation(rotation) {}
    virtual ~OBBCollider() {}

    // OBB 충돌 검사: Separating Axis Theorem (SAT) 기반 검사
    virtual bool CheckCollision(const Collider* other) const override;
    bool CheckCollisionWithOBB(const OBBCollider* other) const;

    virtual void Update(const Transform& transform) override;
    virtual void Render(HDC hDC, int renderX, int renderY) const override;

    virtual std::string GetDebugString() const override;

    // Getter
    int GetX() const { return m_x; }
    int GetY() const { return m_y; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    float GetRotation() const { return m_rotation; }

private:
    int m_x, m_y;         // 중심 좌표
    int m_width, m_height;
    float m_rotation;       // 회전 각도 (도 단위)
};