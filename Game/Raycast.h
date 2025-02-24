#pragma once
#include "MathUtils.h"
#include "Collider.h"
#include "AABBCollider.h"
#include "OBBCollider.h"


// direction은 반드시 정규화된 값
class Ray {
public:
    Vector2<float> origin;
    Vector2<float> direction; // normalized

    Ray() : origin(0.0f, 0.0f), direction(0.0f, 1.0f) {}
    Ray(const Vector2<float>& origin, const Vector2<float>& direction)
        : origin(origin), direction(direction.Normalized()) {}
};


namespace Raycast {
    bool IntersectCollider(const Ray& ray, const Collider* collider, float& outDistance);
    bool IntersectAABB(const Ray& ray, const class AABBCollider* aabb, float& outDistance);
    bool IntersectOBB(const Ray& ray, const class OBBCollider* obb, float& outDistance);
}

