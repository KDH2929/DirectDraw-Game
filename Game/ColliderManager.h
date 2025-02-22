#pragma once
#include "Collider.h"
#include <vector>
#include <utility>

class ColliderManager {
public:
    ColliderManager();
    ~ColliderManager();

    void AddCollider(Collider* collider);
    void RemoveCollider(Collider* collider);
    void ClearColliders();
    std::vector<std::pair<Collider*, Collider*>> CheckAllCollisions();          // 현재는 완전탐색 기반
    void ProcessCollisions();

    Vector2 ComputeAABBMTV(Collider* a, Collider* b);

private:
    std::vector<Collider*> m_colliders;
};
