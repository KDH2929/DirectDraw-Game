#include "stdafx.h"
#include "OBBCollider.h"
#include "AABBCollider.h"
#include "MathUtils.h"
#include <algorithm>
#include <sstream>

// OBB의 꼭짓점 계산
// box->GetX(), box->GetY()는 월드 좌표계에서의 중심 좌표라고 가정
static void ComputeOBBCorners(const OBBCollider* box, Vector2<float> corners[4]) {
    float halfWidth = box->GetWidth() / 2.0f;
    float halfHeight = box->GetHeight() / 2.0f;
    float rad = degToRad(box->GetRotation());

    // 회전된 로컬 기저벡터 (axisX, axisY)
    Vector2<float> axisX = { std::cos(rad), std::sin(rad) };
    Vector2<float> axisY = { -std::sin(rad), std::cos(rad) };

    // 월드 좌표계에서의 중심
    Vector2<float> center = { static_cast<float>(box->GetX()), static_cast<float>(box->GetY()) };

    // 4개 꼭짓점 계산 (시계방향)
    corners[0] = center + axisX * halfWidth + axisY * halfHeight;
    corners[1] = center - axisX * halfWidth + axisY * halfHeight;
    corners[2] = center - axisX * halfWidth - axisY * halfHeight;
    corners[3] = center + axisX * halfWidth - axisY * halfHeight;
}

// 주어진 축(axis)에 대해 4개 꼭짓점을 투영하여 최소/최대 값을 계산
static void ProjectOntoAxis(const Vector2<float> corners[4], const Vector2<float>& axis, float& minProj, float& maxProj) {
    minProj = maxProj = Dot(corners[0], axis);
    for (int i = 1; i < 4; i++) {
        float projection = Dot(corners[i], axis);
        minProj = std::min<float>(minProj, projection);
        maxProj = std::max<float>(maxProj, projection);
    }
}

// 다른 Collider 타입에 대해 분기
bool OBBCollider::CheckCollision(const Collider* other) const {

    // 충돌 필터 검사: 두 Collider의 레이어와 마스크가 서로 충돌 가능한지 확인
    if (((this->GetCollisionMask() & other->GetCollisionLayer()) == 0) || ((other->GetCollisionMask() & this->GetCollisionLayer()) == 0)) {
        return false;
    }


    if (other->GetType() == ColliderType::OBB) {
        const OBBCollider* otherOBB = static_cast<const OBBCollider*>(other);
        return CheckCollisionWithOBB(otherOBB);
    }
    else if (other->GetType() == ColliderType::AABB) {
        // 다른 Collider가 AABB인 경우,
        // AABB의 좌표는 좌상단 기준이므로 중심 좌표로 변환
        const AABBCollider* otherAABB = static_cast<const AABBCollider*>(other);
        float centerX = otherAABB->GetX() + otherAABB->GetWidth() / 2;
        float centerY = otherAABB->GetY() + otherAABB->GetHeight() / 2;
        // 회전이 0인 OBB로 변환하여 충돌 검사를 수행
        OBBCollider aabbAsObb(nullptr, centerX, centerY, otherAABB->GetWidth(), otherAABB->GetHeight(), 0.0f);
        return CheckCollisionWithOBB(&aabbAsObb);
    }
    return false;
}

// OBBCollider::CheckCollisionWithOBB 구현 (SAT 기반)
bool OBBCollider::CheckCollisionWithOBB(const OBBCollider* other) const {
    // 각 OBB의 꼭짓점 계산
    Vector2<float> cornersA[4], cornersB[4];
    ComputeOBBCorners(this, cornersA);
    ComputeOBBCorners(other, cornersB);

    // 자신의 회전축의 두 기저벡터와 other의 회전축의 두 기저벡터 (총 4축)
    float radA = degToRad(GetRotation());
    float radB = degToRad(other->GetRotation());
    Vector2<float> axes[4];
    axes[0] = Normalize(Vector2<float>{ std::cos(radA), std::sin(radA) });
    axes[1] = Normalize(Vector2<float>{ -std::sin(radA), std::cos(radA) });
    axes[2] = Normalize(Vector2<float>{ std::cos(radB), std::sin(radB) });
    axes[3] = Normalize(Vector2<float>{ -std::sin(radB), std::cos(radB) });

    // 각 축에 대해 투영된 구간이 겹치는지 확인
    for (int i = 0; i < 4; i++) {
        float minA, maxA, minB, maxB;
        ProjectOntoAxis(cornersA, axes[i], minA, maxA);
        ProjectOntoAxis(cornersB, axes[i], minB, maxB);

        // 한 축이라도 구간이 겹치지 않으면 충돌하지 않음
        if (maxA < minB || maxB < minA)
            return false;
    }
    // 모든 축에서 구간이 겹치면 충돌이 발생함
    return true;
}

void OBBCollider::Update(const Transform& transform)
{
    // Transform 정보를 이용해 중심 좌표를 업데이트
    // 필요시 transform에서 회전 값을 가져오기
    m_x = static_cast<float>(transform.position.x);
    m_y = static_cast<float>(transform.position.y);
}

void OBBCollider::Render(HDC hDC, int renderX, int renderY) const
{

}

const Vector2<float> OBBCollider::GetCenterPosition() const
{
    return Vector2<float>(m_x, m_y);
}

std::string OBBCollider::GetDebugString() const {
    std::ostringstream oss;
    oss << "OBB(center=(" << m_x << ", " << m_y << "), w=" << m_width
        << ", h=" << m_height << ", rot=" << m_rotation << ")";
    return oss.str();
}

void OBBCollider::SetRenderPosition(const Vector2<float>& pos)
{
    m_RenderPosition = pos;
}

Vector2<float> OBBCollider::GetRenderPosition() const
{
    return m_RenderPosition;
}