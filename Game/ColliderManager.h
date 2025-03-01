#pragma once
#include "Collider.h"
#include <vector>
#include <utility>

struct CollisionInfo {
    GameObject* other;
    CollisionResponse response;
    Vector2<float> normal;
    Vector2<float> collisionPoint;
    float penetrationDepth;

    // 구조체 생성자
    CollisionInfo(GameObject* _other = nullptr,
        CollisionResponse _response = CollisionResponse::Ignore,
        const Vector2<float>& _normal = Vector2<float>(0.0f, 0.0f),
        const Vector2<float>& _collisionPoint = Vector2<float>(0.0f, 0.0f),
        float _penetrationDepth = 0.0f)
        : other(_other),
        response(_response),
        normal(_normal),
        collisionPoint(_collisionPoint),
        penetrationDepth(_penetrationDepth)
    {}

};

class ColliderManager {
public:
    // 포인터 기반 싱글톤 인스턴스 접근 함수
    static ColliderManager* GetInstance();

    void Initialize();      // Stage 시작 시 호출
    static void DestroyInstance();      // Stage 종료 시 호출

    ColliderManager(const ColliderManager&) = delete;
    ColliderManager& operator=(const ColliderManager&) = delete;

    void AddCollider(Collider* collider);
    void RemoveCollider(Collider* collider);
    void ClearColliders();
    std::vector<std::pair<Collider*, Collider*>> CheckAllCollisions(); // 완전탐색 기반
    void ProcessCollisions();

    Vector2<float> ComputeAABBMTV(Collider* a, Collider* b);

    const std::vector<Collider*>& GetAllColliders() const;

private:
    // 싱글톤은 생성자/소멸자가 private
    ColliderManager();
    ~ColliderManager();

    static ColliderManager* s_instance;

    std::vector<Collider*> m_colliders;

};
