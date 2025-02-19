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
        
        // AABB�� ȸ���� 0�� OBB�� �� �� ����.
        // ��, AABB�� m_x, m_y�� �»�� ��ǥ�̹Ƿ�, �̸� �߽� ��ǥ�� ��ȯ����� ��.
        int centerX = m_x + m_width / 2;
        int centerY = m_y + m_height / 2;

        OBBCollider aabbAsObb(centerX, centerY, m_width, m_height, 0.0f);
        const OBBCollider* otherOBB = static_cast<const OBBCollider*>(other);

        return aabbAsObb.CheckCollisionWithOBB(otherOBB);
    }   

    return false;
}


bool AABBCollider::CheckCollisionWithAABB(const AABBCollider* other) const {
    // �� AABB �浹 �˻�: �浹�� ������ false, ������ true
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
    // ��� ��带 �������� ���� (�ʿ��� ���)
    SetBkMode(hDC, TRANSPARENT);
    // �� ������ ���������� ���� (�ʿ��� ���)
    SetTextColor(hDC, RGB(0, 0, 0));

    /*
    ������

    ���� �÷��̾� ĳ���ʹ� RenderX, RenderY���� ���� ȭ�鿡 �������� �ϰ�
    ��ǥ�� PlayerPosX, PlayerPosY�� ���� ó����

    ���� �÷��̾� ĳ���Ϳ� ���ؼ� Render�Լ����� RenderX, RenderY�� ���� �ؾ� Collider�� �ð������� ���̰� ��

    */

    // AABB�� �»�� ��ǥ�� �ʺ�, ���̸� �̿��� �簢�� ������ ���
   
    int left = renderX;
    int top = renderY;
    int right = renderX + m_width;
    int bottom = renderY + m_height;

    // ���� ���õ� �귯�ø� �����ϰ�, NULL �귯�÷� �����Ͽ� ���� ä��⸦ ����
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));
    
    // �ܰ����� �׸���
    Rectangle(hDC, left, top, right, bottom);

    // ���� �귯�÷� ����
    SelectObject(hDC, hOldBrush);

}


std::string AABBCollider::GetDebugString() const {
    std::ostringstream oss;
    oss << "AABB(x=" << m_x << ", y=" << m_y
        << ", w=" << m_width << ", h=" << m_height << ")";
    return oss.str();
}