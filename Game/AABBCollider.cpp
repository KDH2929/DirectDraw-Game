#include "stdafx.h"
#include <algorithm>
#include <sstream>
#include "AABBCollider.h"
#include "OBBCollider.h"  // OBB 충돌 관련 함수 사용


bool AABBCollider::CheckCollision(const Collider* other) const {
    if (other->GetType() == ColliderType::AABB) {
        const AABBCollider* otherAABB = static_cast<const AABBCollider*>(other);
        return CheckCollisionWithAABB(otherAABB);
    }
    else if (other->GetType() == ColliderType::OBB) {
        // AABB는 회전이 0인 OBB로 볼 수 있음.
        // 단, AABB의 m_x, m_y는 좌상단 좌표이므로, 중심 좌표로 변환
        int centerX = m_x + m_width / 2;
        int centerY = m_y + m_height / 2;
        OBBCollider aabbAsObb(GetOwner(), centerX, centerY, m_width, m_height, 0.0f);
        const OBBCollider* otherOBB = static_cast<const OBBCollider*>(other);
        return aabbAsObb.CheckCollisionWithOBB(otherOBB);
    }
    return false;
}

bool AABBCollider::CheckCollisionWithAABB(const AABBCollider* other) const {
    return !(m_x + m_width < other->m_x ||
        other->m_x + other->m_width < m_x ||
        m_y + m_height < other->m_y ||
        other->m_y + other->m_height < m_y);
}

void AABBCollider::Update(const Transform& transform) {
    m_x = static_cast<int>(transform.position.x);
    m_y = static_cast<int>(transform.position.y);
}

void AABBCollider::Render(HDC hDC, int renderX, int renderY) const {
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(0, 0, 0));

    int left = renderX;
    int top = renderY;
    int right = renderX + m_width;
    int bottom = renderY + m_height;

    // 현재 선택된 브러시를 저장하고, NULL 브러시로 변경하여 내부 채우기를 제거
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));
    Rectangle(hDC, left, top, right, bottom);
    SelectObject(hDC, hOldBrush);
}

std::string AABBCollider::GetDebugString() const {
    std::ostringstream oss;
    oss << "AABB(x=" << m_x << ", y=" << m_y
        << ", w=" << m_width << ", h=" << m_height << ")";
    return oss.str();
}
