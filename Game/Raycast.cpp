#include "stdafx.h"
#include "Raycast.h"
#include <algorithm>

namespace Raycast {

    bool IntersectCollider(const Ray& ray, const Collider* collider, float& outDistance) {
        switch (collider->GetType()) {
        case ColliderType::AABB:
            return IntersectAABB(ray, static_cast<const AABBCollider*>(collider), outDistance);
        case ColliderType::OBB:
            return IntersectOBB(ray, static_cast<const OBBCollider*>(collider), outDistance);
        default:
            return false;
        }
    }

    bool IntersectAABB(const Ray& ray, const AABBCollider* aabb, float& outDistance) {
        // AABB�� �»�� ���� ��ǥ�� ũ�⸦ �̿��� �ּ�/�ִ� ��ǥ�� ����մϴ�.
        Vector2 aabbMin(static_cast<float>(aabb->GetX()), static_cast<float>(aabb->GetY()));
        Vector2 aabbMax(static_cast<float>(aabb->GetX() + aabb->GetWidth()), static_cast<float>(aabb->GetY() + aabb->GetHeight()));

        // ���� ������: ray.origin + t * ray.direction ���� t���� ����.
        float tmin = (aabbMin.x - ray.origin.x) / ray.direction.x;
        float tmax = (aabbMax.x - ray.origin.x) / ray.direction.x;
        if (tmin > tmax) std::swap(tmin, tmax);

        float tymin = (aabbMin.y - ray.origin.y) / ray.direction.y;
        float tymax = (aabbMax.y - ray.origin.y) / ray.direction.y;
        if (tymin > tymax) std::swap(tymin, tymax);

        // �� �࿡���� ���� ������ ��ġ�� ������ �浹���� ����
        if ((tmin > tymax) || (tymin > tmax))
            return false;

        // �� ���� ���� ������ ���� ������ ���
        tmin = (tymin > tmin) ? tymin : tmin;
        tmax = (tymax < tmax) ? tymax : tmax;

        if (tmax < 0)  // ������ AABB ���ʿ��� �����ϴ� ���
            return false;

        outDistance = (tmin >= 0) ? tmin : tmax;
        return true;
    }


    bool IntersectOBB(const Ray& ray, const OBBCollider* obb, float& outDistance) {
        // OBB�� ȸ���� AABB�̹Ƿ�, ���� OBB�� �߽ɰ� ȸ�� ������ �̿���
        // ������ OBB�� ���� ��ǥ��� ��ȯ
        float rad = degToRad(obb->GetRotation());
        float cosR = std::cos(rad);
        float sinR = std::sin(rad);

        // OBB�� �߽� (���� ��ǥ)
        Vector2 obbCenter(static_cast<float>(obb->GetX()), static_cast<float>(obb->GetY()));
        // ��ġ��
        float halfWidth = obb->GetWidth() / 2.0f;
        float halfHeight = obb->GetHeight() / 2.0f;

        // ������ ������ OBB �߽� �������� ��ȯ
        Vector2 localOrigin = ray.origin - obbCenter;
        // OBB�� ���� ��ǥ��� ��ȯ (��ȸ��)
        Vector2 transformedOrigin(
            localOrigin.x * cosR + localOrigin.y * sinR,
            -localOrigin.x * sinR + localOrigin.y * cosR
        );
        Vector2 transformedDirection(
            ray.direction.x * cosR + ray.direction.y * sinR,
            -ray.direction.x * sinR + ray.direction.y * cosR
        );
        // ���� OBB�� ���� AABB�� �߽��� (0,0)�̰� �ּ� ��ǥ(-halfWidth, -halfHeight), �ִ� ��ǥ(halfWidth, halfHeight)
        Vector2 localMin(-halfWidth, -halfHeight);
        Vector2 localMax(halfWidth, halfHeight);

        float tmin = (localMin.x - transformedOrigin.x) / transformedDirection.x;
        float tmax = (localMax.x - transformedOrigin.x) / transformedDirection.x;
        if (tmin > tmax) std::swap(tmin, tmax);

        float tymin = (localMin.y - transformedOrigin.y) / transformedDirection.y;
        float tymax = (localMax.y - transformedOrigin.y) / transformedDirection.y;
        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        tmin = (tymin > tmin) ? tymin : tmin;
        tmax = (tymax < tmax) ? tymax : tmax;

        if (tmax < 0)
            return false;

        outDistance = (tmin >= 0) ? tmin : tmax;
        return true;
    }
}
