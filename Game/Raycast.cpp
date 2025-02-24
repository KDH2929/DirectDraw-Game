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
        // AABB의 좌상단 기준 좌표와 크기를 이용해 최소/최대 좌표 계산
        Vector2<float> aabbMin(static_cast<float>(aabb->GetX()), static_cast<float>(aabb->GetY()));
        Vector2<float> aabbMax(static_cast<float>(aabb->GetX() + aabb->GetWidth()),
            static_cast<float>(aabb->GetY() + aabb->GetHeight()));

        // 레이 원점이 AABB 내부에 있으면 즉시 충돌로 처리
        if (ray.origin.x >= aabbMin.x && ray.origin.x <= aabbMax.x &&
            ray.origin.y >= aabbMin.y && ray.origin.y <= aabbMax.y) {
            outDistance = 0.0f;
            return true;
        }

        float tmin, tmax, tymin, tymax;

        // x 축 처리: 방향 벡터의 x 성분이 0인 경우는 slab 내부 여부로 판단
        if (ray.direction.x != 0.0f) {
            tmin = (aabbMin.x - ray.origin.x) / ray.direction.x;
            tmax = (aabbMax.x - ray.origin.x) / ray.direction.x;
            if (tmin > tmax) std::swap(tmin, tmax);
        }
        else {
            // 레이의 x 방향이 0이면, 원점이 AABB slab 내에 있어야 함
            if (ray.origin.x < aabbMin.x || ray.origin.x > aabbMax.x)
                return false;
            tmin = -std::numeric_limits<float>::infinity();
            tmax = std::numeric_limits<float>::infinity();
        }

        // y 축 처리
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

        // 두 축의 교차 구간이 겹치지 않으면 충돌 없음
        if ((tmin > tymax) || (tymin > tmax))
            return false;

        // 두 축의 공통 교차 구간 계산
        tmin = (tymin > tmin) ? tymin : tmin;
        tmax = (tymax < tmax) ? tymax : tmax;

        if (tmax < 0)  // 레이가 AABB 뒤쪽에서 시작하는 경우
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
        Vector2<float> obbCenter(static_cast<float>(obb->GetX()), static_cast<float>(obb->GetY()));
        
        float halfWidth = obb->GetWidth() / 2.0f;
        float halfHeight = obb->GetHeight() / 2.0f;

        // 광선의 원점을 OBB 중심 기준으로 변환
        Vector2<float> localOrigin = ray.origin - obbCenter;
        // OBB의 로컬 좌표계로 변환 (역회전)
        Vector2<float> transformedOrigin(
            localOrigin.x * cosR + localOrigin.y * sinR,
            -localOrigin.x * sinR + localOrigin.y * cosR
        );
        Vector2<float> transformedDirection(
            ray.direction.x * cosR + ray.direction.y * sinR,
            -ray.direction.x * sinR + ray.direction.y * cosR
        );
        // 이제 OBB의 로컬 AABB는 중심이 (0,0)이고 최소 좌표(-halfWidth, -halfHeight), 최대 좌표(halfWidth, halfHeight)
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
