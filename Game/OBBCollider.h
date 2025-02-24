#pragma once
#include "Collider.h"

// OBB �浹ü Ŭ���� (ȸ�� ������ ����)
class OBBCollider : public Collider {
public:
    // ������: centerX, centerY�� �߽� ��ǥ, rotation�� ȸ�� ���� (�� ����)
    // owner: �� Collider�� ������ GameObject
    OBBCollider(GameObject* owner, int centerX, int centerY, int width, int height, float rotation)
        : Collider(ColliderType::OBB, owner),
        m_x(centerX), m_y(centerY), m_width(width), m_height(height), m_rotation(rotation) {}
    virtual ~OBBCollider() {}

    // OBB �浹 �˻�: Separating Axis Theorem (SAT) ��� �˻�
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

    virtual void SetRenderPosition(const Vector2<float>& pos) override;
    virtual Vector2<float> GetRenderPosition() const override;

private:
    int m_x, m_y;         // �߽� ��ǥ
    int m_width, m_height;
    float m_rotation;       // ȸ�� ���� (�� ����)
};
