#include "stdafx.h"
#include "CollisionQuery.h"
#include <algorithm>
#include <climits>

namespace CollisionQuery {

    // queryMask: 충돌 검사 대상 레이어들의 비트마스크
    // 만약 (queryMask & collider->GetLayer())가 0이면, 이 collider는 검사 대상이 아님
    bool Raycast(const Ray& ray, const Collider* collider, float maxDistance, float& outDistance, CollisionLayer queryMask) {
        // Bitwise AND 연산을 통해 collider가 queryMask에 포함되어 있는지 확인
        if ((queryMask & collider->GetCollisionLayer()) == 0) {
            return false;
        }

        if (collider->GetCollisionResponse() == CollisionResponse::Ignore) {
            return false;
        }

        switch (collider->GetType()) {
        case ColliderType::AABB:
            return RaycastAABB(ray, static_cast<const AABBCollider*>(collider), maxDistance, outDistance);
        case ColliderType::OBB:
            return RaycastOBB(ray, static_cast<const OBBCollider*>(collider), maxDistance, outDistance);
        default:
            return false;
        }
    }

    bool RaycastAABB(const Ray& ray, const AABBCollider* aabb, float maxDistance, float& outDistance) {
        // AABB의 좌상단 기준 좌표와 크기를 이용해 최소/최대 좌표 계산
        Vector2<float> aabbMin(static_cast<float>(aabb->GetX()), static_cast<float>(aabb->GetY()));
        Vector2<float> aabbMax(static_cast<float>(aabb->GetX() + aabb->GetWidth()),
            static_cast<float>(aabb->GetY() + aabb->GetHeight()));

        // 광선의 원점이 AABB 내부에 있으면 즉시 충돌로 처리
        if (ray.origin.x >= aabbMin.x && ray.origin.x <= aabbMax.x &&
            ray.origin.y >= aabbMin.y && ray.origin.y <= aabbMax.y) {
            outDistance = 0.0f;
            return true;
        }


        /*
        Slab Test
        핵심 개념: 각 축(x, y)에서 박스의 최소/최대 평면과 광선이 교차하는 매개변수 t를 구하는 것
        직선의 매개변수 방정식 : P(t) = O + tD
         - P(t): 광선 위의 한 점
         - O: 광선의 시작점 (ray.origin)
         - D: 광선의 방향 벡터 (ray.direction)
         - t: 광선의 진행 매개변수 (ray가 진행하는 거리 비율)

        각 축의 AABB 평면과 교차하는 t값을 구하기   (위의 수식을 t에 대해 정리)
            tmin = (AABB_Min - ray.origin) / ray.direction
            tmax = (AABB_Max - ray.origin) / ray.direction
        tmin과 tmax는 교차 구간을 정의하며, 모든 축에서 공통 구간이 존재하면 충돌.

        변수명에서 t는 위 수식에서의 매개변수 t를 의미.

        */



        float txmin, txmax, tymin, tymax;


        // x 축 처리: Slab Test를 통해 광선과 AABB의 x축 평면 교차 여부를 계산 (Kay-Kajiya test)
        // 방향 벡터의 x 성분이 0인 경우, slab 교차 여부를 직접 계산할 수 없으므로 별도로 처리

        if (ray.direction.x != 0.0f) {
            // P(t) = O + tD 의  t값을 유도
            // t = ( P(t) - O ) / D    수식을 그대로 적은 것
            
            txmin = (aabbMin.x - ray.origin.x) / ray.direction.x;       // AABB Box의 x의 최솟값일 때의 t값 
            txmax = (aabbMax.x - ray.origin.x) / ray.direction.x;      // AABB Box의 y의 최솟값일 때의 t값

            // swap에 의해 뒷 부분의 t_enter는 항상 왼쪽에서 시작하는 것으로 고려하면 됨
            if (txmin > txmax)
            {
                std::swap(txmin, txmax);
            }
        }

        else {
            // x축 성분이 0인데 시작점의 x가 AABB 박스의 x범위를 벗어난다면 false
            // 왜냐하면 y축성분이 얼마든지 간에 평행하게 됨
            if (ray.origin.x < aabbMin.x || ray.origin.x > aabbMax.x)
                return false;

            // 값을 각각 음수 무한대, 양수 무한대로 두어 해당 값은 고려하지 않도록 함
            // 즉 해당 축에 대해서는 Slab 교차여부를 판정하지 않음
            txmin = -FLT_MAX;
            txmax = FLT_MAX;
        }

        // y 축 처리
        if (ray.direction.y != 0.0f) {

            // P(t) = O + tD 의  t값을 유도
            // t = ( P(t) - O ) / D    수식을 그대로 적은 것

            tymin = (aabbMin.y - ray.origin.y) / ray.direction.y;       // AABB Box의 y의 최솟값일 때의 t값 
            tymax = (aabbMax.y - ray.origin.y) / ray.direction.y;      // AABB Box의 y의 최댓값일 때의 t값 

            // swap에 의해 뒷 부분의 t_enter는 항상 왼쪽에서 시작하는 것으로 고려하면 됨
            if (tymin > tymax) {
                std::swap(tymin, tymax);
            }
        }
        else {

            // y축 성분이 0인데 시작점의 y가 AABB 박스 범위의 y를 벗어난다면 false
            // 왜냐하면 x축성분이 얼마든지 간에 평행하게 됨
            if (ray.origin.y < aabbMin.y || ray.origin.y > aabbMax.y)
                return false;

            // 값을 각각 음수 무한대, 양수 무한대로 두어 해당 값은 고려하지 않도록 함
            // 즉 해당 축에 대해서는 Slab 교차여부를 판정하지 않음
            tymin = -FLT_MAX;
            tymax = FLT_MAX;
        }

        // 두 축의 교차 구간이 겹치지 않으면 충돌 없음
        if ((txmin > tymax) || (tymin > txmax))
            return false;


        // 위에서 swap 처리를 하였기에 아래 로직이 항상 성립
        
        // tmin: AABB에 진입하는 순간의 매개변수 t값 (즉, AABB를 처음으로 만나는 지점)
        // tmax: AABB에서 이탈하는 순간의 매개변수 t값 (즉, AABB를 마지막으로 만나는 지점)

        float t_enter, t_exit;

        // 최종 tmin, tmax 결정
        t_enter = std::max<float>(txmin, tymin);
        t_exit = std::min<float>(txmax, tymax);


        // Ray가 AABB 뒤쪽에서 시작하는 경우
        if (t_exit < 0)
            return false;


        // 충돌이 maxDistance 이내인지 확인
        if (t_enter <= maxDistance) {
            outDistance = t_enter;
            return true;
        }

        return false;
    }

    bool RaycastOBB(const Ray& ray, const OBBCollider* obb, float maxDistance, float& outDistance) {
        // OBB는 AABB와 원리는 동일하고, 대신 회전된 축을 기준좌표계로 하여 계산하면 됨
        // 즉 Ray의 원점과 방향을 회전된 축을 기준으로 맞춰서 계산
        
        // OBB를 AABB 좌표계로 변환
        float rad = degToRad(obb->GetRotation());
        float cosR = std::cos(rad);
        float sinR = std::sin(rad);

        // OBB의 중심 (월드 좌표)
        Vector2<float> obbCenter(static_cast<float>(obb->GetX()), static_cast<float>(obb->GetY()));

        float halfWidth = obb->GetWidth() / 2.0f;
        float halfHeight = obb->GetHeight() / 2.0f;

        // 광선의 원점을 OBB 중심 기준으로 변환
        Vector2<float> localOrigin = ray.origin - obbCenter;
        // 광선의 원점을 OBB의 로컬 좌표계로 변환
        Vector2<float> transformedOrigin(
            localOrigin.x * cosR + localOrigin.y * sinR,
            -localOrigin.x * sinR + localOrigin.y * cosR
        );

        // OBB의 로컬 AABB 범위 (중심이 0,0인 정렬된 AABB)
        Vector2<float> localMin(-halfWidth, -halfHeight);
        Vector2<float> localMax(halfWidth, halfHeight);

        // 광선의 시작점이 OBB 내부에 있으면 바로 충돌로 처리
        if (transformedOrigin.x >= localMin.x && transformedOrigin.x <= localMax.x &&
            transformedOrigin.y >= localMin.y && transformedOrigin.y <= localMax.y) {
            outDistance = 0.0f;
            return true;
        }

        // 광선 방향도 OBB의 로컬 좌표계로 변환
        Vector2<float> transformedDirection(
            ray.direction.x * cosR + ray.direction.y * sinR,
            -ray.direction.x * sinR + ray.direction.y * cosR
        );

        float txmin, txmax, tymin, tymax;

        // X축 Slab Test
        if (transformedDirection.x != 0.0f) {
            txmin = (localMin.x - transformedOrigin.x) / transformedDirection.x;
            txmax = (localMax.x - transformedOrigin.x) / transformedDirection.x;
            if (txmin > txmax) std::swap(txmin, txmax);
        }
        else {
            if (transformedOrigin.x < localMin.x || transformedOrigin.x > localMax.x)
                return false;
            txmin = -FLT_MAX;
            txmax = FLT_MAX;
        }

        // Y축 Slab Test
        if (transformedDirection.y != 0.0f) {
            tymin = (localMin.y - transformedOrigin.y) / transformedDirection.y;
            tymax = (localMax.y - transformedOrigin.y) / transformedDirection.y;
            if (tymin > tymax) std::swap(tymin, tymax);
        }
        else {
            if (transformedOrigin.y < localMin.y || transformedOrigin.y > localMax.y)
                return false;
            tymin = -FLT_MAX;
            tymax = FLT_MAX;
        }

        // Slab 교차 구간 확인
        if ((txmin > tymax) || (tymin > txmax))
            return false;

        // 최종 진입/이탈 지점 계산
        float t_enter = std::max<float>(txmin, tymin);
        float t_exit = std::min<float>(txmax, tymax);

        // Ray가 OBB 뒤쪽에서 시작하는 경우
        if (t_exit < 0)
            return false;

        // 충돌이 maxDistance 이내인지 확인
        if (t_enter <= maxDistance) {
            outDistance = t_enter;
            return true;
        }

        return false;
    }




    bool OverlapBox(const Vector2<float>& center, float width, float height, float rotation, const Collider* collider, CollisionLayer queryMask)
    {
        // QueryMask 필터
        if ((collider->GetCollisionLayer() & queryMask) == 0) {
            return false;
        }

        if (collider->GetCollisionResponse() == CollisionResponse::Ignore) {
            return false;
        }

        OBBCollider queryOBB(nullptr, center.x, center.y, width, height, rotation);

        return queryOBB.CheckCollision(collider);
    }

}
