#pragma once
#include "Collider.h"
#include <vector>
#include <utility>

struct CollisionInfo {
    GameObject* other;              // �浹�� ��� ��ü
    CollisionResponse response;     // �浹 ���� ����
    Vector2 normal;                 // �浹 ����
    Vector2 collisionPoint;         // �浹 ����
    float penetrationDepth;         // �浹 ����
};

class ColliderManager {
public:
    // �̱��� �ν��Ͻ� ���� �Լ�
    static ColliderManager& GetInstance() {
        static ColliderManager instance;
        return instance;
    }

    ColliderManager(const ColliderManager&) = delete;
    ColliderManager& operator=(const ColliderManager&) = delete;

    void AddCollider(Collider* collider);
    void RemoveCollider(Collider* collider);
    void ClearColliders();
    std::vector<std::pair<Collider*, Collider*>> CheckAllCollisions(); // ����Ž�� ���
    void ProcessCollisions();

    Vector2 ComputeAABBMTV(Collider* a, Collider* b);

    const std::vector<Collider*>& GetColliders() const;

private:
    // �̱����� ������/�Ҹ��ڰ� private
    ColliderManager();
    ~ColliderManager();

    std::vector<Collider*> m_colliders;
};
