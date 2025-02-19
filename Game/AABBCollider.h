#pragma once
#include "Collider.h"


// AABB 충돌체 클래스
class AABBCollider : public Collider {
public:
    // x, y는 좌상단 월드좌표
    AABBCollider() : Collider(ColliderType::AABB), m_x(0), m_y(0), m_width(0), m_height(0) {}
    AABBCollider(int x, int y, int width, int height)
        : Collider(ColliderType::AABB), m_x(x), m_y(y), m_width(width), m_height(height) {}
    virtual ~AABBCollider() {}

    // AABB 충돌 검사: 다른 Collider와의 충돌 여부를 반환
    virtual bool CheckCollision(const Collider* other) const override;

    // AABB와 AABB 간의 충돌 검사
    bool CheckCollisionWithAABB(const AABBCollider* other) const;

    virtual void Update(const Transform& transform) override;
    virtual void Render(HDC hDC, int renderX, int renderY) const override;


    virtual std::string GetDebugString() const override;

    // Getter (필요에 따라 Setter 추가)
    int GetX() const { return m_x; }
    int GetY() const { return m_y; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    int m_x, m_y;         // 좌상단 좌표
    int m_width, m_height;  // 크기
};
