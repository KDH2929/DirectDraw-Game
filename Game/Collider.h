#pragma once
#include "MathUtils.h"
#include <string>
#include <Windows.h>

// �浹ü ����
enum class ColliderType {
    AABB,
    OBB,
};

// �浹 ���� Ÿ��
enum class CollisionResponse {
    Block,
    Overlap,
    Ignore
};

// ���� ����
class GameObject;


// �⺻ Collider Ŭ���� (�߻� Ŭ����)
class Collider {
public:
    // owner: �� Collider�� ������ GameObject  
    // response: �⺻ �浹 ���� (�⺻������ Block)
    Collider(ColliderType type, GameObject* owner, CollisionResponse response = CollisionResponse::Block)
        : m_type(type), m_owner(owner), m_response(response) {}
    virtual ~Collider() {}

    ColliderType GetType() const { return m_type; }
    GameObject* GetOwner() const { return m_owner; }
    CollisionResponse GetCollisionResponse() const { return m_response; }

    // �� Collider ���� �浹 ���θ� �˻��ϴ� �߻� �Լ�
    virtual bool CheckCollision(const Collider* other) const = 0;
    virtual void Update(const Transform& transform) = 0;
    virtual void Render(HDC hDC, int renderX, int renderY) const = 0;

    // ����׿� ���ڿ� ��ȯ
    virtual std::string GetDebugString() const = 0;


    // ������ ��ġ ���� �Լ�
    // ������ ��ġ�� ���� ��ǥ�� ���� ��ġ�� ���Ӽ��迡 ���� ���� ���� �ٸ� ���� ����
    virtual void SetRenderPosition(const Vector2<float>& pos) = 0;
    virtual Vector2<float> GetRenderPosition() const = 0;


protected:
    ColliderType m_type;
    GameObject* m_owner;
    CollisionResponse m_response;

    // ������ ��ġ 
    // DirectX ��ũ�� ��ǥ��󿡼��� ��ġ
    Vector2<float> m_RenderPosition;
};
