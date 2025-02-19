#pragma once
#include "Collider.h"


// AABB �浹ü Ŭ����
class AABBCollider : public Collider {
public:
    // x, y�� �»�� ������ǥ
    AABBCollider() : Collider(ColliderType::AABB), m_x(0), m_y(0), m_width(0), m_height(0) {}
    AABBCollider(int x, int y, int width, int height)
        : Collider(ColliderType::AABB), m_x(x), m_y(y), m_width(width), m_height(height) {}
    virtual ~AABBCollider() {}

    // AABB �浹 �˻�: �ٸ� Collider���� �浹 ���θ� ��ȯ
    virtual bool CheckCollision(const Collider* other) const override;

    // AABB�� AABB ���� �浹 �˻�
    bool CheckCollisionWithAABB(const AABBCollider* other) const;

    virtual void Update(const Transform& transform) override;
    virtual void Render(HDC hDC, int renderX, int renderY) const override;


    virtual std::string GetDebugString() const override;

    // Getter (�ʿ信 ���� Setter �߰�)
    int GetX() const { return m_x; }
    int GetY() const { return m_y; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    int m_x, m_y;         // �»�� ��ǥ
    int m_width, m_height;  // ũ��
};
