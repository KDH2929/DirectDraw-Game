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
        // AABB의 좌상단 기준 좌표와 크기를 이용해 최소/최대 좌표를 계산합니다.
        Vector2 aabbMin(static_cast<float>(aabb->GetX()), static_cast<float>(aabb->GetY()));
        Vector2 aabbMax(static_cast<float>(aabb->GetX() + aabb->GetWidth()), static_cast<float>(aabb->GetY() + aabb->GetHeight()));

        // 레이 방정식: ray.origin + t * ray.direction 에서 t값을 구함.
        float tmin = (aabbMin.x - ray.origin.x) / ray.direction.x;
        float tmax = (aabbMax.x - ray.origin.x) / ray.direction.x;
        if (tmin > tmax) std::swap(tmin, tmax);

        float tymin = (aabbMin.y - ray.origin.y) / ray.direction.y;
        float tymax = (aabbMax.y - ray.origin.y) / ray.direction.y;
        if (tymin > tymax) std::swap(tymin, tymax);

        // 두 축에서의 교차 구간이 겹치지 않으면 충돌하지 않음
        if ((tmin > tymax) || (tymin > tmax))
            return false;

        // 두 축의 교차 구간의 공통 영역을 계산
        tmin = (tymin > tmin) ? tymin : tmin;
        tmax = (tymax < tmax) ? tymax : tmax;

        if (tmax < 0)  // 광선이 AABB 뒤쪽에서 시작하는 경우
            return false;

        outDistance = (tmin >= 0) ? tmin : tmax;
        return true;
    }


    bool IntersectOBB(const Ray& ray, const OBBCollider* obb, float& outDistance) {
        // OBB는 회전된 AABB이므로, 먼저 OBB의 중심과 회전 정보를 이용해
        // 광선을 OBB의 로컬 좌표계로 변환
        float rad = degToRad(obb->GetRotation());
        float cosR = std::cos(rad);
        float sinR = std::sin(rad);

        // OBB의 중심 (월드 좌표)
        Vector2 obbCenter(static_cast<float>(obb->GetX()), static_cast<float>(obb->GetY()));
        // 반치수
        float halfWidth = obb->GetWidth() / 2.0f;
        float halfHeight = obb->GetHeight() / 2.0f;

        // 광선의 원점을 OBB 중심 기준으로 변환
        Vector2 localOrigin = ray.origin - obbCenter;
        // OBB의 로컬 좌표계로 변환 (역회전)
        Vector2 transformedOrigin(
            localOrigin.x * cosR + localOrigin.y * sinR,
            -localOrigin.x * sinR + localOrigin.y * cosR
        );
        Vector2 transformedDirection(
            ray.direction.x * cosR + ray.direction.y * sinR,
            -ray.direction.x * sinR + ray.direction.y * cosR
        );
        // 이제 OBB의 로컬 AABB는 중심이 (0,0)이고 최소 좌표(-halfWidth, -halfHeight), 최대 좌표(halfWidth, halfHeight)
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
