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


namespace CollisionQuery {
    bool Raycast(const Ray& ray, const Collider* collider, float maxDistance, float& outDistance, CollisionLayer queryMask = DEFAULT_COLLISION_LAYER_MASK);
    bool RaycastAABB(const Ray& ray, const AABBCollider* aabb, float maxDistance, float& outDistance);
    bool RaycastOBB(const Ray& ray, const OBBCollider* obb, float maxDistance, float& outDistance);

    bool OverlapBox(const Vector2<float>& center, float width, float height, float rotation, const Collider* collider, CollisionLayer queryMask);
}

