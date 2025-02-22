#include "stdafx.h"
#include "ColliderManager.h"
#include "AABBCollider.h"
#include "GameObject.h"
#include <algorithm>
#include <sstream>
#include <unordered_map>


ColliderManager::ColliderManager() {
}

ColliderManager::~ColliderManager() {
    ClearColliders();
}

void ColliderManager::AddCollider(Collider* collider) {
    m_colliders.push_back(collider);
}

void ColliderManager::RemoveCollider(Collider* collider) {
    auto it = std::find(m_colliders.begin(), m_colliders.end(), collider);
    if (it != m_colliders.end()) {
        m_colliders.erase(it);
    }
}

void ColliderManager::ClearColliders() {
    for (auto collider : m_colliders) {
        delete collider;
    }
    m_colliders.clear();
}

std::vector<std::pair<Collider*, Collider*>> ColliderManager::CheckAllCollisions() {
    std::vector<std::pair<Collider*, Collider*>> collisions;
    size_t count = m_colliders.size();
    for (size_t i = 0; i < count; ++i) {
        for (size_t j = i + 1; j < count; ++j) {
            if (m_colliders[i]->CheckCollision(m_colliders[j])) {
                collisions.push_back({ m_colliders[i], m_colliders[j] });
            }
        }
    }
    return collisions;
}


void ColliderManager::ProcessCollisions() {
    // ��� �浹 ���� ������
    auto collisions = CheckAllCollisions();

    // �� GameObject�� ���� ������ MTV(�ּ� �̵� ����)�� ������ ��
    std::unordered_map<GameObject*, Vector2> mtvMap;

    // ��� �浹 ���� ��ȸ�ϸ� ó��
    for (auto& pair : collisions) {
        Collider* colliderA = pair.first;
        Collider* colliderB = pair.second;
        GameObject* objA = colliderA->GetOwner();
        GameObject* objB = colliderB->GetOwner();

        if (objA && objB) {
            CollisionResponse responseA = colliderA->GetCollisionResponse();
            CollisionResponse responseB = colliderB->GetCollisionResponse();

            // �� �� �ϳ��� Block�̸� Block, �ƴϸ� Overlap, �� �� Ignore�̸� ����
            CollisionResponse finalResponse = CollisionResponse::Ignore;
            if (responseA == CollisionResponse::Block || responseB == CollisionResponse::Block)
                finalResponse = CollisionResponse::Block;
            else if (responseA == CollisionResponse::Overlap || responseB == CollisionResponse::Overlap)
                finalResponse = CollisionResponse::Overlap;

            if (finalResponse == CollisionResponse::Block) {
                // ComputeAABBMTV()�� �� Collider ���� �ּ� �̵� ����(MTV)�� ����ϴ� �Լ�
                Vector2 mtv = ComputeAABBMTV(colliderA, colliderB);

                // �� ��ü�� ������ ������ ������
                // ������ ������ ���� ������ ��ü�� ������ �ſ� ū(������ ����) ��ü�� ���� ����� �� ����
                // �� ���, ������ ��ü�� ������ �ſ� ū ��ü�� ��� ������ ������ 0���� ó������
                float invMassA = objA->GetInvMass();
                float invMassB = objB->GetInvMass();
                float invMassSum = invMassA + invMassB;


                //  m1/(m1+m2) ��� ������ ���
                // ������ ���� 0�� ���� �� �� �����̰ų� �浹 ó���� �� �ʿ� ���� ���
                if (invMassSum > 0.0f) {
                    // �� ��ü�� ���� MTV�� ���� ������ ���� �д��Ͽ� ������ ���
                    Vector2 correctionA = mtv * (invMassA / invMassSum);
                    Vector2 correctionB = mtv * (invMassB / invMassSum);
                    mtvMap[objA] = mtvMap[objA] + correctionA;
                    mtvMap[objB] = mtvMap[objB] - correctionB;
                }
            }
        }
    }

    // ������ MTV�� �� GameObject�� �� ���� ������
    for (auto& entry : mtvMap) {
        GameObject* obj = entry.first;
        Vector2 totalMTV = entry.second;
        obj->SetPosition(obj->GetPosition() + totalMTV);
    }
}




// AABB �浹�� ���� MTV(�ּ� �̵� ����)�� ����ϴ� �Լ�
// �� Collider ��ü�� AABB Ÿ���̶�� ������
Vector2 ColliderManager::ComputeAABBMTV(Collider* a, Collider* b) {
    // Collider�� AABB Ÿ������ Ȯ��
    AABBCollider* boxA = dynamic_cast<AABBCollider*>(a);
    AABBCollider* boxB = dynamic_cast<AABBCollider*>(b);
    if (!boxA || !boxB)
        return Vector2{ 0.0f, 0.0f };

    // �� �ڽ��� �»�� ��ǥ�� ũ��
    int aLeft = boxA->GetX();
    int aTop = boxA->GetY();
    int aRight = aLeft + boxA->GetWidth();
    int aBottom = aTop + boxA->GetHeight();

    int bLeft = boxB->GetX();
    int bTop = boxB->GetY();
    int bRight = bLeft + boxB->GetWidth();
    int bBottom = bTop + boxB->GetHeight();

    // �� �ڽ��� ��ġ�� ������ ���� ���� ���
    float overlapX = std::min<int>(aRight, bRight) - std::max<int>(aLeft, bLeft);
    float overlapY = std::min<int>(aBottom, bBottom) - std::max<int>(aTop, bTop);

    // ��ħ�� ���ٸ� (�� ���� �浹�� �������� �ʾҾ�� ��)
    if (overlapX < 0 || overlapY < 0)
        return Vector2{ 0.0f, 0.0f };

    // �� �ڽ��� �߽� ��ǥ ���
    float aCenterX = aLeft + boxA->GetWidth() / 2.0f;
    float aCenterY = aTop + boxA->GetHeight() / 2.0f;
    float bCenterX = bLeft + boxB->GetWidth() / 2.0f;
    float bCenterY = bTop + boxB->GetHeight() / 2.0f;

    // �� �߽� �� ���� ���� ��� (�� ��ü���� ��� ��ü�� ���� ���)
    Vector2 diff = { aCenterX - bCenterX, aCenterY - bCenterY };

    // �ּ� �̵� ����(MTV) ����: ��ħ�� ���� ���� �������� �̵�
    Vector2 mtv = { 0.0f, 0.0f };
    if (overlapX < overlapY) {
        // x�� �������� �и�
        mtv.x = (diff.x < 0) ? -overlapX : overlapX;
    }
    else {
        // y�� �������� �и�
        mtv.y = (diff.y < 0) ? -overlapY : overlapY;
    }
    return mtv;
}