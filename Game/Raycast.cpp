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
        // AABB�� �»�� ���� ��ǥ�� ũ�⸦ �̿��� �ּ�/�ִ� ��ǥ ���
        Vector2<float> aabbMin(static_cast<float>(aabb->GetX()), static_cast<float>(aabb->GetY()));
        Vector2<float> aabbMax(static_cast<float>(aabb->GetX() + aabb->GetWidth()),
            static_cast<float>(aabb->GetY() + aabb->GetHeight()));

        // ���� ������ AABB ���ο� ������ ��� �浹�� ó��
        if (ray.origin.x >= aabbMin.x && ray.origin.x <= aabbMax.x &&
            ray.origin.y >= aabbMin.y && ray.origin.y <= aabbMax.y) {
            outDistance = 0.0f;
            return true;
        }

        float tmin, tmax, tymin, tymax;

        // x �� ó��: ���� ������ x ������ 0�� ���� slab ���� ���η� �Ǵ�
        if (ray.direction.x != 0.0f) {
            tmin = (aabbMin.x - ray.origin.x) / ray.direction.x;
            tmax = (aabbMax.x - ray.origin.x) / ray.direction.x;
            if (tmin > tmax) std::swap(tmin, tmax);
        }
        else {
            // ������ x ������ 0�̸�, ������ AABB slab ���� �־�� ��
            if (ray.origin.x < aabbMin.x || ray.origin.x > aabbMax.x)
                return false;
            tmin = -std::numeric_limits<float>::infinity();
            tmax = std::numeric_limits<float>::infinity();
        }

        // y �� ó��
        if (ray.direction.y != 0.0f) {
            tymin = (aabbMin.y - ray.origin.y) / ray.direction.y;
            tymax = (aabbMax.y - ray.origin.y) / ray.direction.y;
            if (tymin > tymax) std::swap(tymin, tymax);
        }
        else {
            if (ray.origin.y < aabbMin.y || ray.origin.y > aabbMax.y)
                return false;
            tymin = -std::numeric_limits<float>::infinity();
            tymax = std::numeric_limits<float>::infinity();
        }

        // �� ���� ���� ������ ��ġ�� ������ �浹 ����
        if ((tmin > tymax) || (tymin > tmax))
            return false;

        // �� ���� ���� ���� ���� ���
        tmin = (tymin > tmin) ? tymin : tmin;
        tmax = (tymax < tmax) ? tymax : tmax;

        if (tmax < 0)  // ���̰� AABB ���ʿ��� �����ϴ� ���
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
        Vector2<float> obbCenter(static_cast<float>(obb->GetX()), static_cast<float>(obb->GetY()));
        
        float halfWidth = obb->GetWidth() / 2.0f;
        float halfHeight = obb->GetHeight() / 2.0f;

        // ������ ������ OBB �߽� �������� ��ȯ
        Vector2<float> localOrigin = ray.origin - obbCenter;
        // OBB�� ���� ��ǥ��� ��ȯ (��ȸ��)
        Vector2<float> transformedOrigin(
            localOrigin.x * cosR + localOrigin.y * sinR,
            -localOrigin.x * sinR + localOrigin.y * cosR
        );
        Vector2<float> transformedDirection(
            ray.direction.x * cosR + ray.direction.y * sinR,
            -ray.direction.x * sinR + ray.direction.y * cosR
        );
        // ���� OBB�� ���� AABB�� �߽��� (0,0)�̰� �ּ� ��ǥ(-halfWidth, -halfHeight), �ִ� ��ǥ(halfWidth, halfHeight)
        Vector2<float> localMin(-halfWidth, -halfHeight);
        Vector2<float> localMax(halfWidth, halfHeight);

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
