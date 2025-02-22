#pragma once
#include "Collider.h"
#include <vector>
#include <utility>

struct CollisionInfo {
    GameObject* other;              // 충돌한 상대 객체
    CollisionResponse response;     // 충돌 반응 유형
    Vector2 normal;                 // 충돌 법선
    Vector2 collisionPoint;         // 충돌 지점
    float penetrationDepth;         // 충돌 깊이
};

class ColliderManager {
public:
    // 싱글톤 인스턴스 접근 함수
    static ColliderManager& GetInstance() {
        static ColliderManager instance;
        return instance;
    }

    ColliderManager(const ColliderManager&) = delete;
    ColliderManager& operator=(const ColliderManager&) = delete;

    void AddCollider(Collider* collider);
    void RemoveCollider(Collider* collider);
    void ClearColliders();
    std::vector<std::pair<Collider*, Collider*>> CheckAllCollisions(); // 완전탐색 기반
    void ProcessCollisions();

    Vector2 ComputeAABBMTV(Collider* a, Collider* b);

    const std::vector<Collider*>& GetColliders() const;

private:
    // 싱글톤은 생성자/소멸자가 private
    ColliderManager();
    ~ColliderManager();

    std::vector<Collider*> m_colliders;
};
