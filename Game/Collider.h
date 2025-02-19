#pragma once
#include "MathUtils.h"
#include <string>
#include <vector>
#include <utility>
#include <Windows.h>


// �浹ü ����
enum class ColliderType {
    AABB,
    OBB,
};

// �⺻ Collider Ŭ���� (�߻� Ŭ����)
class Collider {
public:
    Collider(ColliderType type) : m_type(type) {}
    virtual ~Collider() {}

    ColliderType GetType() const { return m_type; }

    // �� Collider ���� �浹 ���θ� �˻��ϴ� �߻� �Լ�
    virtual bool CheckCollision(const Collider* other) const = 0;


    virtual void Update(const Transform& transform) = 0;
    virtual void Render(HDC hDC, int renderX, int renderY) const = 0;


    // ����׿� ���ڿ� ��ȯ
    virtual std::string GetDebugString() const = 0;

protected:
    ColliderType m_type;
};