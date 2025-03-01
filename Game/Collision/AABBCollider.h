#pragma once
#include "Collider.h"

// AABB 충돌체 클래스
class AABBCollider : public Collider {
public:
    // owner: 이 Collider를 소유한 GameObject  
    // 기본값: 좌상단 좌표 0, 크기도 0
    AABBCollider(GameObject* owner, int x = 0, int y = 0, int width = 0, int height = 0)
        : Collider(ColliderType::AABB, owner), m_x(x), m_y(y), m_width(width), m_height(height) {}
    virtual ~AABBCollider() {}

    virtual bool CheckCollision(const Collider* other) const override;
    bool CheckCollisionWithAABB(const AABBCollider* other) const;
    virtual void Update(const Transform& transform) override;
    virtual void Render(HDC hDC, int renderX, int renderY) const override;
    virtual std::string GetDebugString() const override;

    // Getter
    int GetX() const { return m_x; }
    int GetY() const { return m_y; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    // 중심 좌표 반환 함수 (좌상단 기준)
    virtual const Vector2<float> GetCenterPosition() const override;


    virtual void SetRenderPosition(const Vector2<float>& pos) override;
    virtual Vector2<float> GetRenderPosition() const override;

private:
    int m_x, m_y;         // 좌상단 좌표
    int m_width, m_height;  // 크기
};
