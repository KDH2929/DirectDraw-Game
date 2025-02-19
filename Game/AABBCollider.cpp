#include "stdafx.h"
#include "AABBCollider.h"
#include "OBBCollider.h"

#include <algorithm>
#include <sstream>


// AABBCollider Implementation


bool AABBCollider::CheckCollision(const Collider* other) const {
    if (other->GetType() == ColliderType::AABB) {
        const AABBCollider* otherAABB = static_cast<const AABBCollider*>(other);
        return CheckCollisionWithAABB(otherAABB);
    }

    else if (other->GetType() == ColliderType::OBB) {
        
        // AABB는 회전이 0인 OBB로 볼 수 있음.
        // 단, AABB의 m_x, m_y는 좌상단 좌표이므로, 이를 중심 좌표로 변환해줘야 함.
        int centerX = m_x + m_width / 2;
        int centerY = m_y + m_height / 2;

        OBBCollider aabbAsObb(centerX, centerY, m_width, m_height, 0.0f);
        const OBBCollider* otherOBB = static_cast<const OBBCollider*>(other);

        return aabbAsObb.CheckCollisionWithOBB(otherOBB);
    }   

    return false;
}


bool AABBCollider::CheckCollisionWithAABB(const AABBCollider* other) const {
    // 두 AABB 충돌 검사: 충돌이 없으면 false, 있으면 true
    return !(m_x + m_width < other->m_x ||
        other->m_x + other->m_width < m_x ||
        m_y + m_height < other->m_y ||
        other->m_y + other->m_height < m_y);
}

void AABBCollider::Update(const Transform& transform)
{
    m_x = static_cast<int>(transform.position.x);
    m_y = static_cast<int>(transform.position.y);
}

void AABBCollider::Render(HDC hDC, int renderX, int renderY) const
{
    // 배경 모드를 투명으로 설정 (필요한 경우)
    SetBkMode(hDC, TRANSPARENT);
    // 선 색상을 검정색으로 설정 (필요한 경우)
    SetTextColor(hDC, RGB(0, 0, 0));

    /*
    주의점

    현재 플레이어 캐릭터는 RenderX, RenderY값을 통해 화면에 렌더링을 하고
    좌표는 PlayerPosX, PlayerPosY로 따로 처리함

    따라서 플레이어 캐릭터에 대해선 Render함수에선 RenderX, RenderY를 통해 해야 Collider가 시각적으로 보이게 됨

    */

    // AABB의 좌상단 좌표와 너비, 높이를 이용해 사각형 영역을 계산
   
    int left = renderX;
    int top = renderY;
    int right = renderX + m_width;
    int bottom = renderY + m_height;

    // 현재 선택된 브러시를 저장하고, NULL 브러시로 변경하여 내부 채우기를 제거
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));
    
    // 외곽선만 그리기
    Rectangle(hDC, left, top, right, bottom);

    // 원래 브러시로 복원
    SelectObject(hDC, hOldBrush);

}


std::string AABBCollider::GetDebugString() const {
    std::ostringstream oss;
    oss << "AABB(x=" << m_x << ", y=" << m_y
        << ", w=" << m_width << ", h=" << m_height << ")";
    return oss.str();
}